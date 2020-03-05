/******************************************************************************
 * Copyright (c) 2000-2020 Ericsson Telecom AB
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
#ifndef JSONAST_HH_
#define	JSONAST_HH_

#include "../datatypes.h"
#include "../vector.hh"
#include "RawAST.hh"

class JsonSchemaExtension {
private:
  void init(char* p_key, char* p_value);
public:
  char* key;
  char* value;
  
  JsonSchemaExtension(char* p_key, char* p_value) { init(p_key, p_value); }
  JsonSchemaExtension(const JsonSchemaExtension& x) { init(mcopystr(x.key), mcopystr(x.value)); }
  ~JsonSchemaExtension();
};

struct JsonEnumText {
  char* from;
  char* to;
  int index; // set during semantic analysis
  JsonEnumText(char* p_from, char* p_to): from(p_from), to(p_to) {}
  ~JsonEnumText();
};

class JsonAST {
public:
  enum json_type_indicator {
    JSON_NO_TYPE,
    JSON_NUMBER,
    JSON_INTEGER,
    JSON_STRING,
    JSON_ARRAY,
    JSON_OBJECT,
    JSON_OBJECT_MEMBER,
    JSON_LITERAL
  };
  enum json_string_escaping {
    ESCAPING_UNSET, // no escaping attribute was set (equivalent with ESCAPE_AS_SHORT at runtime)
    ESCAPE_AS_SHORT, // attribute "escape as short" was set explicitly
    ESCAPE_AS_USI,
    ESCAPE_AS_TRANSPARENT
  };
private:
  void init_JsonAST();
  JsonAST(const JsonAST&);
  JsonAST& operator=(const JsonAST&);
public:
  boolean omit_as_null;
  char* alias;
  boolean as_value;
  char* default_value;
  vector<JsonSchemaExtension> schema_extensions;
  boolean metainfo_unbound;
  boolean as_number;
  rawAST_tag_list* tag_list;
  boolean as_map;
  boolean use_null;
  vector<JsonEnumText> enum_texts;
  json_type_indicator type_indicator;
  json_string_escaping string_escaping;

  JsonAST() { init_JsonAST(); }
  JsonAST(const JsonAST *other_val);
  ~JsonAST();

  const char* get_type_str() const;
  const char* get_escaping_str() const;
  const char* get_escaping_gen_str() const;

  boolean empty() const;

  void print_JsonAST() const;
};

#endif	/* JSONAST_HH_ */

