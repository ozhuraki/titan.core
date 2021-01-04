/******************************************************************************
 * Copyright (c) 2000-2021 Ericsson Telecom AB
 * All rights reserved. This program and the accompanying materials
 * are made available under the terms of the Eclipse Public License v2.0
 * which accompanies this distribution, and is available at
 * https://www.eclipse.org/org/documents/epl-2.0/EPL-2.0.html
 *
 * Contributors:
 *   Balasko, Jeno
 *   Raduly, Csaba
 *   Szabados, Kristof
 *   Szabo, Bence Janos
 *
 ******************************************************************************/
#include "EnumItem.hh"
#include "Value.hh"
#include "Int.hh"
#include <stdlib.h>

namespace Common {

// =================================
// ===== EnumItem
// =================================

EnumItem::EnumItem(Identifier *p_name, Value *p_value)
: Node(), Location(), name(p_name), value(p_value), int_value(NULL)
{
  if (!p_name) FATAL_ERROR("NULL parameter: Common::EnumItem::EnumItem()");
}

EnumItem::EnumItem(const EnumItem& p)
: Node(p), Location(p)
{
  name=p.name->clone();
  value=p.value?p.value->clone():0;
  if (p.int_value) {
    int_value = new int_val_t(*p.int_value);
  } else {
    int_value = 0;
  }
}

EnumItem::~EnumItem()
{
  delete name;
  delete value;
  delete int_value;
}

EnumItem *EnumItem::clone() const
{
  return new EnumItem(*this);
}

void EnumItem::set_fullname(const string& p_fullname)
{
  Node::set_fullname(p_fullname);
  if(value) value->set_fullname(p_fullname);
}

void EnumItem::set_my_scope(Scope *p_scope)
{
  if(value) value->set_my_scope(p_scope);
}

void EnumItem::set_value(Value *p_value)
{
  if(!p_value) FATAL_ERROR("NULL parameter: Common::EnumItem::set_value()");
  if(value) FATAL_ERROR("Common::EnumItem::set_value()");
  value=p_value;
  delete int_value;
  int_value = NULL;
  calculate_int_value();
}

void EnumItem::set_text(const string& p_text)
{
  text = p_text;
  
  // De escape special XML characters if any found, into descaped_text
  descaped_text = p_text;
  size_t pos = 0;
  while ((pos = descaped_text.find("&apos;", 0)) != descaped_text.size()) {
    descaped_text.replace(pos, 6, "'");
  }
  while ((pos = descaped_text.find("&quot;", 0)) != descaped_text.size()) {
    descaped_text.replace(pos, 6, "\\\"");
  }
  while ((pos = descaped_text.find("&lt;", 0)) != descaped_text.size()) {
    descaped_text.replace(pos, 4, "<");
  }
  while ((pos = descaped_text.find("&gt;", 0)) != descaped_text.size()) {
    descaped_text.replace(pos, 4, ">");
  }
  while ((pos = descaped_text.find("&amp;", 0)) != descaped_text.size()) {
    descaped_text.replace(pos, 5, "&");
  }
  while ((pos = descaped_text.find("&#60;", 0)) != descaped_text.size()) {
    descaped_text.replace(pos, 5, "<");
  }
  while ((pos = descaped_text.find("&#62;", 0)) != descaped_text.size()) {
    descaped_text.replace(pos, 5, ">");
  }
  while ((pos = descaped_text.find("&#34;", 0)) != descaped_text.size()) {
    descaped_text.replace(pos, 5, "\\\"");
  }
  while ((pos = descaped_text.find("&#38;", 0)) != descaped_text.size()) {
    descaped_text.replace(pos, 5, "&");
    pos++;
  }
  while ((pos = descaped_text.find("&#39;", 0)) != descaped_text.size()) {
    descaped_text.replace(pos, 5, "'");
  }
}

bool EnumItem::calculate_int_value() {
  if (int_value) {
    return true;
  }
  Value *v = NULL;
  value->set_lowerid_to_ref();
  if (value->get_valuetype() == Value::V_REFD || value->get_valuetype() == Value::V_EXPR) {
    v = value->get_value_refd_last();
  } else {
    v = value;
  }
  
  if (v->is_unfoldable()) {
    value->error("A value known at compile time was expected for enumeration `%s'",
      name->get_dispname().c_str());
    return false;
  }
  
  switch (v->get_valuetype()) {
    case Value::V_INT:
      int_value = new int_val_t(*(v->get_val_Int()));
      break;
    case Value::V_BSTR: {
      long int res = strtol(v->get_val_str().c_str(), NULL, 2);
      int_value = new int_val_t(res);
      break;
    }
    case Value::V_OSTR:
    case Value::V_HSTR: {
      long int res = strtol(v->get_val_str().c_str(), NULL, 16);
      int_value = new int_val_t(res);
      break;
    }
    default:
      value->error("INTEGER or BITSTRING or OCTETSTRING or HEXSTRING value was expected for enumeration `%s'",
        name->get_dispname().c_str());
      return false;
  }
  return true;
}

int_val_t * EnumItem::get_int_val() const {
  if (value == NULL) {
    FATAL_ERROR("NULL value in Common::EnumItem::calculate_int_value()");
  }
  if (int_value == NULL) {
    FATAL_ERROR("Not calculated value in Common::EnumItem::calculate_int_value()");
  }
  return int_value;
}

void EnumItem::dump(unsigned level) const
{
  name->dump(level);
  if(value) {
    DEBUG(level, "with value:");
    value->dump(level+1);
  }
}

// =================================
// ===== EnumItems
// =================================

EnumItems::EnumItems(const EnumItems& p)
: Node(p), my_scope(0)
{
  for (size_t i = 0; i < p.eis_v.size(); i++) add_ei(p.eis_v[i]->clone());
}

EnumItems::~EnumItems()
{
  for(size_t i = 0; i < eis_v.size(); i++) delete eis_v[i];
  eis_v.clear();
  eis_m.clear();
}

void EnumItems::release_eis()
{
  eis_v.clear();
  eis_m.clear();
}

EnumItems* EnumItems::clone() const
{
  return new EnumItems(*this);
}

void EnumItems::set_fullname(const string& p_fullname)
{
  Node::set_fullname(p_fullname);
  for (size_t i = 0; i < eis_v.size(); i++) {
    EnumItem *ei = eis_v[i];
    ei->set_fullname(p_fullname + "." + ei->get_name().get_dispname());
  }
}

void EnumItems::set_my_scope(Scope *p_scope)
{
  my_scope = p_scope;
  for(size_t i = 0; i < eis_v.size(); i++)
    eis_v[i]->set_my_scope(p_scope);
}

void EnumItems::add_ei(EnumItem *p_ei)
{
  if(!p_ei)
    FATAL_ERROR("NULL parameter: Common::EnumItems::add_ei()");
  eis_v.add(p_ei);
  const Identifier& id = p_ei->get_name();
  const string& name = id.get_name();
  if (!eis_m.has_key(name)) eis_m.add(name, p_ei);
  p_ei->set_fullname(get_fullname()+"."+id.get_dispname());
  p_ei->set_my_scope(my_scope);
}

void EnumItems::dump(unsigned level) const
{
  for (size_t i = 0; i < eis_v.size(); i++) eis_v[i]->dump(level);
}

}
