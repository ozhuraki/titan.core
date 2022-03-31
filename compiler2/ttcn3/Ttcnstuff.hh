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
 *   Delic, Adam
 *   Knapp, Adam
 *   Raduly, Csaba
 *   Szabados, Kristof
 *   Szabo, Bence Janos
 *   Zalanyi, Balazs Andor
 *
 ******************************************************************************/
#ifndef TTCNSTUFF_H_
#define TTCNSTUFF_H_

#include "../../common/version.h"
#include "../Setting.hh"
#include "../Type.hh" // for Common::Common::Type::MessageEncodingType_t
#include "AST_ttcn3.hh" // For Def_Function_Base::prototype_t
#include "port.h"
#include "../vector.hh"

namespace Ttcn {

class Reference;

/**
 * Class to represent an error behavior setting in the codec API of the
 * run-time environment. The setting contains a pair of strings:
 * the error type identifier and the way of error handling.
 */
class ErrorBehaviorSetting : public Common::Node, public Common::Location {
private:
  string error_type, error_handling;
public:
  ErrorBehaviorSetting(const string& p_error_type,
    const string& p_error_handling)
  : Common::Node(), Common::Location(), error_type(p_error_type),
    error_handling(p_error_handling) { }
  virtual ErrorBehaviorSetting *clone() const;
  const string& get_error_type() const { return error_type; }
  void set_error_type(const string& p_error_type)
    { error_type = p_error_type; }
  const string& get_error_handling() const { return error_handling; }
  void set_error_handling(const string& p_error_handling)
    { error_handling = p_error_handling; }
  virtual void dump(unsigned level) const;
};

/**
 * Class to represent a list of error behavior settings in the codec API
 * of the run-time environment.
 */
class ErrorBehaviorList : public Common::Node, public Common::Location {
private:
  vector<ErrorBehaviorSetting> ebs_v;
  map<string, ErrorBehaviorSetting> ebs_m;
  ErrorBehaviorSetting *ebs_all;
  bool checked;
  /** Copy constructor not implemented */
  ErrorBehaviorList(const ErrorBehaviorList& p);
  /** Assignment disabled */
  ErrorBehaviorList& operator=(const ErrorBehaviorList& p);
public:
  ErrorBehaviorList() : Common::Node(), Common::Location(), ebs_all(0), checked(false) { }
  virtual ~ErrorBehaviorList();
  virtual ErrorBehaviorList *clone() const;
  virtual void set_fullname(const string& p_fullname);
  void add_ebs(ErrorBehaviorSetting *p_ebs);
  void steal_ebs(ErrorBehaviorList *p_eblist);
  size_t get_nof_ebs() const { return ebs_v.size(); }
  ErrorBehaviorSetting *get_ebs_byIndex(size_t n) const { return ebs_v[n]; }
  bool has_setting(const string& p_error_type);
  string get_handling(const string& p_error_type);
  void chk();
  char *generate_code(char *str);
  virtual void dump(unsigned level) const;
};

/** Printing type class for JSON encoder functions */
class PrintingType : public Common::Node, public Common::Location {
public:
  /** Printing types */
  enum printing_t {
    /** No printing has been set */
    PT_NONE,
    /** Printing without any white spaces to make the JSON code as short as possible */
    PT_COMPACT,
    /** Printing with extra white spaces to make the JSON code easier to read */
    PT_PRETTY
  };
  
  PrintingType() : Common::Node(), Common::Location(), printing(PT_NONE) { }
  virtual ~PrintingType() {};
  virtual PrintingType *clone() const;
  void set_printing(printing_t p_pt) { printing = p_pt; }
  printing_t get_printing() const { return printing; }
  char *generate_code(char *str);
  
private:
  printing_t printing;
  
  /** Copy constructor not implemented */
  PrintingType(const PrintingType& p);
  /** Assignment disabled */
  PrintingType& operator=(const PrintingType& p);
};

/** A single target type for type mapping */
class TypeMappingTarget : public Common::Node, public Common::Location {
public:
  enum TypeMappingType_t {
    TM_SIMPLE, /**< simple mapping (source == target) */
    TM_DISCARD, /**< the message is discarded (target type is NULL) */
    TM_FUNCTION, /**< mapping using a [external] function */
    TM_ENCODE, /**< mapping using a built-in encoder */
    TM_DECODE /**< mapping using a built-in decoder */
  };
private:
  Common::Type *target_type; // owned
  TypeMappingType_t mapping_type;
  union {
    struct {
      Ttcn::Reference *function_ref;
      Ttcn::Def_Function_Base *function_ptr;
    } func;
    struct {
      Common::Type::MessageEncodingType_t coding_type; // BER,RAW,...
      string *coding_options;
      ErrorBehaviorList *eb_list;
    } encdec;
  } u;
  bool checked;
  /** Copy constructor not implemented */
  TypeMappingTarget(const TypeMappingTarget& p);
  /** Assignment disabled */
  TypeMappingTarget& operator=(const TypeMappingTarget& p);
public:
  /** Constructor for TM_SIMPLE and TM_DISCARD. */
  TypeMappingTarget(Common::Type *p_target_type, TypeMappingType_t p_mapping_type);
  /** Constructor for TM_FUNCTION. */
  TypeMappingTarget(Common::Type *p_target_type, TypeMappingType_t p_mapping_type,
    Ttcn::Reference *p_function_ref);
  /** Constructor for TM_ENCODE and TM_DECODE. */
  TypeMappingTarget(Common::Type *p_target_type, TypeMappingType_t p_mapping_type,
    Common::Type::MessageEncodingType_t p_coding_type, string *p_coding_options,
    ErrorBehaviorList *p_eb_list);
  virtual ~TypeMappingTarget();
  virtual TypeMappingTarget *clone() const;
  virtual void set_fullname(const string& p_fullname);
  virtual void set_my_scope(Common::Scope *p_scope);
  TypeMappingType_t get_mapping_type() const { return mapping_type; }
  /** Returns the string representation of \a mapping_type */
  const char *get_mapping_name() const;
  Common::Type *get_target_type() const { return target_type; }
  /** Applicable if \a mapping_type is TM_FUNCTION. */
  Ttcn::Def_Function_Base *get_function() const;
  /** Applicable if \a mapping_type is TM_ENCODE or TM_DECODE. */
  Common::Type::MessageEncodingType_t get_coding_type() const;
  /** Applicable if \a mapping_type is TM_ENCODE or TM_DECODE. */
  bool has_coding_options() const;
  /** Applicable if \a mapping_type is TM_ENCODE or TM_DECODE and
   * \a has_coding_options() returned true. */
  const string& get_coding_options() const;
  /** Applicable if \a mapping_type is TM_ENCODE or TM_DECODE. */
  ErrorBehaviorList *get_eb_list() const;
private:
  /** Check that the source_type is identical with the target_type of
   * this simple mapping. */
  void chk_simple(Common::Type *source_type);
  /** Check function mapping.
   * The function must have "prototype()", the function's input parameter
   * must match the source_type and its return type must match the target_type
   * of this mapping.
   * Param legacy: true if the old user attribute was used or false when the 
   *               syntax in the standard was used.
     Param incoming: true if the mapping is in the in list of port, false if the
   *                 mapping is in the out list of the port.*/
  void chk_function(Common::Type *source_type, Common::Type *port_type, bool legacy, bool incoming);
  /** Check "encode()" mapping. */
  void chk_encode(Common::Type *source_type);
  /** Check "decode()" mapping. */
  void chk_decode(Common::Type *source_type);
public:
  /* Param legacy: true if the old user attribute was used or false when the 
   *               syntax in the standard was used.
     Param incoming: true if the mapping is in the in list of port, false if the
   *                 mapping is in the out list of the port.*/
  void chk(Common::Type *source_type, Common::Type *port_type, bool legacy, bool incoming);
  /** Fills the appropriate data structure \a target for code generation. */
  bool fill_type_mapping_target(port_msg_type_mapping_target *target,
    Common::Type *source_type, Common::Scope *p_scope, stringpool& pool);
  virtual void dump(unsigned level) const;
};

/** A list of mapping targets */
class TypeMappingTargets : public Common::Node {
private:
  vector<TypeMappingTarget> targets_v;
  /** Copy constructor not implemented */
  TypeMappingTargets(const TypeMappingTargets& p);
  /** Assignment disabled */
  TypeMappingTargets& operator=(const TypeMappingTargets& p);
public:
  TypeMappingTargets() : Common::Node() { }
  virtual ~TypeMappingTargets();
  virtual TypeMappingTargets *clone() const;
  virtual void set_fullname(const string& p_fullname);
  virtual void set_my_scope(Common::Scope *p_scope);
  void add_target(TypeMappingTarget *p_target);
  size_t get_nof_targets() const { return targets_v.size(); }
  TypeMappingTarget *get_target_byIndex(size_t n) const
    { return targets_v[n]; }
  virtual void dump(unsigned level) const;
};

/** A mapping.
 *  One source type, multiple targets:
 *  @code
 *  in(octetstring -> PDU1 : function(f),
 *                    PDU2 : decode(RAW),
 *                    -    : discard
 *    )
 *  @endcode
 */
class TypeMapping : public Common::Node, public Common::Location {
private:
  Common::Type *source_type;
  TypeMappingTargets *targets;
  /** Copy constructor not implemented */
  TypeMapping(const TypeMapping& p);
  /** Assignment disabled */
  TypeMapping& operator=(const TypeMapping& p);
public:
  TypeMapping(Common::Type *p_source_type, TypeMappingTargets *p_targets);
  virtual ~TypeMapping();
  virtual TypeMapping *clone() const;
  virtual void set_fullname(const string& p_fullname);
  virtual void set_my_scope(Common::Scope *p_scope);
  Common::Type *get_source_type() const { return source_type; }
  size_t get_nof_targets() const { return targets->get_nof_targets(); }
  TypeMappingTarget *get_target_byIndex(size_t n) const
    { return targets->get_target_byIndex(n); }
  /* Param legacy: true if the old user attribute was used or false when the 
   *               syntax in the standard was used.
     Param incoming: true if the mapping is in the in list of port, false if the
   *                 mapping is in the out list of the port.*/
  void chk(Common::Type *port_type, bool legacy, bool incoming);
  virtual void dump(unsigned level) const;
};

class TypeMappings : public Common::Node, public Common::Location {
private:
  vector<TypeMapping> mappings_v;
  map<string, TypeMapping> mappings_m;
  bool checked;
  /** Copy constructor not implemented */
  TypeMappings(const TypeMappings& p);
  /** Assignment disabled */
  TypeMappings& operator=(const TypeMappings& p);
public:
  TypeMappings() : Common::Node(), Common::Location(), checked(false) { }
  virtual ~TypeMappings();
  virtual TypeMappings *clone() const;
  virtual void set_fullname(const string& p_fullname);
  virtual void set_my_scope(Common::Scope *p_scope);
  void add_mapping(TypeMapping *p_mapping);
  void steal_mappings(TypeMappings *p_mappings);
  size_t get_nof_mappings() const { return mappings_v.size(); }
  TypeMapping *get_mapping_byIndex(size_t n) const { return mappings_v[n]; }
  bool has_mapping_for_type(Common::Type *p_type) const;
  TypeMapping *get_mapping_byType(Common::Type *p_type) const;
  /* Param legacy: true if the old user attribute was used or false when the 
   *               syntax in the standard was used.
     Param incoming: true if the mapping is in the in list of port, false if the
   *                 mapping is in the out list of the port.*/
  void chk(Common::Type *port_type, bool legacy, bool incoming);
  virtual void dump(unsigned level) const;
};

/**
 * Type list, used in port and class types
 */
class Types : public Common::Node, public Common::Location {
private:
  vector<Common::Type> types;
  /** Copy constructor not implemented */
  Types(const Types& p);
  /** Assignment disabled */
  Types& operator=(const Types& p);
public:
  Types() : Common::Node(), Common::Location() { }
  virtual ~Types();
  virtual Types *clone() const;

  /** Add a new type to the list.
   *  It is now owned by the list. */
  void add_type(Common::Type *p_type);
  /** Moves (appends) all types of \a p_tl into this. */
  void steal_types(Types *p_tl);
  size_t get_nof_types() const { return types.size(); }
  /** Return a type based on its index. */
  Common::Type *get_type_byIndex(size_t n) const { return types[n]; }
  /** Remove a type from the list and return it.
   *  The old value is replaced with a NULL pointer. */
  Common::Type *extract_type_byIndex(size_t n);

  virtual void set_fullname(const string& p_fullname);
  virtual void set_my_scope(Common::Scope *p_scope);
  virtual void dump(unsigned level) const;
};

/**
 * Unique set of types. It does not own its members. The types are identified
 * using the string returned by get_typename().
 */
class TypeSet : public Common::Node {
  /** Copy constructor not implemented */
  TypeSet(const TypeSet& p);
  /** Assignment not implemented */
  TypeSet& operator=(const TypeSet& p);
private:
  vector<Common::Type> types_v;
  map<string, Common::Type> types_m;
public:
  TypeSet() : Common::Node() { }
  virtual ~TypeSet();
  virtual TypeSet *clone() const;

  /** Adds type \a p_type to the set. */
  void add_type(Common::Type *p_type);
  /** Returns the number of types in the set. */
  size_t get_nof_types() const { return types_v.size(); }
  /** Returns the \a n-th element of the set. */
  Common::Type *get_type_byIndex(size_t n) const { return types_v[n]; }
  /** Returns whether type \a p_type is a member of the set. */
  bool has_type(Common::Type *p_type) const;
  /** Returns the number of types in the set that are compatible with
   * \a p_type. */
  size_t get_nof_compatible_types(Common::Type *p_type) const;
  /** Returns the index of type \a p_type in the set. It is assumed that
   * \a p_type is a member of the set, otherwise FATAL_ERROR occurs. */
  size_t get_index_byType(Common::Type *p_type) const;
  /** Returns whether the set contains a type with typename \a p_name. */
  bool has_type_withName(const string& p_name) const
    { return types_m.has_key(p_name); }
  /** Returns the type from the set with typename \a p_name. */
  Common::Type *get_type_byName(const string& p_name) const
    { return types_m[p_name]; }

  virtual void dump(unsigned level) const;
};

/** Class to hold a parsed PortDefAttribs.
 *  This includes the operation mode and the input/output types. */
class PortTypeBody : public Common::Node, public Common::Location {
public:
  /**
   * Enumeration to represent TTCN-3 port operation modes.
   */
  enum PortOperationMode_t {
    PO_MESSAGE,
    PO_PROCEDURE,
    PO_MIXED
  };
  /**
   * Enumeration to represent the test port API variants.
   */
  enum TestPortAPI_t {
    TP_REGULAR, /**< regular test port API */
    TP_INTERNAL, /**< no test port (only connections are allowed) */
    TP_ADDRESS /**< usage of the address type is supported */
  };
  /**
   * Enumeration to represent the the properties of the port type.
   */
  enum PortType_t {
    PT_REGULAR, /**< regular port type */
    PT_PROVIDER, /**< the port type provides the external interface for other
                  * port types */
    PT_USER /**< the port type uses another port type as external interface */
  };
private:
  Common::Type *my_type; // the Type that owns this object
  PortOperationMode_t operation_mode; // message|procedure|mixed
  Types *in_list, *out_list, *inout_list;
  bool in_all, out_all, inout_all; // whether "(in|out|inout) all" was used
  bool checked;
  bool attributes_checked;
  bool legacy; // Old extension syntax or new standard syntax
  /* Types and signatures that can be sent and received.
   * These are initially empty; filled by PortTypeBody::chk_list based on
   * in_list, out_list, inout_list.
   * Signature types are added to _sigs, "normal" types to _msgs.
   */
  TypeSet *in_msgs, *out_msgs, *in_sigs, *out_sigs;
  TestPortAPI_t testport_type; // regular|internal|address
  PortType_t port_type; // regular|provider|user
  vector<Ttcn::Reference>provider_refs; ///< references to provider ports, for PT_USER
  vector<Common::Type> provider_types; ///< the types that provider_refs refers to, for PT_USER
  TypeMappings *in_mappings, *out_mappings; ///< mappings for PT_USER
  Definitions *vardefs; ///< variable definitions inside the port
  bool realtime;
  FormalParList *map_params, *unmap_params;
  /** Copy constructor not implemented */
  PortTypeBody(const PortTypeBody& p);
  /** Assignment disabled */
  PortTypeBody& operator=(const PortTypeBody& p);
public:
  PortTypeBody(PortOperationMode_t p_operation_mode,
    Types *p_in_list, Types *p_out_list, Types *p_inout_list,
    bool p_in_all, bool p_out_all, bool p_inout_all, Definitions *defs,
    bool p_realtime, FormalParList *p_map_params, FormalParList *p_unmap_params);
  ~PortTypeBody();
  virtual PortTypeBody *clone() const;
  virtual void set_fullname(const string& p_fullname);
  virtual void set_my_scope(Common::Scope *p_scope);
  void set_my_type(Common::Type *p_type);
  PortOperationMode_t get_operation_mode() const { return operation_mode; }
  TypeSet *get_in_msgs() const;
  TypeSet *get_out_msgs() const;
  TypeSet *get_in_sigs() const;
  TypeSet *get_out_sigs() const;
  Definitions *get_vardefs() const;
  bool is_realtime() const { return realtime; }
  bool has_queue() const;
  bool getreply_allowed() const;
  bool catch_allowed() const;
  bool is_internal() const;
  FormalParList* get_map_parameters(bool map) const;
  /** Returns the address type that can be used in communication operations
   * on this port type. NULL is returned if addressing inside SUT is not
   * supported or the address type does not exist. */
  Common::Type *get_address_type();
  TestPortAPI_t get_testport_type() const { return testport_type; }
  void set_testport_type(TestPortAPI_t p_testport_type)
    { testport_type = p_testport_type; }
  PortType_t get_type() const { return port_type; }
  void add_provider_attribute();
  void add_user_attribute(Ttcn::Reference **p_provider_refs, size_t n_provider_refs,
    TypeMappings *p_in_mappings, TypeMappings *p_out_mappings, bool p_legacy = true);
  Common::Type *get_provider_type() const;
private:
  void chk_list(const Types *list, bool is_in, bool is_out);
  void chk_user_attribute();
public:
  void chk();
  void chk_map_translation();
  void chk_connect_translation();
  void chk_attributes(Ttcn::WithAttribPath *w_attrib_path);
  /** Returns whether the outgoing messages and signatures of \a this are
   * on the incoming lists of \a p_other. */
  bool is_connectable(PortTypeBody *p_other) const;
  /** Reports the error messages about the outgoing types of \a this that
   * are not handled by the incoming lists of \a p_other.
   * Applicable only if \a is_connectable() returned false. */
  void report_connection_errors(PortTypeBody *p_other) const;
  /** Returns whether \a this as the type of a test component port can be
   * mapped to a system port with type \a p_other. */
  bool is_mappable(PortTypeBody *p_other) const;
  /** Reports all errors that prevent mapping of \a this as the type of a
   * test component port to system port \a p_other.
   * Applicable only if \a is_mappable() returned false. */
  void report_mapping_errors(PortTypeBody *p_other) const;
  /** True if the PortTypeBody has translation capability towards p_other */
  bool is_translate(PortTypeBody *p_other) const;
  /** Special case when mapping a port that has out procedure/message but 
   * the \a p_other does not have any, and \a p_other has in procedure/message but 'this'  
   * does not have any. In that case it is not possible to send or receive anything.
   */
  bool map_can_receive_or_send(PortTypeBody *p_other) const;
  /** Special case when connecting a port that has no out procedure/message
   * and p_other has no out procedure/message.
   * In that case it is not possible to send or receive anything.
   */
  bool connect_can_receive_or_send(PortTypeBody *p_other) const;
  Type* get_my_type() const { return my_type; }
  bool is_legacy() const { return legacy; }
  //void add_mapper_type(Type* t) { mapper_types.add(t); }
  void generate_code(output_struct *target);
  virtual void dump(unsigned level) const;
};

/** Class to represent an "extension" attribute.
 *
 */
class ExtensionAttribute : public Location {
public:
  enum extension_t { NONE, PROTOTYPE, ENCODE, DECODE, PORT_API, // 0-4
    PORT_TYPE_USER, PORT_TYPE_PROVIDER, ERRORBEHAVIOR, ANYTYPELIST, // 5-8
    ENCDECVALUE, VERSION, VERSION_TEMPLATE, REQUIRES, REQ_TITAN, // 9-13
    TRANSPARENT, PRINTING // 14-15
  };

  /// Constructor for the PROTOTYPE type (convert, fast, backtrack, sliding)
  ExtensionAttribute(Def_Function_Base::prototype_t p)
  : Location(), type_(PROTOTYPE), value_()
  {
    value_.proto_ = p;
  }

  /// Constructor for the ENCODE/DECODE type
  ExtensionAttribute(extension_t t, Type::MessageEncodingType_t met,
    string *enc_opt)
  : Location(), type_(t), value_()
  {
    if (type_ != ENCODE && type_ != DECODE)
      FATAL_ERROR("ExtensionAttribute::ExtensionAttribute()");
    value_.encdec_.mess_ = met;
    value_.encdec_.s_ = enc_opt; // may be NULL
  }

  /// Constructor for the ERRORBEHAVIOR type
  ExtensionAttribute(ErrorBehaviorList *ebl)
  : Location(), type_(ERRORBEHAVIOR), value_()
  {
    if (ebl == NULL) FATAL_ERROR("ExtensionAttribute::ExtensionAttribute()");
    value_.ebl_ = ebl;
  }
  
  /// Constructor for the PRINTING type
  ExtensionAttribute(PrintingType *pt)
  : Location(), type_(PRINTING), value_()
  {
    if (NULL == pt) {
      FATAL_ERROR("ExtensionAttribute::ExtensionAttribute()");
    }
    value_.pt_ = pt;
  }

  /// Constructor for the PORT_API type (internal or address)
  ExtensionAttribute(PortTypeBody::TestPortAPI_t api)
  : Location(), type_(PORT_API), value_()
  {
    value_.api_ = api;
  }

  /// Constructor for the PORT_TYPE_PROVIDER and TRANSPARENT type
  ExtensionAttribute(extension_t type)
  : Location(), type_(type), value_()
  {
    switch (type) {
    case PORT_TYPE_PROVIDER:
    case TRANSPARENT:
      break;
    default:
      FATAL_ERROR("ExtensionAttribute::ExtensionAttribute()");
    }
  }

  /// Constructor for the USER type
  ExtensionAttribute(Reference *ref, TypeMappings *in, TypeMappings *out)
  : Location(), type_(PORT_TYPE_USER), value_()
  {
    if (in == NULL && out == NULL) // one may be null but not both
      FATAL_ERROR("ExtensionAttribute::ExtensionAttribute()");
    value_.user_.ref_ = ref;
    value_.user_.inmaps_ = in;
    value_.user_.outmaps_ = out;
  }

  /// Constructor for the ANYTYPELIST type
  ExtensionAttribute(Types *t)
  : Location(), type_(ANYTYPELIST), value_()
  {
    if (t == NULL) FATAL_ERROR("ExtensionAttribute::ExtensionAttribute()");
    value_.anytypes_ = t;
  }

  /** Constructor for the ENCDECVALUE type
   *
   * @param in  "in"  type mappings
   * @param out "out" type mappings
   * @note at most one out of \a in and \a out may be NULL
   */
  ExtensionAttribute(TypeMappings *in, TypeMappings *out)
  : Location(), type_(ENCDECVALUE), value_()
  {
    if (in == NULL && out == NULL)
      FATAL_ERROR("ExtensionAttribute::ExtensionAttribute");
    value_.encdecvalue_.inmaps_ = in;
    value_.encdecvalue_.outmaps_ = out;
  }

  /** Constructor for the VERSION type
   *
   * @param ver contains the unparsed version
   * (a string according to the template "RnXnn")
   *
   * Parses \p ver. If parsing is successful, the ExtensionAttribute
   * becomes responsible for \p ver, otherwise the caller has to free it.
   */
  ExtensionAttribute(const char* ABCClass, int type_number, int sequence,
    int suffix, Identifier *ver, enum version_t version_type);

  /** Constructor for the REQUIRES type
   *
   * @param mod contains the module name
   * @param ver contains the unparsed version
   * (a string according to the template "RnXnn")
   *
   * Parses \p ver. If successful, ExtensionAttribute becomes responsible
   * for both (stores \p mod and deletes \p ver).
   * If unsuccessful, freeing the identifiers remains the caller's duty.
   */
  ExtensionAttribute(Identifier *mod, const char* ABCClass, int type_number,
    int sequence, int suffix, Identifier *ver, enum version_t version_type);

  /** Constructor for the REQ_TITAN type or the VERSION_TEMPLATE type
   *
   * @param et either REQ_TITAN or VERSION_TEMPLATE
   * @param ver contains the unparsed version
   * (a string according to the template "RnXnn" for REQ_TITAN
   * or exactly "RnXnn" - the template itself - for VERSION_TEMPLATE)
   *
   * Parses \p ver. If parsing is successful, the ExtensionAttribute
   * becomes responsible for \p ver, otherwise the caller has to free it.
   */
  ExtensionAttribute(const char* ABCClass, int type_number, int sequence,
    int suffix, Identifier* ver, extension_t et, enum version_t version_type);

  ~ExtensionAttribute();

  /// Return the type of the attribute
  extension_t get_type() const { return type_; }

  /** @name Accessors
   *  @{
   *  All these functions act as extractors: resource ownership is passed
   *  to the caller, and type is set to NONE. Therefore only one of these
   *  functions can be called during object's life, and only once!
   */

  /// Return the data for the anytype
  /// @pre type_ is ANYTYPELIST, or else FATAL_ERROR
  /// @post type_ is NONE
  Types *get_types();

  /// Return the data for the prototype  (convert, fast, backtrack, sliding)
  /// @pre type_ is PROTOTYPE, or else FATAL_ERROR
  /// @post type_ is NONE
  Ttcn::Def_Function_Base::prototype_t get_proto();

  /// Return the data for the encode/decode attribute
  /// @pre type_ is ENCODE or DECODE, or else FATAL_ERROR
  /// @post type_ is NONE
  void get_encdec_parameters(Type::MessageEncodingType_t &p_encoding_type,
    string *&p_encoding_options);

  /// Return the error behavior list
  /// @pre type_ is ERRORBEHAVIOR, or else FATAL_ERROR
  /// @post type_ is NONE
  ErrorBehaviorList *get_eb_list();
  
  /// Return the printing type
  /// @pre type_ is PRINTING, or else FATAL_ERROR
  /// @post type_ is NONE
  PrintingType *get_printing();

  /// Return the data for (internal or address)
  /// @pre type_ is PORT_API, or else FATAL_ERROR
  /// @post type_ is NONE
  PortTypeBody::TestPortAPI_t get_api();

  /// Return the data for the "user" attribute
  /// @pre type_ is USER, or else FATAL_ERROR
  /// @post type_ is NONE
  void get_user(Reference *&ref, TypeMappings *&in, TypeMappings *&out);

  /// Returns the in and out mappings
  /// @pre type_ is ENCDECVALUE
  void get_encdecvalue_mappings(TypeMappings *&in, TypeMappings *&out);

  /** Return version information
   *
   * @pre type_ is REQUIRES, REQ_TITAN, VERSION or VERSION_TEMPLATE
   *
   * @param[out] product_number a string like "CRL 113 200"
   * @param[out] suffix the /3
   * @param[out] rel release
   * @param[out] patch patch version
   * @param[out] bld build number
   * @return pointer to the identifier of the module; the caller must not free
   */
  Common::Identifier *get_id(char*& product_number, unsigned int& suffix,
      unsigned int& rel, unsigned int& patch, unsigned int& bld, char*& extra, enum version_t& version_type);
  /// @}
private:
  /// Attribute type.
  extension_t type_;
  /** Internal data.
   *  All pointers are owned by the object and deleted in the destructor.
   */
  union {
    Ttcn::Def_Function_Base::prototype_t proto_;
    struct {
      Common::Type::MessageEncodingType_t  mess_;
      string *s_;
    } encdec_;
    ErrorBehaviorList *ebl_;
    PortTypeBody::TestPortAPI_t api_;
    struct {
      Reference *ref_;
      TypeMappings *inmaps_;
      TypeMappings *outmaps_;
    } user_;
    struct {
      TypeMappings *inmaps_;
      TypeMappings *outmaps_;
    } encdecvalue_;
    Types *anytypes_;
    struct {
      Common::Identifier *module_;
      char* productNumber_; ///< "CRL 113 200" for CRL versions, "CAX 105 7730" for CAX versions, "" for current version
      unsigned int suffix_; ///< The "/3"
      unsigned int release_;///< release
      unsigned int patch_;  ///< patch
      unsigned int build_;  ///< build number
      char* extra_; ///< extra junk at the end, for titansim
      enum version_t version_type_; ///< possible values: UNKNOWN_VERSION_TYPE, LEGACY_CRL, LEGACY_CAX and DOT_SEPARATED for the current version
    } version_;
    PrintingType *pt_;
  } value_;

  void check_product_number(const char* ABCClass, int type_number, int sequence);
  void parse_version(Identifier *ver);
};

/** Container for ExtensionAttribute.
 *  Private inheritance means is-implemented-in-terms-of. */
class ExtensionAttributes : private vector<ExtensionAttribute> {
public:
  ExtensionAttributes() : vector<ExtensionAttribute>()
  {}
  ~ExtensionAttributes();
  void add(ExtensionAttribute *a);

  ExtensionAttribute &get(size_t idx) const
  {
    return *vector<ExtensionAttribute>::operator[](idx);
  }

  /** Take over the elements of another container.
   *
   * @param other source container, will be emptied.
   */
  void import(ExtensionAttributes *other);

  /// Return the number of elements
  size_t size() const { return vector<ExtensionAttribute>::size(); }
};


class ClassTypeBody : public Common::Scope, public Common::Location {
  Common::Identifier* class_id; // not owned
  Def_Type* my_def; // pointer to the class type definition (not owned)
  boolean external;
  boolean final;
  boolean abstract;
  boolean trait;
  boolean built_in;
  Common::Type* base_type;
  ClassTypeBody* base_class; // not owned
  Types* base_traits;
  Reference* runs_on_ref;
  Type* runs_on_type;
  Reference* mtc_ref;
  Type* mtc_type;
  Reference* system_ref;
  Type* system_type;
  Definitions* members;
  StatementBlock* finally_block;
  /** set during semantic analysis to either a pointer to the constructor in
    * 'members', or the actual default constructor (if no constructor is defined) */
  Def_Constructor* constructor;
  map<string, Def_AbsFunction> abstract_functions;
  bool checked;
  bool default_constructor; /// true if the class uses a default constructor
  map<FormalPar*, char> defpar_list;
  static FormalParList* object_toString_fplist;
  static FormalParList* object_equals_fplist;
  
public:
  ClassTypeBody(Common::Identifier* p_class_id, boolean p_external, boolean p_final,
    boolean p_abstract, boolean p_trait, Types* p_base_types,
    Ttcn::Reference* p_runs_on_ref, Reference* p_mtc_ref, Reference* p_system_ref,
    Definitions* p_members, StatementBlock* p_finally_block);
  ClassTypeBody();
  ClassTypeBody(const ClassTypeBody& p);
  ClassTypeBody* clone() const;
  virtual ~ClassTypeBody();
  
  void set_my_def(Def_Type* p_def);
  Def_Type* get_my_def() { return my_def; }
  boolean is_abstract() const { return abstract; }
  boolean is_trait() const { return trait; }
  boolean is_external() const { return external; }
  bool has_default_constructor() const { return default_constructor; }
  
  void set_fullname(const string& p_fullname);
  void set_my_scope(Scope* p_scope);
  void dump(unsigned level) const;
  
  virtual bool is_class_scope() const { return true; }
  virtual ClassTypeBody* get_scope_class() { return this; }
  Common::Identifier* get_id() const { return class_id; }
  Def_Constructor* get_constructor();
  Common::Type* get_base_type() const { return base_type; }
  ClassTypeBody* get_base_class();
  boolean is_built_in() const { return built_in; }
  void add_defpar(FormalPar* p_defpar);
  char* get_defpar_type_name(FormalPar* p_defpar);
  
  Type* get_RunsOnType();
  Type* get_MtcType();
  Type* get_SystemType();
  
  bool is_parent_class(const ClassTypeBody* p_class);
  bool has_local_ass_withId(const Identifier& p_id);
  Common::Assignment* get_local_ass_byId(const Identifier& p_id);
  Common::Assignment* get_ass_bySRef(Common::Ref_simple* p_ref);
  
  static bool compare_members(ClassTypeBody* c1, ClassTypeBody* c2, Location* subclass_loc = NULL);
  bool chk_visibility(Common::Assignment* ass, Common::Location* usage_loc,
    Common::Scope* usage_scope);
  void chk();
  void chk_recursions(ReferenceChain& refch);
  
  void generate_code(output_struct* target);
  void generate_class_skeleton(output_struct* target);

  static FormalParList* get_object_method_fplist(const string& p_id);
  static Common::Type* get_object_method_return_type(const string& p_id);
  static void object_method_cleanup();
};

}

#endif /* TTCNSTUFF_H_ */
