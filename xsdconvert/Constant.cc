/******************************************************************************
 * Copyright (c) 2000-2019 Ericsson Telecom AB
 * All rights reserved. This program and the accompanying materials
 * are made available under the terms of the Eclipse Public License v2.0
 * which accompanies this distribution, and is available at
 * https://www.eclipse.org/org/documents/epl-2.0/EPL-2.0.html
 *
 * Contributors:
 *   Szabo, Bence Janos
 *
 ******************************************************************************/
#include "Constant.hh"
#include "ComplexType.hh"


Constant::Constant(SimpleType* p_parent, Mstring p_type, Mstring p_value)
  : RootType(p_parent->getParser(), p_parent->getModule(), p_parent->getConstruct())
{
  parent = p_parent;
  type.upload(p_type, false);
  value = p_value;
  alterego = NULL;
  checked = false;
  unsupported = false;
}

void Constant::nameConversion(const NameConversionMode conversion_mode, const List<NamespaceType> & ns) {
  switch (conversion_mode) {
    case nameMode:
      // Nameconversion of the constants name is postponed to finalFinalModification
      break;
    case typeMode:
      nameConversion_types(ns);
      break;
    case fieldMode:
      break;
  }
}

void Constant::nameConversion_types(const List<NamespaceType> & ns) {

  Mstring prefix = type.convertedValue.getPrefix(':');
  Mstring value_str = type.convertedValue.getValueWithoutPrefix(':');

  Mstring uri;
  for (List<NamespaceType>::iterator namesp = ns.begin(); namesp; namesp = namesp->Next) {
    if (prefix == namesp->Data.prefix) {
      uri = namesp->Data.uri;
      break;
    }
  }

  QualifiedName tmp(uri, value_str);

  QualifiedNames::iterator origTN = TTCN3ModuleInventory::getInstance().getTypenames().begin();
  for (; origTN; origTN = origTN->Next) {
    if (tmp == origTN->Data) {
      QualifiedName tmp_name(module->getTargetNamespace(), name.convertedValue);
      if (tmp_name == origTN->Data)
        continue; // get a new type name
      else
        break;
    }
  }
  if (origTN != NULL) {
    setTypeValue(origTN->Data.name);
    // This      ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^ is always value_str
    // The only effect here is to remove the "xs:" prefix from type.convertedValue,
    // otherwise the new value is always the same as the old.
  } else {
    Mstring res, var;
    XSDName2TTCN3Name(value_str, empty_string, TTCN3ModuleInventory::getInstance().getTypenames(), type_reference_name, res, var);
    setTypeValue(res);
  }
}

void Constant::finalModification() {
  if (checked) return;
  Mstring tmp_type;
  // Find the buildInType which the type is derived from
  if (isBuiltInType(type.originalValueWoPrefix)) {
    tmp_type = type.originalValueWoPrefix;
  } else if (!parent->getReference().empty() && parent->getReference().get_ref() != NULL) {
    SimpleType * st = (SimpleType*)parent->getReference().get_ref();
    if (isBuiltInType(st->getBuiltInBase())) {
      tmp_type = st->getBuiltInBase();
    } else {
      tmp_type = type.originalValueWoPrefix;
    }
  } else {
    tmp_type = type.originalValueWoPrefix;
  }
  
  // If the element or attribute is an unnamed enumeration and has default
  // or fixed attribute
  if (parent->getEnumeration().modified &&
     (parent->getReference().get_ref() == NULL ||
      !((SimpleType*)(parent->getReference().get_ref()))->getEnumeration().modified)) {
    Mstring newtype = Mstring("");
    // If the unnamed enumeration is a field of a type
    if (parent->getMainType() != NULL) {
      newtype = parent->getMainType()->getName().convertedValue + ".";
      newtype += parent->getPath();
    } else {
      // If the unnamed enumeration is a top level attribute or element
      newtype = parent->getName().convertedValue;
    }
    type.upload(newtype);
  }
  
  // These types do not support enumeration restriction
  if (!(parent->getEnumeration().modified ||
     (parent->getReference().get_ref() != NULL &&
     ((SimpleType*)(parent->getReference().get_ref()))->getEnumeration().modified))
    && (tmp_type == "NMTOKENS" || tmp_type == "IDREFS" || tmp_type == "ENTITIES")) {
    // These are not supported by TITAN.
    // Reset the default value and fixed value
    parent->getValue().default_value = "";
    parent->getValue().fixed_value = "";
    unsupported = true;
    // Set the constant to invisible so it won't be generated into the code.
    setInvisible();
  } else if (parent->getEnumeration().modified ||
     (parent->getReference().get_ref() != NULL &&
     ((SimpleType*)(parent->getReference().get_ref()))->getEnumeration().modified)) {
    
    EnumerationType* enumeration =
      &(parent->getEnumeration().modified ? parent->getEnumeration() :
        ((SimpleType*)(parent->getReference().get_ref()))->getEnumeration());
    
    // float enumeration handled differently
    if (isFloatType(enumeration->parent->getBuiltInBase())) {
      value = xmlFloat2TTCN3FloatStr(value);
    } else {
      // We pushed the converted enumeration values to the converted_facets,
      // here we find the converted value based on the position in the list.
      int i = 0, j = 0;
      for(List<Mstring>::iterator it = enumeration->facets.begin(); it ; it = it->Next) {
        if (it->Data == value) break;
        i++;
      }
      for(List<Mstring>::iterator it = enumeration->converted_facets.begin(); it ; it = it->Next) {
        if (i == j) {
          value = it->Data;
          break;
        }
        j++;
      }
    }
  // String and time types need the quote character around the value
  } else if (isStringType(tmp_type) || isTimeType(tmp_type) || tmp_type == "anySimpleType") {
    value = Mstring("\"") + value + Mstring("\"");
  } else if (isFloatType(tmp_type)) {
    value = xmlFloat2TTCN3FloatStr(value);
  } else if (isBooleanType(tmp_type)) {
    if (value == "1") {
      value = "true";
    } else if (value == "0") {
      value = "false";
    }
  } else if (isQNameType(tmp_type) || isAnyType(tmp_type)) {
    // These are not supported by TITAN.
    // Reset the default value and fixed value
    parent->getValue().default_value = "";
    parent->getValue().fixed_value = "";
    unsupported = true;
    // Set the constant to invisible so it won't be generated into the code.
    setInvisible();
  }
  checked = true;
}

void Constant::finalFinalModification(List<RootType*> constantDefs) {
  // Make the pointers point to the constant which have the same type and value,
  // and set one of them invisible, so only one constant will be generated for
  // each unique type and value pair.
  for (List<RootType*>::iterator it = constantDefs.begin(); it; it = it->Next) {
    if (!it->Data->isVisible()) continue;
    for (List<RootType*>::iterator it2 = it->Next; it2; it2 = it2->Next) {
      if (!it2->Data->isVisible()) continue;
      if (*(Constant*)(it->Data) == *(Constant*)(it2->Data)) {
        ((Constant*)it2->Data)->alterego = (Constant*)(it->Data);
        it2->Data->setInvisible();
      }
    }
  }
  if (!constantDefs.empty()) {
    TTCN3Module* module = constantDefs.begin()->Data->getModule();
    expstring_t tmpname = NULL;
    for (List<RootType*>::iterator it = constantDefs.begin(); it; it = it->Next) {
      if (!it->Data->isVisible()) continue;
      tmpname = mputprintf(tmpname, "c_defaultForEmpty_%u", module->getConstCounter());
      module->increaseConstCounter();
      const_cast<NameType&>(it->Data->getName()).upload(Mstring(tmpname));
      Free(tmpname);
      tmpname = NULL;
    }
  }
}

Mstring Constant::getConstantName(const TTCN3Module* other_mod) const {
  Mstring qname;
  if (alterego != NULL) {
    qname = alterego->getName().convertedValue;
  } else {
    qname = getName().convertedValue;  
  }
  if (!o_flag_used && other_mod->getModulename() != getModule()->getModulename()) {
    qname = getModule()->getModulename() + "." + qname;
  }
  return qname;
}

void Constant::printToFile(FILE * file) {
  if (!visible) return;
  fprintf(file, "const %s %s := %s;\n\n\n",
    type.convertedValue.c_str(),
    name.convertedValue.c_str(),
    value.c_str());
}

void Constant::dump(const unsigned int depth) const {
  fprintf(stderr, "%*s Constant '%s' -> '%s' with value: '%s' at %p\n", depth * 2, "",
    name.originalValueWoPrefix.c_str(), name.convertedValue.c_str(), value.c_str(),
    (const void*) this);
}
