/******************************************************************************
 * Copyright (c) 2000-2019 Ericsson Telecom AB
 * All rights reserved. This program and the accompanying materials
 * are made available under the terms of the Eclipse Public License v2.0
 * which accompanies this distribution, and is available at
 * https://www.eclipse.org/org/documents/epl-2.0/EPL-2.0.html
 *
 * Contributors:
 *   Balasko, Jeno
 *   Baranyi, Botond
 *   Szabo, Bence Janos
 *
 ******************************************************************************/
#ifndef JSON_HH_
#define	JSON_HH_

#include "Types.h"

class TTCN_Buffer;
class JSON_Tokenizer;
class CHARSTRING;
class INTEGER;

/** Enumerated text change structure */
struct JsonEnumText {
  int index;
  const char* text;
};

/** Helper enumerated type for storing the different methods of escaping in character strings */
enum json_string_escaping {
  ESCAPE_AS_SHORT, /* use short escapes wherever possible (e.g. '\n', '\\', etc.)*/
  ESCAPE_AS_USI, /* use USI escapes in all cases (i.e. '\u' followed by 4 hex nibbles) */
  ESCAPE_AS_TRANSPARENT /* do not escape anything, except control characters */
};

/** Descriptor for JSON encoding/decoding during runtime */
struct TTCN_JSONdescriptor_t 
{
  /** Encoding only. 
    * true  : use the null literal to encode omitted fields in records or sets
    *         example: { "field1" : value1, "field2" : null, "field3" : value3 } 
    * false : skip both the field name and the value if a field is omitted
    *         example: { "field1" : value1, "field3" : value3 }
    * The decoder will always accept both variants. */
  boolean omit_as_null;
  
  /** An alias for the name of the field (in a record, set or union). 
    * Encoding: this alias will appear instead of the name of the field
    * Decoding: the decoder will look for this alias instead of the field's real name */
  const char* alias;
  
  /** If set, the union will be encoded as a JSON value instead of a JSON object
    * with one name-value pair. 
    * Since the field name is no longer present, the decoder will determine the
    * selected field based on the type of the value. The first field (in the order
    * of declaration) that can successfully decode the value will be the selected one. */
  boolean as_value;
  
  /** Decoding only.
    * Fields that don't appear in the JSON code will decode this value instead. */
  const char* default_value;
  
  /** If set, encodes unbound fields of records and sets as null and inserts a
    * meta info field into the JSON object specifying that the field is unbound.
    * The decoder sets the field to unbound if the meta info field is present and
    * the field's value in the JSON code is either null or a valid value for that
    * field.
    * Example: { "field1" : null, "metainfo field1" : "unbound" }
    *
    * Also usable on record of/set of/array types to indicate that an element is
    * unbound. Unbound elements are encoded as a JSON object containing one
    * metainfo member. The decoder sets the element to unbound if the object
    * with the meta information is found.
    * Example: [ value1, value2, { "metainfo []" : "unbound" }, value3 ] */
  boolean metainfo_unbound;
  
  /** If set, the enumerated value's numeric form will be encoded as a JSON
    * number, instead of its name form as a JSON string (affects both encoding
    * and decoding). */
  boolean as_number;
  
  /** If set, encodes the value into a map of key-value pairs (i.e. a fully 
    * customizable JSON object). The encoded type has to be a record of/set of
    * with a record/set element type, which has 2 fields, the first of which is
    * a non-optional universal charstring.
    * Example: { "key1" : value1, "key2" : value2 } */
  boolean as_map;
  
  /** Number of enumerated values whose texts are changed. */
  size_t nof_enum_texts;
  
  /** List of enumerated values whose texts are changed. */
  const JsonEnumText* enum_texts;
  
  /** If set, encodes this value as the JSON literal 'null'. */
  boolean use_null;
  
  /** Setting for escaping of special characters in character strings */
  json_string_escaping escaping;
};

/** This macro makes sure that coding errors will only be displayed if the silent
  * flag is not set. */
#define JSON_ERROR if(!p_silent) TTCN_EncDec_ErrorContext::error

// JSON descriptors for base types
extern const TTCN_JSONdescriptor_t INTEGER_json_;
extern const TTCN_JSONdescriptor_t FLOAT_json_;
extern const TTCN_JSONdescriptor_t BOOLEAN_json_;
extern const TTCN_JSONdescriptor_t BITSTRING_json_;
extern const TTCN_JSONdescriptor_t HEXSTRING_json_;
extern const TTCN_JSONdescriptor_t OCTETSTRING_json_;
extern const TTCN_JSONdescriptor_t CHARSTRING_json_;
extern const TTCN_JSONdescriptor_t UNIVERSAL_CHARSTRING_json_;
extern const TTCN_JSONdescriptor_t VERDICTTYPE_json_;
extern const TTCN_JSONdescriptor_t GeneralString_json_;
extern const TTCN_JSONdescriptor_t NumericString_json_;
extern const TTCN_JSONdescriptor_t UTF8String_json_;
extern const TTCN_JSONdescriptor_t PrintableString_json_;
extern const TTCN_JSONdescriptor_t UniversalString_json_;
extern const TTCN_JSONdescriptor_t BMPString_json_;
extern const TTCN_JSONdescriptor_t GraphicString_json_;
extern const TTCN_JSONdescriptor_t IA5String_json_;
extern const TTCN_JSONdescriptor_t TeletexString_json_;
extern const TTCN_JSONdescriptor_t VideotexString_json_;
extern const TTCN_JSONdescriptor_t VisibleString_json_;
extern const TTCN_JSONdescriptor_t ASN_NULL_json_;
extern const TTCN_JSONdescriptor_t OBJID_json_;
extern const TTCN_JSONdescriptor_t ASN_ROID_json_;
extern const TTCN_JSONdescriptor_t ASN_ANY_json_;
extern const TTCN_JSONdescriptor_t ENUMERATED_json_;

/** JSON decoder error codes */
enum json_decode_error {
  /** An unexpected JSON token was extracted. The token might still be valid and
    * useful for the caller structured type. */
  JSON_ERROR_INVALID_TOKEN = -1,
  /** The JSON tokeniser couldn't extract a valid token (JSON_TOKEN_ERROR) or the
    * format of the data extracted is invalid. In either case, this is a fatal 
    * error and the decoding cannot continue. 
    * @note This error code is always preceeded by a decoding error, if the
    * caller receives this code, it means that decoding error behavior is (at least 
    * partially) set to warnings. */
  JSON_ERROR_FATAL = -2
};

/** JSON meta info states during decoding */
enum json_metainfo_t {
  /** The field does not have meta info enabled */
  JSON_METAINFO_NOT_APPLICABLE,
  /** Initial state if meta info is enabled for the field */
  JSON_METAINFO_NONE,
  /** The field's value is set to null, but no meta info was received for the field yet */
  JSON_METAINFO_NEEDED,
  /** Meta info received: the field is unbound */
  JSON_METAINFO_UNBOUND
};

enum json_chosen_field_t {
  CHOSEN_FIELD_UNSET = -1,
  CHOSEN_FIELD_OMITTED = -2
};

// JSON decoding error messages
#define JSON_DEC_BAD_TOKEN_ERROR "Failed to extract valid token, invalid JSON format%s"
#define JSON_DEC_FORMAT_ERROR "Invalid JSON %s format, expecting %s value"
#define JSON_DEC_NAME_TOKEN_ERROR "Invalid JSON token, expecting JSON field name"
#define JSON_DEC_OBJECT_END_TOKEN_ERROR "Invalid JSON token, expecting JSON name-value pair or object end mark%s"
#define JSON_DEC_REC_OF_END_TOKEN_ERROR "Invalid JSON token, expecting JSON value or array end mark%s"
#define JSON_DEC_ARRAY_ELEM_TOKEN_ERROR "Invalid JSON token, expecting %d more JSON value%s"
#define JSON_DEC_ARRAY_END_TOKEN_ERROR "Invalid JSON token, expecting JSON array end mark%s"
#define JSON_DEC_FIELD_TOKEN_ERROR "Invalid JSON token found while decoding field '%.*s'"
#define JSON_DEC_INVALID_NAME_ERROR "Invalid field name '%.*s'"
#define JSON_DEC_MISSING_FIELD_ERROR "No JSON data found for field '%s'"
#define JSON_DEC_STATIC_OBJECT_END_TOKEN_ERROR "Invalid JSON token, expecting JSON object end mark%s"
#define JSON_DEC_AS_VALUE_ERROR "Extracted JSON %s could not be decoded by any field of the union"
#define JSON_DEC_METAINFO_NAME_ERROR "Meta info provided for non-existent field '%.*s'"
#define JSON_DEC_METAINFO_VALUE_ERROR "Invalid meta info for field '%s'"
#define JSON_DEC_METAINFO_NOT_APPLICABLE "Meta info not applicable to field '%s'"
#define JSON_DEC_CHOSEN_FIELD_NOT_NULL "Invalid JSON token, expecting 'null' (as indicated by a condition in attribute 'chosen')%s"
#define JSON_DEC_CHOSEN_FIELD_OMITTED "Field '%s' cannot be omitted (as indicated by a condition in attribute 'chosen')"
#define JSON_DEC_CHOSEN_FIELD_OMITTED_NULL "Field cannot be omitted (as indicated by a condition in attribute 'chosen')%s"


// Functions for conversion between json and cbor and vice versa
void json2cbor_coding(TTCN_Buffer& buff, JSON_Tokenizer& tok, size_t& num_of_items);
void cbor2json_coding(TTCN_Buffer& buff, JSON_Tokenizer& tok, bool in_object);

// Functions for conversion between json and bson and vice versa
void json2bson_coding(TTCN_Buffer& buff, JSON_Tokenizer& tok, bool in_object, bool in_array, INTEGER& length, CHARSTRING& f_name, bool& is_special);
void bson2json_coding(TTCN_Buffer& buff, JSON_Tokenizer& tok, bool in_object, bool in_array);

#endif	/* JSON_HH_ */

