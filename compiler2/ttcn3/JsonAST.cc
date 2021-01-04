/******************************************************************************
 * Copyright (c) 2000-2021 Ericsson Telecom AB
 * All rights reserved. This program and the accompanying materials
 * are made available under the terms of the Eclipse Public License v2.0
 * which accompanies this distribution, and is available at
 * https://www.eclipse.org/org/documents/epl-2.0/EPL-2.0.html
 *
 * Contributors:
 *   Balasko, Jeno
 *   Baranyi, Botond
 *
 ******************************************************************************/
#include "JsonAST.hh"
#include "../../common/memory.h"
#include <cstddef>
#include <cstdio>

void JsonSchemaExtension::init(char* p_key, char* p_value)
{
  key = p_key;
  value = p_value;
}

JsonSchemaExtension::~JsonSchemaExtension()
{
  Free(key);
  Free(value);
}

JsonEnumText::~JsonEnumText()
{
  Free(from);
  Free(to);
}

void JsonAST::init_JsonAST()
{
  omit_as_null = false;
  alias = NULL;
  as_value = false;
  default_value = NULL;
  metainfo_unbound = false;
  as_number = false;
  tag_list = NULL;
  as_map = false;
  use_null = false;
  type_indicator = JSON_NO_TYPE;
  string_escaping = ESCAPING_UNSET;
}

JsonAST::JsonAST(const JsonAST *other_val)
{
  init_JsonAST();
  if (NULL != other_val) {
    omit_as_null = other_val->omit_as_null;
    alias = (NULL != other_val->alias) ? mcopystr(other_val->alias) : NULL;
    as_value = other_val->as_value;
    default_value = (NULL != other_val->default_value) ? mcopystr(other_val->default_value) : NULL;
    as_number = other_val->as_number;
    for (size_t i = 0; i < other_val->schema_extensions.size(); ++i) {
      schema_extensions.add(new JsonSchemaExtension(*other_val->schema_extensions[i]));
    }
    metainfo_unbound = other_val->metainfo_unbound;
    as_map = other_val->as_map;
    for (size_t i = 0; i < other_val->enum_texts.size(); ++i) {
      enum_texts.add(new JsonEnumText(mcopystr(other_val->enum_texts[i]->from),
        mcopystr(other_val->enum_texts[i]->to)));
    }
    use_null = other_val->use_null;
    type_indicator = other_val->type_indicator;
    string_escaping = other_val->string_escaping;
  }
}

JsonAST::~JsonAST()
{
  Free(alias);
  Free(default_value);
  for (size_t i = 0; i < schema_extensions.size(); ++i) {
    delete schema_extensions[i];
  }
  schema_extensions.clear();
  if (tag_list != NULL) {
    free_rawAST_tag_list(tag_list);
    delete tag_list;
  }
  for (size_t i = 0; i < enum_texts.size(); ++i) {
    delete enum_texts[i];
  }
  enum_texts.clear();
}

boolean JsonAST::empty() const
{
  return omit_as_null == false && alias == NULL && as_value == false &&
    default_value == NULL && metainfo_unbound == false && as_number == false &&
    tag_list == NULL && as_map == false && enum_texts.size() == 0 &&
    use_null == false && type_indicator != JSON_OBJECT &&
    type_indicator != JSON_LITERAL &&
    (string_escaping == ESCAPING_UNSET || string_escaping == ESCAPE_AS_SHORT);
}

const char* JsonAST::get_type_str() const
{
  switch (type_indicator) {
  case JSON_NO_TYPE:
    return "<none>";
  case JSON_NUMBER:
    return "JSON:number";
  case JSON_INTEGER:
    return "JSON:integer";
  case JSON_STRING:
    return "JSON:string";
  case JSON_ARRAY:
    return "JSON:array";
  case JSON_OBJECT:
    return "JSON:object";
  case JSON_OBJECT_MEMBER:
    return "JSON:objectMember";
  case JSON_LITERAL:
    return "JSON:literal";
  default:
    FATAL_ERROR("JsonAST::get_type_str");
  }
}

const char* JsonAST::get_escaping_str() const
{
  switch (string_escaping) {
  case ESCAPE_AS_SHORT:
    return "escape as short";
  case ESCAPE_AS_USI:
    return "escape as usi";
  case ESCAPE_AS_TRANSPARENT:
    return "escape as transparent";
  default:
    FATAL_ERROR("JsonAST::get_escaping_str");
  }
}

const char* JsonAST::get_escaping_gen_str() const
{
  switch (string_escaping) {
  case ESCAPING_UNSET:
  case ESCAPE_AS_SHORT:
    return "ESCAPE_AS_SHORT";
  case ESCAPE_AS_USI:
    return "ESCAPE_AS_USI";
  case ESCAPE_AS_TRANSPARENT:
    return "ESCAPE_AS_TRANSPARENT";
  default:
    FATAL_ERROR("JsonAST::get_escaping_gen_str");
  }
}

void JsonAST::print_JsonAST() const
{
  printf("\n\rOmit encoding: ");
  if (omit_as_null) {
    printf("as null value\n\r");
  } else {
    printf("skip field\n\r");
  }
  if (alias) {
    printf("Name as %s\n\r", alias);
  }
  if (as_value) {
    printf("Encoding unions as JSON value\n\r");
  }
  if (default_value) {
    printf("Default value: %s\n\r", default_value);
  }
  if (as_number) {
    printf("Encoding enumerated values as numbers\n\r");
  }
  if (0 != schema_extensions.size()) {
    printf("Extensions:");
    for (size_t i = 0; i < schema_extensions.size(); ++i) {
      printf(" \"%s\" : \"%s\"", schema_extensions[i]->key, schema_extensions[i]->value);
    }
  }
  if (metainfo_unbound) {
    printf("Metainfo for unbound field(s)\n\r");
  }
  if (tag_list != NULL) {
    printf("Chosen union fields:\n\r");
    printf("  Number of rules: %d\n\r", tag_list->nElements);
    for (int i = 0; i < tag_list->nElements; ++i) {
      printf("  Rule #%d:\n\r", i);
      printf("    Chosen field: %s\n\r", tag_list->tag[i].fieldName != NULL ?
        tag_list->tag[i].fieldName->get_name().c_str() : "omit");
      printf("    Number of conditions: %d\n\r", tag_list->tag[i].nElements);
      for (int j = 0; j < tag_list->tag[i].nElements; ++j) {
        printf("    Condition #%d:\n\r", j);
        printf("      Value: %s\n\r", tag_list->tag[i].keyList[j].value);
        printf("      Field: ");
        for (int k = 0; k < tag_list->tag[i].keyList[j].keyField->nElements; ++k) {
          if (k != 0) {
            printf(".");
          }
          printf("%s", tag_list->tag[i].keyList[j].keyField->names[k]->get_name().c_str());
        }
        printf("\n\r");
      }
    }
    printf("Type: %s\n\r", get_type_str());
    if (string_escaping != ESCAPING_UNSET) {
      printf("%s\n\r", get_escaping_str());
    }
  }
  if (as_map) {
    printf("Encoding elements into a map of key-value pairs.\n\r");
  }
  if (0 != enum_texts.size()) {
    printf("Enum texts:");
    for (size_t i = 0; i < enum_texts.size(); ++i) {
      printf(" '%s' -> '%s'", enum_texts[i]->from, enum_texts[i]->to);
    }
    printf("\n\r");
  }
}
