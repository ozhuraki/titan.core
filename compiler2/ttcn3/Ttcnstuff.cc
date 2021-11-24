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
 *   Kovacs, Ferenc
 *   Raduly, Csaba
 *   Szabados, Kristof
 *   Szabo, Bence Janos
 *   Zalanyi, Balazs Andor
 *
 ******************************************************************************/
#include "Ttcnstuff.hh"
#include "../Int.hh"
#include "../CompilerError.hh"
#include "AST_ttcn3.hh"
#include "../main.hh"
#include "Attributes.hh"
#include <errno.h>
#include "Statement.hh"
#include "Templatestuff.hh"
#include "TtcnTemplate.hh"
#include "../CompType.hh"
#include "../../common/path.h"
#include "../../common/userinfo.h"
#include "../../common/version_internal.h"
#include <stdlib.h>

// implemented in coding_attrib_p.y
extern Ttcn::ExtensionAttributes * parse_extattributes(
  Ttcn::WithAttribPath *w_attrib_path);

namespace Ttcn {

  // =================================
  // ===== ErrorBehaviorSetting
  // =================================

  ErrorBehaviorSetting *ErrorBehaviorSetting::clone() const
  {
    FATAL_ERROR("ErrorBehaviorSetting::clone");
  }

  void ErrorBehaviorSetting::dump(unsigned level) const
  {
    DEBUG(level, "%s : %s", error_type.c_str(), error_handling.c_str());
  }

  // =================================
  // ===== ErrorBehaviorList
  // =================================

  ErrorBehaviorList::~ErrorBehaviorList()
  {
    for (size_t i = 0; i < ebs_v.size(); i++)
      delete ebs_v[i];
    ebs_v.clear();
    ebs_m.clear();
  }

  ErrorBehaviorList *ErrorBehaviorList::clone() const
  {
    FATAL_ERROR("ErrorBehaviorList::clone");
  }

  void ErrorBehaviorList::set_fullname(const string& p_fullname)
  {
    Node::set_fullname(p_fullname);
    for (size_t i = 0; i < ebs_v.size(); i++)
      ebs_v[i]->set_fullname(p_fullname
        + ".<setting" + Common::Int2string(i + 1) + ">");
  }

  void ErrorBehaviorList::add_ebs(ErrorBehaviorSetting *p_ebs)
  {
    if (!p_ebs || checked) FATAL_ERROR("ErrorBehaviorList::add_ebs()");
    ebs_v.add(p_ebs);
  }

  void ErrorBehaviorList::steal_ebs(ErrorBehaviorList *p_eblist)
  {
    if (!p_eblist || checked || p_eblist->checked)
      FATAL_ERROR("ErrorBehaviorList::steal_ebs()");
    for (size_t i = 0; i < p_eblist->ebs_v.size(); i++)
      ebs_v.add(p_eblist->ebs_v[i]);
    p_eblist->ebs_v.clear();
    join_location(*p_eblist);
  }

  bool ErrorBehaviorList::has_setting(const string& p_error_type)
  {
    if (!checked) chk();
    return ebs_all != 0 || ebs_m.has_key(p_error_type);
  }

  string ErrorBehaviorList::get_handling(const string& p_error_type)
  {
    if (!checked) chk();
    if (ebs_m.has_key(p_error_type))
      return ebs_m[p_error_type]->get_error_handling();
    else if (ebs_all) return ebs_all->get_error_handling();
    else return string("DEFAULT");
  }

  void ErrorBehaviorList::chk()
  {
    if (checked) return;
    const string all_str("ALL");
    Common::Error_Context cntxt(this, "In error behavior list");
    for (size_t i = 0; i < ebs_v.size(); i++) {
      ErrorBehaviorSetting *ebs = ebs_v[i];
      const string& error_type = ebs->get_error_type();
      if (error_type == all_str) {
        if (ebs_all) {
          ebs->warning("Duplicate setting for error type `ALL'");
          ebs_all->warning("The previous setting is ignored");
        }
        ebs_all = ebs;
        if (!ebs_m.empty()) {
          ebs->warning("All settings before `ALL' are ignored");
          ebs_m.clear();
        }
      } else {
        if (ebs_m.has_key(error_type)) {
          ebs->warning("Duplicate setting for error type `%s'",
            error_type.c_str());
          ErrorBehaviorSetting*& ebs_ref = ebs_m[error_type];
          ebs_ref->warning("The previous setting is ignored");
          // replace the previous setting in the map
          ebs_ref = ebs;
        } else ebs_m.add(error_type, ebs);
        static const char * const valid_types[] = {
          "UNBOUND", "INCOMPL_ANY", "ENC_ENUM", "INCOMPL_MSG", "LEN_FORM",
          "INVAL_MSG", "REPR", "CONSTRAINT", "TAG", "SUPERFL", "EXTENSION",
          "DEC_ENUM", "DEC_DUPFLD", "DEC_MISSFLD", "DEC_OPENTYPE", "DEC_UCSTR",
          "LEN_ERR", "SIGN_ERR", "INCOMP_ORDER", "TOKEN_ERR", "LOG_MATCHING",
          "FLOAT_TR", "FLOAT_NAN", "OMITTED_TAG", "NEGTEST_CONFL", "EXTRA_DATA",
          NULL };
        bool type_found = false;
        for (const char * const *str = valid_types; *str; str++) {
          if (error_type == *str) {
            type_found = true;
            break;
          }
        }
        if (!type_found) {
          ebs->warning("String `%s' is not a valid error type",
            error_type.c_str());
        }
      }
      const string& error_handling = ebs->get_error_handling();
      static const char * const valid_handlings[] = {
        "DEFAULT", "ERROR", "WARNING", "IGNORE", NULL };
      bool handling_found = false;
      for (const char * const *str = valid_handlings; *str; str++) {
        if (error_handling == *str) {
          handling_found = true;
          break;
        }
      }
      if (!handling_found) {
        ebs->warning("String `%s' is not a valid error handling",
          error_handling.c_str());
      }
    }
    checked = true;
  }

  char *ErrorBehaviorList::generate_code(char *str)
  {
    if (!checked) FATAL_ERROR("ErrorBehaviorList::generate_code()");
    const string all_str("ALL");
    if (ebs_all) {
      str = mputprintf(str, "TTCN_EncDec::set_error_behavior("
        "TTCN_EncDec::ET_ALL, TTCN_EncDec::EB_%s);\n",
        ebs_all->get_error_handling().c_str());
    } else {
      // reset all error behavior to default
      str = mputstr(str, "TTCN_EncDec::set_error_behavior(TTCN_EncDec::ET_ALL, "
        "TTCN_EncDec::EB_DEFAULT);\n");
    }
    for (size_t i = 0; i < ebs_m.size(); i++) {
      ErrorBehaviorSetting *ebs = ebs_m.get_nth_elem(i);
      str = mputprintf(str, "TTCN_EncDec::set_error_behavior("
        "TTCN_EncDec::ET_%s, TTCN_EncDec::EB_%s);\n",
        ebs->get_error_type().c_str(), ebs->get_error_handling().c_str());
    }
    return str;
  }

  void ErrorBehaviorList::dump(unsigned level) const
  {
    DEBUG(level, "Error behavior: (%lu pcs.)", (unsigned long) ebs_v.size());
    for (size_t i = 0; i < ebs_v.size(); i++)
      ebs_v[i]->dump(level + 1);
  }
  
  // =================================
  // ===== PrintingType
  // =================================
  
  PrintingType *PrintingType::clone() const
  {
    FATAL_ERROR("PrintingType::clone");
  }
  
  char *PrintingType::generate_code(char *str)
  {
    return mputprintf(str, ", %d", (PT_PRETTY == printing) ? 1 : 0);
  }

  // =================================
  // ===== TypeMappingTarget
  // =================================

  TypeMappingTarget::TypeMappingTarget(Common::Type *p_target_type,
    TypeMappingType_t p_mapping_type)
    : Node(), Location(), target_type(p_target_type),
    mapping_type(p_mapping_type), checked(false)
  {
    if (p_mapping_type == TM_SIMPLE && p_target_type != NULL) { // acceptable
      target_type->set_ownertype(Type::OT_TYPE_MAP_TARGET, this);
    }
    else if (p_mapping_type == TM_DISCARD && p_target_type == NULL)
    {} // also acceptable but nothing to do
    else FATAL_ERROR("TypeMappingTarget::TypeMappingTarget()");
  }

  TypeMappingTarget::TypeMappingTarget(Common::Type *p_target_type,
    TypeMappingType_t p_mapping_type, Ttcn::Reference *p_function_ref)
    : Node(), Location(), target_type(p_target_type),
    mapping_type(p_mapping_type), checked(false)
  {
    if (!p_target_type || p_mapping_type != TM_FUNCTION || !p_function_ref)
      FATAL_ERROR("TypeMappingTarget::TypeMappingTarget()");
    u.func.function_ref = p_function_ref;
    u.func.function_ptr = 0;
    target_type->set_ownertype(Type::OT_TYPE_MAP_TARGET, this);
  }

  TypeMappingTarget::TypeMappingTarget(Common::Type *p_target_type,
    TypeMappingType_t p_mapping_type,
    Common::Type::MessageEncodingType_t p_coding_type, string *p_coding_options,
    ErrorBehaviorList *p_eb_list)
    : Node(), Location(), target_type(p_target_type),
    mapping_type(p_mapping_type), checked(false)
  {
    if (!p_target_type || (p_mapping_type != TM_ENCODE &&
      p_mapping_type != TM_DECODE))
      FATAL_ERROR("TypeMappingTarget::TypeMappingTarget()");
    u.encdec.coding_type = p_coding_type;
    u.encdec.coding_options = p_coding_options;
    u.encdec.eb_list = p_eb_list;
    target_type->set_ownertype(Type::OT_TYPE_MAP_TARGET, this);
  }

  TypeMappingTarget::~TypeMappingTarget()
  {
    delete target_type;
    switch (mapping_type) {
    case TM_FUNCTION:
      delete u.func.function_ref;
      break;
    case TM_ENCODE:
    case TM_DECODE:
      delete u.encdec.coding_options;
      delete u.encdec.eb_list;
    default:
      break;
    }
  }

  TypeMappingTarget *TypeMappingTarget::clone() const
  {
    FATAL_ERROR("TypeMappingTarget::clone");
  }

  void TypeMappingTarget::set_fullname(const string& p_fullname)
  {
    Node::set_fullname(p_fullname);
    if (target_type) target_type->set_fullname(p_fullname + ".<target_type>");
    switch (mapping_type) {
    case TM_SIMPLE:
    case TM_DISCARD:
      break;
    case TM_FUNCTION:
      u.func.function_ref->set_fullname(p_fullname + ".<function_ref>");
      break;
    case TM_ENCODE:
    case TM_DECODE:
      if (u.encdec.eb_list)
        u.encdec.eb_list->set_fullname(p_fullname + ".<errorbehavior>");
      break;
    default:
      FATAL_ERROR("TypeMappingTarget::set_fullname()");
    }
  }

  void TypeMappingTarget::set_my_scope(Common::Scope *p_scope)
  {
    if (target_type) target_type->set_my_scope(p_scope);
    switch (mapping_type) {
    case TM_SIMPLE:
    case TM_DISCARD:
    case TM_ENCODE:
    case TM_DECODE:
      break;
    case TM_FUNCTION:
      u.func.function_ref->set_my_scope(p_scope);
      break;
    default:
      FATAL_ERROR("TypeMappingTarget::set_my_scope()");
    }
  }

  const char *TypeMappingTarget::get_mapping_name() const
  {
    switch (mapping_type) {
    case TM_SIMPLE:
      return "simple";
    case TM_DISCARD:
      return "discard";
    case TM_FUNCTION:
      return "function";
    case TM_ENCODE:
      return "encode";
    case TM_DECODE:
      return "decode";
    default:
      return "<unknown mapping>";
    }
  }

  Ttcn::Def_Function_Base *TypeMappingTarget::get_function() const
  {
    if (mapping_type != TM_FUNCTION || !checked)
      FATAL_ERROR("TypeMappingTarget::get_function()");
    return u.func.function_ptr;
  }

  Type::MessageEncodingType_t TypeMappingTarget::get_coding_type() const
  {
    if (mapping_type != TM_ENCODE && mapping_type != TM_DECODE)
      FATAL_ERROR("TypeMappingTarget::get_coding_type()");
    return u.encdec.coding_type;
  }

  bool TypeMappingTarget::has_coding_options() const
  {
    if (mapping_type != TM_ENCODE && mapping_type != TM_DECODE)
      FATAL_ERROR("TypeMappingTarget::has_coding_options()");
    return u.encdec.coding_options != 0;
  }

  const string& TypeMappingTarget::get_coding_options() const
  {
    if ((mapping_type != TM_ENCODE && mapping_type != TM_DECODE) ||
        !u.encdec.coding_options)
      FATAL_ERROR("TypeMappingTarget::get_coding_options()");
    return *u.encdec.coding_options;
  }

  ErrorBehaviorList *TypeMappingTarget::get_eb_list() const
  {
    if (mapping_type != TM_ENCODE && mapping_type != TM_DECODE)
      FATAL_ERROR("TypeMappingTarget::get_eb_list()");
    return u.encdec.eb_list;
  }

  void TypeMappingTarget::chk_simple(Type *source_type)
  {
    Error_Context cntxt(this, "In `simple' mapping");
    if (!source_type->is_identical(target_type)) {
      target_type->error("The source and target types must be the same: "
        "`%s' was expected instead of `%s'",
        source_type->get_typename().c_str(),
        target_type->get_typename().c_str());
    }
  }

  void TypeMappingTarget::chk_function(Type *source_type, Type *port_type, bool legacy, bool incoming)
  {
    Error_Context cntxt(this, "In `function' mapping");
    Common::Assignment *t_ass = u.func.function_ref->get_refd_assignment(false);
    if (!t_ass) return;
    t_ass->chk();
    switch (t_ass->get_asstype()) {
    case Common::Assignment::A_FUNCTION:
    case Common::Assignment::A_FUNCTION_RVAL:
    case Common::Assignment::A_FUNCTION_RTEMP:
      break;
    case Common::Assignment::A_EXT_FUNCTION:
    case Common::Assignment::A_EXT_FUNCTION_RVAL:
    case Common::Assignment::A_EXT_FUNCTION_RTEMP:
      // External functions are not allowed when the standard like behaviour is used
      if (legacy) {
        break;
      }
    default:
      u.func.function_ref->error("Reference to a function%s"
        " was expected instead of %s",
        legacy ? " or external function" : "",
        t_ass->get_description().c_str());
      return;
    }
    u.func.function_ptr = dynamic_cast<Ttcn::Def_Function_Base*>(t_ass);
    if (!u.func.function_ptr) FATAL_ERROR("TypeMappingTarget::chk_function()");
    if (legacy && u.func.function_ptr->get_prototype() ==
        Ttcn::Def_Function_Base::PROTOTYPE_NONE) {
      u.func.function_ref->error("The referenced %s does not have `prototype' "
        "attribute", u.func.function_ptr->get_description().c_str());
      return;
    }
   
    if (!legacy && u.func.function_ptr->get_prototype() !=
        Ttcn::Def_Function_Base::PROTOTYPE_FAST) {
      u.func.function_ref->error("The referenced %s does not have `prototype' "
        "fast attribute", u.func.function_ptr->get_description().c_str());
      return;
    }
    Type *input_type = u.func.function_ptr->get_input_type();
    if (legacy && input_type && !source_type->is_identical(input_type)) {
      source_type->error("The input type of %s must be the same as the source "
        "type of the mapping: `%s' was expected instead of `%s'",
        u.func.function_ptr->get_description().c_str(),
        source_type->get_typename().c_str(),
        input_type->get_typename().c_str());
    }
    Type *output_type = u.func.function_ptr->get_output_type();
    if (legacy && output_type && !target_type->is_identical(output_type)) {
      target_type->error("The output type of %s must be the same as the "
        "target type of the mapping: `%s' was expected instead of `%s'",
        u.func.function_ptr->get_description().c_str(),
        target_type->get_typename().c_str(),
        output_type->get_typename().c_str());
    }
    
    //  The standard like behaviour has different function param checking
    if (!legacy) {
      // In the error message the source type is the target_type
      // and the target type is the source_type for a reason.
      // Reason: In the new standard like behaviour the conversion functions 
      // has the correct param order for in and out parameters
      // (which is more logical than the old behaviour)
      // For example:
      // in octetstring from integer with int_to_oct()
      //         |              |             |
      //    target_type     source_type   conv. func.
      if (incoming) {
        if (input_type && !target_type->is_identical(input_type)) {
          target_type->error("The input type of %s must be the same as the "
            "source type of the mapping: `%s' was expected instead of `%s'",
            u.func.function_ptr->get_description().c_str(),
            target_type->get_typename().c_str(),
            input_type->get_typename().c_str());
        }
        if (output_type && !source_type->is_identical(output_type)) {
          target_type->error("The output type of %s must be the same as the "
            "target type of the mapping: `%s' was expected instead of `%s'",
            u.func.function_ptr->get_description().c_str(),
            source_type->get_typename().c_str(),
            output_type->get_typename().c_str());
        }
      } else {
        // For example:
        // out octetstring to integer with oct_to_int()
        //         |              |             |
        //    source_type     target_type   conv. func.
        if (input_type && !source_type->is_identical(input_type)) {
          target_type->error("The input type of %s must be the same as the "
            "source type of the mapping: `%s' was expected instead of `%s'",
            u.func.function_ptr->get_description().c_str(),
            source_type->get_typename().c_str(),
            input_type->get_typename().c_str());
        }
        if (output_type && !target_type->is_identical(output_type)) {
          target_type->error("The output type of %s must be the same as the "
            "target type of the mapping: `%s' was expected instead of `%s'",
            u.func.function_ptr->get_description().c_str(),
            target_type->get_typename().c_str(),
            output_type->get_typename().c_str());
        }
      }
      
      // Check that if the function has a port clause it matches the port type
      // that it is defined in.
      Type *port_clause = u.func.function_ptr->get_PortType();
      if (!legacy && port_clause && port_clause != port_type) {
        u.func.function_ref->error("The function %s has a port clause of `%s'"
          " but referenced in another port `%s'",
          u.func.function_ptr->get_description().c_str(),
          port_type->get_dispname().c_str(),
          port_clause->get_dispname().c_str());
      }
    }
  }

  void TypeMappingTarget::chk_encode(Type *source_type)
  {
    Error_Context cntxt(this, "In `encode' mapping");
    if (!source_type->has_encoding(u.encdec.coding_type)) {
      source_type->error("Source type `%s' does not support %s encoding",
        source_type->get_typename().c_str(),
        Type::get_encoding_name(u.encdec.coding_type));
    }
    Type *stream_type = Type::get_stream_type(u.encdec.coding_type, 1);
    if (!stream_type->is_identical(target_type)) {
      target_type->error("Target type of %s encoding should be `%s' instead "
        "of `%s'", Type::get_encoding_name(u.encdec.coding_type),
        stream_type->get_typename().c_str(),
        target_type->get_typename().c_str());
    }
    if (u.encdec.eb_list) u.encdec.eb_list->chk();
  }

  void TypeMappingTarget::chk_decode(Type *source_type)
  {
    Error_Context cntxt(this, "In `decode' mapping");
    Type *stream_type = Type::get_stream_type(u.encdec.coding_type, 1);
    if (!stream_type->is_identical(source_type)) {
      source_type->error("Source type of %s encoding should be `%s' instead "
        "of `%s'", Type::get_encoding_name(u.encdec.coding_type),
        stream_type->get_typename().c_str(),
        source_type->get_typename().c_str());
    }
    if (!target_type->has_encoding(u.encdec.coding_type)) {
      target_type->error("Target type `%s' does not support %s encoding",
        target_type->get_typename().c_str(),
        Type::get_encoding_name(u.encdec.coding_type));
    }
    if (u.encdec.eb_list) u.encdec.eb_list->chk();
  }

  void TypeMappingTarget::chk(Type *source_type, Type *port_type, bool legacy, bool incoming)
  {
    if (checked) return;
    checked = true;
    if (target_type) {
      Error_Context cntxt(target_type, "In target type");
      target_type->chk();
    }
    switch (mapping_type) {
    case TM_SIMPLE:
      chk_simple(source_type);
      break;
    case TM_DISCARD:
      break;
    case TM_FUNCTION:
      chk_function(source_type, port_type, legacy, incoming);
      break;
    case TM_ENCODE:
      chk_encode(source_type);
      break;
    case TM_DECODE:
      chk_decode(source_type);
      break;
    default:
      FATAL_ERROR("TypeMappingTarget::chk()");
    }
  }

  bool TypeMappingTarget::fill_type_mapping_target(
    port_msg_type_mapping_target *target, Type *source_type,
    Scope *p_scope, stringpool& pool)
  {
    bool has_sliding = false;
    if (target_type) {
      target->target_name = pool.add(target_type->get_genname_value(p_scope));
      target->target_dispname = pool.add(target_type->get_typename());
    } else {
      target->target_name = NULL;
      target->target_dispname = NULL;
    }
    switch (mapping_type) {
    case TM_SIMPLE:
      target->mapping_type = M_SIMPLE;
      break;
    case TM_DISCARD:
      target->mapping_type = M_DISCARD;
      break;
    case TM_FUNCTION:
      target->mapping_type = M_FUNCTION;
      if (!u.func.function_ptr)
        FATAL_ERROR("TypeMappingTarget::fill_type_mapping_target()");
      target->mapping.function.name =
        pool.add(u.func.function_ptr->get_genname_from_scope(p_scope));
      target->mapping.function.dispname =
        pool.add(u.func.function_ptr->get_fullname());
      switch (u.func.function_ptr->get_prototype()) {
      case Ttcn::Def_Function_Base::PROTOTYPE_CONVERT:
        target->mapping.function.prototype = PT_CONVERT;
        break;
      case Ttcn::Def_Function_Base::PROTOTYPE_FAST:
        target->mapping.function.prototype = PT_FAST;
        break;
      case Ttcn::Def_Function_Base::PROTOTYPE_BACKTRACK:
        target->mapping.function.prototype = PT_BACKTRACK;
        break;
      case Ttcn::Def_Function_Base::PROTOTYPE_SLIDING:
        target->mapping.function.prototype = PT_SLIDING;
        has_sliding = true;
        break;
      default:
        FATAL_ERROR("TypeMappingTarget::fill_type_mapping_target()");
      }
      break;
    case TM_ENCODE:
    case TM_DECODE:
      if (mapping_type == TM_ENCODE) {
        target->mapping_type = M_ENCODE;
        target->mapping.encdec.typedescr_name =
          pool.add(source_type->get_genname_typedescriptor(p_scope));
      } else {
        target->mapping_type = M_DECODE;
        target->mapping.encdec.typedescr_name =
          pool.add(target_type->get_genname_typedescriptor(p_scope));
      }
      target->mapping.encdec.encoding_type =
        Type::get_encoding_name(u.encdec.coding_type);
      if (u.encdec.coding_options) target->mapping.encdec.encoding_options =
          u.encdec.coding_options->c_str();
      else target->mapping.encdec.encoding_options = NULL;
      if (u.encdec.eb_list) {
        char *str = u.encdec.eb_list->generate_code(memptystr());
        target->mapping.encdec.errorbehavior = pool.add(string(str));
        Free(str);
      } else {
        target->mapping.encdec.errorbehavior =
          "TTCN_EncDec::set_error_behavior(TTCN_EncDec::ET_ALL, "
            "TTCN_EncDec::EB_DEFAULT);\n";
      }
      break;
    default:
      FATAL_ERROR("TypeMappingTarget::fill_type_mapping_target()");
    }
    return has_sliding;
  }

  void TypeMappingTarget::dump(unsigned level) const
  {
    DEBUG(level, "target:");
    if (target_type) target_type->dump(level + 1);
    else DEBUG(level + 1, "<none>");
    DEBUG(level, "mapping type: %s", get_mapping_name());
    switch (mapping_type) {
    case TM_FUNCTION:
      u.func.function_ref->dump(level + 1);
      break;
    case TM_ENCODE:
    case TM_DECODE:
      DEBUG(level + 1, "encoding: %s",
        Type::get_encoding_name(u.encdec.coding_type));
      if (u.encdec.coding_options)
        DEBUG(level + 1, "options: %s", u.encdec.coding_options->c_str());
      if (u.encdec.eb_list) u.encdec.eb_list->dump(level + 1);
    default:
      break;
    }
  }

  // =================================
  // ===== TypeMappingTargets
  // =================================

  TypeMappingTargets::~TypeMappingTargets()
  {
    size_t nof_targets = targets_v.size();
    for (size_t i = 0; i < nof_targets; i++) delete targets_v[i];
    targets_v.clear();
  }

  TypeMappingTargets *TypeMappingTargets::clone() const
  {
    FATAL_ERROR("TypeMappingTargets::clone");
  }

  void TypeMappingTargets::set_fullname(const string& p_fullname)
  {
    Node::set_fullname(p_fullname);
    size_t nof_targets = targets_v.size();
    for (size_t i = 0; i < nof_targets; i++) {
      targets_v[i]->set_fullname(p_fullname + ".<target" + Int2string(i + 1)
        + ">");
    }
  }

  void TypeMappingTargets::set_my_scope(Scope *p_scope)
  {
    size_t nof_targets = targets_v.size();
    for (size_t i = 0; i < nof_targets; i++)
      targets_v[i]->set_my_scope(p_scope);
  }

  void TypeMappingTargets::add_target(TypeMappingTarget *p_target)
  {
    if (!p_target) FATAL_ERROR("TypeMappingTargets::add_target()");
    targets_v.add(p_target);
  }

  void TypeMappingTargets::dump(unsigned level) const
  {
    size_t nof_targets = targets_v.size();
    DEBUG(level, "Targets: (%lu pcs.)", (unsigned long) nof_targets);
    for (size_t i = 0; i < nof_targets; i++) targets_v[i]->dump(level + 1);
  }

  // =================================
  // ===== TypeMapping
  // =================================

  TypeMapping::TypeMapping(Type *p_source_type, TypeMappingTargets *p_targets)
    : Node(), Location(), source_type(p_source_type), targets(p_targets)
  {
    if (!p_source_type || !p_targets)
      FATAL_ERROR("TypeMapping::TypeMapping()");
    source_type->set_ownertype(Type::OT_TYPE_MAP, this);
  }

  TypeMapping::~TypeMapping()
  {
    delete source_type;
    delete targets;
  }

  TypeMapping *TypeMapping::clone() const
  {
    FATAL_ERROR("TypeMapping::clone");
  }

  void TypeMapping::set_fullname(const string& p_fullname)
  {
    Node::set_fullname(p_fullname);
    source_type->set_fullname(p_fullname + ".<source_type>");
    targets->set_fullname(p_fullname);
  }

  void TypeMapping::set_my_scope(Scope *p_scope)
  {
    source_type->set_my_scope(p_scope);
    targets->set_my_scope(p_scope);
  }

  void TypeMapping::chk(Type *port_type, bool legacy, bool incoming)
  {
    Error_Context cntxt(this, "In type mapping");
    {
      Error_Context cntxt2(source_type, "In source type");
      source_type->chk();
    }
    size_t nof_targets = targets->get_nof_targets();
    bool has_sliding = false, has_non_sliding = false;
    for (size_t i = 0; i < nof_targets; i++) {
      TypeMappingTarget *target = targets->get_target_byIndex(i);
      target->chk(source_type, port_type, legacy, incoming);
      if (nof_targets > 1) {
        switch (target->get_mapping_type()) {
        case TypeMappingTarget::TM_DISCARD:
          if (has_sliding) target->error("Mapping `discard' cannot be used "
            "if functions with `prototype(sliding)' are referred from the same "
            "source type");
          else if (i < nof_targets - 1) target->error("Mapping `discard' must "
            "be the last target of the source type");
          break;
        case TypeMappingTarget::TM_FUNCTION: {
          Ttcn::Def_Function_Base *t_function = target->get_function();
          if (t_function) {
            switch (t_function->get_prototype()) {
            case Ttcn::Def_Function_Base::PROTOTYPE_NONE:
              break;           
            case Ttcn::Def_Function_Base::PROTOTYPE_BACKTRACK:
              has_non_sliding = true;
              break;
            case Ttcn::Def_Function_Base::PROTOTYPE_SLIDING:
              has_sliding = true;
              break;
            case Ttcn::Def_Function_Base::PROTOTYPE_FAST:
              // New standard like behaviour has no such limit
              if (!legacy) {
                break;
              }
            default:
              target->error("The referenced %s must have the attribute "
                "`prototype(backtrack)' or `prototype(sliding)' when more "
                "than one targets are present",
                t_function->get_description().c_str());
            }
          }
          break; }
        case TypeMappingTarget::TM_DECODE:
          break;
        default:
          target->error("The type of the mapping must be `function', `decode' "
            "or `discard' instead of `%s' when more than one targets are "
            "present", target->get_mapping_name());
        }
      }
    }
    if (has_sliding && has_non_sliding) {
      error("If one of the mappings refers to a function with attribute "
        "`prototype(sliding)' then mappings of this source type cannot refer "
        "to functions with attribute `prototype(backtrack)'");
    }
  }

  void TypeMapping::dump(unsigned level) const
  {
    DEBUG(level, "Source type:");
    source_type->dump(level + 1);
    targets->dump(level);
  }

  // =================================
  // ===== TypeMappings
  // =================================

  TypeMappings::~TypeMappings()
  {
    size_t nof_mappings = mappings_v.size();
    for (size_t i = 0; i < nof_mappings; i++) delete mappings_v[i];
    mappings_v.clear();
    mappings_m.clear();
  }

  TypeMappings *TypeMappings::clone() const
  {
    FATAL_ERROR("TypeMappings::clone");
  }

  void TypeMappings::set_fullname(const string& p_fullname)
  {
    Node::set_fullname(p_fullname);
    size_t nof_mappings = mappings_v.size();
    for (size_t i = 0; i < nof_mappings; i++) {
      mappings_v[i]->set_fullname(p_fullname + ".<mapping" + Int2string(i + 1)
        + ">");
    }
  }

  void TypeMappings::set_my_scope(Scope *p_scope)
  {
    size_t nof_mappings = mappings_v.size();
    for (size_t i = 0; i < nof_mappings; i++)
      mappings_v[i]->set_my_scope(p_scope);
  }

  void TypeMappings::add_mapping(TypeMapping *p_mapping)
  {
    if (checked || !p_mapping) FATAL_ERROR("TypeMappings::add_mapping()");
    mappings_v.add(p_mapping);
  }

  void TypeMappings::steal_mappings(TypeMappings *p_mappings)
  {
    if (checked || !p_mappings || p_mappings->checked)
      FATAL_ERROR("TypeMappings::steal_mappings()");
    size_t nof_mappings = p_mappings->mappings_v.size();
    for (size_t i = 0; i < nof_mappings; i++)
      mappings_v.add(p_mappings->mappings_v[i]);
    p_mappings->mappings_v.clear();
    join_location(*p_mappings);
  }

  bool TypeMappings::has_mapping_for_type(Type *p_type) const
  {
    if (!checked || !p_type)
      FATAL_ERROR("TypeMappings::has_mapping_for_type()");
    if (p_type->get_type_refd_last()->get_typetype() == Type::T_ERROR)
      return true;
    else return mappings_m.has_key(p_type->get_typename());
  }

  TypeMapping *TypeMappings::get_mapping_byType(Type *p_type) const
  {
    if (!checked || !p_type)
      FATAL_ERROR("TypeMappings::get_mapping_byType()");
    return mappings_m[p_type->get_typename()];
  }

  void TypeMappings::chk(Type *port_type, bool legacy, bool incoming)
  {
    if (checked) return;
    checked = true;
    size_t nof_mappings = mappings_v.size();
    for (size_t i = 0; i < nof_mappings; i++) {
      TypeMapping *mapping = mappings_v[i];
      mapping->chk(port_type, legacy, incoming);
      Type *source_type = mapping->get_source_type();
      if (source_type->get_type_refd_last()->get_typetype() != Type::T_ERROR) {
        const string& source_type_name = source_type->get_typename();
        if (mappings_m.has_key(source_type_name)) {
          const char *source_type_name_str = source_type_name.c_str();
          source_type->error("Duplicate mapping for type `%s'",
            source_type_name_str);
          mappings_m[source_type_name]->note("The mapping of type `%s' is "
            "already given here", source_type_name_str);
        } else mappings_m.add(source_type_name, mapping);
      }
    }
  }

  void TypeMappings::dump(unsigned level) const
  {
    size_t nof_mappings = mappings_v.size();
    DEBUG(level, "type mappings: (%lu pcs.)", (unsigned long) nof_mappings);
    for (size_t i = 0; i < nof_mappings; i++)
      mappings_v[i]->dump(level + 1);
  }

  // =================================
  // ===== Types
  // =================================

  Types::~Types()
  {
    size_t nof_types = types.size();
    for (size_t i = 0; i < nof_types; i++) delete types[i];
    types.clear();
  }

  Types *Types::clone() const
  {
    FATAL_ERROR("Types::clone");
  }

  void Types::add_type(Type *p_type)
  {
    if (!p_type) FATAL_ERROR("Types::add_type()");
    p_type->set_ownertype(Type::OT_TYPE_LIST, this);
    types.add(p_type);
  }

  Type *Types::extract_type_byIndex(size_t n)
  {
    Type *retval = types[n];
    types[n] = 0;
    return retval;
  }

  void Types::steal_types(Types *p_tl)
  {
    if (!p_tl) FATAL_ERROR("Types::steal_types()");
    size_t nof_types = p_tl->types.size();
    for (size_t i = 0; i < nof_types; i++) types.add(p_tl->types[i]);
    p_tl->types.clear();
    join_location(*p_tl);
  }

  void Types::set_fullname(const string& p_fullname)
  {
    Node::set_fullname(p_fullname);
    size_t nof_types = types.size();
    for (size_t i = 0; i < nof_types; i++)
      types[i]->set_fullname(p_fullname + ".<type" + Int2string(i + 1) + ">");
  }

  void Types::set_my_scope(Scope *p_scope)
  {
    size_t nof_types = types.size();
    for (size_t i = 0; i < nof_types; i++) types[i]->set_my_scope(p_scope);
  }

  void Types::dump(unsigned level) const
  {
    size_t nof_types = types.size();
    DEBUG(level, "Types: (%lu pcs.)", (unsigned long) nof_types);
    for (size_t i = 0; i < nof_types; i++) types[i]->dump(level + 1);
  }

  // =================================
  // ===== TypeSet
  // =================================

  TypeSet::~TypeSet()
  {
    types_v.clear();
    types_m.clear();
  }

  TypeSet *TypeSet::clone() const
  {
    FATAL_ERROR("TypeSet::clone()");
  }

  void TypeSet::add_type(Type *p_type)
  {
    if (!p_type) FATAL_ERROR("TypeSet::add_type()");
    types_v.add(p_type);
    types_m.add(p_type->get_typename(), p_type);
  }

  bool TypeSet::has_type(Type *p_type) const
  {
    if (!p_type) FATAL_ERROR("TypeSet::has_type()");
    if (p_type->get_type_refd_last()->get_typetype() == Type::T_ERROR)
      return true;
    else return types_m.has_key(p_type->get_typename());
  }

  size_t TypeSet::get_nof_compatible_types(Type *p_type) const
  {
    if (!p_type) FATAL_ERROR("TypeSet::get_nof_compatible_types()");
    if (p_type->get_type_refd_last()->get_typetype() == Type::T_ERROR) {
      // Return a positive answer for erroneous types.
      return 1;
    } else {
      size_t ret_val = 0;
      size_t nof_types = types_v.size();
      for (size_t i = 0; i < nof_types; i++) {
        // Don't allow type compatibility.
        if (types_v[i]->is_compatible(p_type, NULL, NULL)) ret_val++;
      }
      return ret_val;
    }
  }

  size_t TypeSet::get_index_byType(Type *p_type) const
  {
    if (!p_type) FATAL_ERROR("TypeSet::get_index_byType()");
    const string& name = p_type->get_typename();
    size_t nof_types = types_v.size();
    for (size_t i = 0; i < nof_types; i++)
      if (types_v[i]->get_typename() == name) return i;
    FATAL_ERROR("TypeSet::get_index_byType()");
    return static_cast<size_t>(-1);
  }

  void TypeSet::dump(unsigned level) const
  {
    size_t nof_types = types_v.size();
    DEBUG(level, "Types: (%lu pcs.)", (unsigned long) nof_types);
    for (size_t i = 0; i < nof_types; i++) types_v[i]->dump(level + 1);
  }

  // =================================
  // ===== PortTypeBody
  // =================================

  PortTypeBody::PortTypeBody(PortOperationMode_t p_operation_mode,
    Types *p_in_list, Types *p_out_list, Types *p_inout_list,
    bool p_in_all, bool p_out_all, bool p_inout_all, Definitions *defs,
    bool p_realtime, FormalParList *p_map_params, FormalParList *p_unmap_params)
    : Node(), Location(), my_type(0), operation_mode(p_operation_mode),
    in_list(p_in_list), out_list(p_out_list), inout_list(p_inout_list),
    in_all(p_in_all), out_all(p_out_all), inout_all(p_inout_all),
    checked(false), attributes_checked(false), legacy(true),
    in_msgs(0), out_msgs(0), in_sigs(0), out_sigs(0),
    testport_type(TP_REGULAR), port_type(PT_REGULAR),
    provider_refs(), provider_types(),
    in_mappings(0), out_mappings(0), vardefs(defs), realtime(p_realtime),
    map_params(p_map_params), unmap_params(p_unmap_params)
  {
  }

  PortTypeBody::~PortTypeBody()
  {
    delete in_list;
    delete out_list;
    delete inout_list;
    delete in_msgs;
    delete out_msgs;
    delete in_sigs;
    delete out_sigs;
    for (size_t i = 0; i < provider_refs.size(); i++) {
      delete provider_refs[i];
    }
    provider_refs.clear();
    provider_types.clear();
    delete in_mappings;
    delete out_mappings;
    delete vardefs;
    delete map_params;
    delete unmap_params;
  }

  PortTypeBody *PortTypeBody::clone() const
  {
    FATAL_ERROR("PortTypeBody::clone");
  }

  void PortTypeBody::set_fullname(const string& p_fullname)
  {
    Node::set_fullname(p_fullname);
    if (in_list) in_list->set_fullname(p_fullname + ".<in_list>");
    if (out_list) out_list->set_fullname(p_fullname + ".<out_list>");
    if (inout_list) inout_list->set_fullname(p_fullname + ".<inout_list>");
    if (in_msgs) in_msgs->set_fullname(p_fullname + ".<incoming_messages>");
    if (out_msgs) out_msgs->set_fullname(p_fullname + ".<outgoing_messages>");
    if (in_sigs) in_sigs->set_fullname(p_fullname + ".<incoming_signatures>");
    if (out_sigs) out_sigs->set_fullname(p_fullname + ".<outgoing_signatures>");
    for (size_t i = 0; i < provider_refs.size(); i++) {
      provider_refs[i]->set_fullname(p_fullname + ".<provider_ref>");
    }
    if (in_mappings) in_mappings->set_fullname(p_fullname + ".<in_mappings>");
    if (out_mappings) out_mappings->set_fullname(p_fullname + ".<out_mappings>");
    vardefs->set_fullname(p_fullname + ".<port_var>");
    if (map_params != NULL) {
      map_params->set_fullname(p_fullname + ".<map_params>");
    }
    if (unmap_params != NULL) {
      unmap_params->set_fullname(p_fullname + ".<unmap_params>");
    }
  }

  void PortTypeBody::set_my_scope(Scope *p_scope)
  {
    if (in_list) in_list->set_my_scope(p_scope);
    if (out_list) out_list->set_my_scope(p_scope);
    if (inout_list) inout_list->set_my_scope(p_scope);
    for (size_t i = 0; i < provider_refs.size(); i++) {
      provider_refs[i]->set_my_scope(p_scope);
    }
    if (in_mappings) in_mappings->set_my_scope(p_scope);
    if (out_mappings) out_mappings->set_my_scope(p_scope);
    vardefs->set_parent_scope(p_scope);
    if (map_params != NULL) {
      map_params->set_my_scope(p_scope);
    }
    if (unmap_params != NULL) {
      unmap_params->set_my_scope(p_scope);
    }
  }

  void PortTypeBody::set_my_type(Type *p_type)
  {
    if (!p_type || p_type->get_typetype() != Type::T_PORT)
      FATAL_ERROR("PortTypeBody::set_my_type()");
    my_type = p_type;
  }

  TypeSet *PortTypeBody::get_in_msgs() const
  {
    if (!checked) FATAL_ERROR("PortTypeBody::get_in_msgs()");
    return in_msgs;
  }

  TypeSet *PortTypeBody::get_out_msgs() const
  {
    if (!checked) FATAL_ERROR("PortTypeBody::get_out_msgs()");
    return out_msgs;
  }

  TypeSet *PortTypeBody::get_in_sigs() const
  {
    if (!checked) FATAL_ERROR("PortTypeBody::get_in_sigs()");
    return in_sigs;
  }

  TypeSet *PortTypeBody::get_out_sigs() const
  {
    if (!checked) FATAL_ERROR("PortTypeBody::get_out_sigs()");
    return out_sigs;
  }
  
  Definitions *PortTypeBody::get_vardefs() const
  {
    if (!checked) FATAL_ERROR("PortTypeBody::get_vardefs()");
    return vardefs;
  }

  bool PortTypeBody::has_queue() const
  {
    if (!checked) FATAL_ERROR("PortTypeBody::has_queue()");
    if (in_msgs || in_sigs) return true;
    if (out_sigs) {
      size_t nof_sigs = out_sigs->get_nof_types();
      for (size_t i = 0; i < nof_sigs; i++) {
        Type *t_sig = out_sigs->get_type_byIndex(i)->get_type_refd_last();
        if (!t_sig->is_nonblocking_signature() ||
            t_sig->get_signature_exceptions()) return true;
      }
    }
    return false;
  }

  bool PortTypeBody::getreply_allowed() const
  {
    if (!checked) FATAL_ERROR("PortTypeBody::getreply_allowed()");
    if (out_sigs) {
      size_t nof_sigs = out_sigs->get_nof_types();
      for (size_t i = 0; i < nof_sigs; i++) {
        if (!out_sigs->get_type_byIndex(i)->get_type_refd_last()
            ->is_nonblocking_signature()) return true;
      }
    }
    return false;
  }

  bool PortTypeBody::catch_allowed() const
  {
    if (!checked) FATAL_ERROR("PortTypeBody::catch_allowed()");
    if (out_sigs) {
      size_t nof_sigs = out_sigs->get_nof_types();
      for (size_t i = 0; i < nof_sigs; i++) {
        if (out_sigs->get_type_byIndex(i)->get_type_refd_last()
            ->get_signature_exceptions()) return true;
      }
    }
    return false;
  }

  bool PortTypeBody::is_internal() const
  {
    if (!checked) FATAL_ERROR("PortTypeBody::is_internal_port()");
    return testport_type == TP_INTERNAL;
  }
  
  FormalParList* PortTypeBody::get_map_parameters(bool map) const
  {
    return map ? map_params : unmap_params;
  }

  Type *PortTypeBody::get_address_type()
  {
    if (!checked) FATAL_ERROR("PortTypeBody::get_address_type()");
    if (testport_type == TP_ADDRESS) {
      Type *t;
      // in case of 'user' port types the address visible and supported by the
      // 'provider' port type is relevant
      // TODO: currently not supported the address in the new standard like behaviour
      if (port_type == PT_USER && provider_types.size() > 0) t = provider_types[0];
      else t = my_type;
      return t->get_my_scope()->get_scope_mod()->get_address_type();
    } else {
      // the port type does not support SUT addresses
      return 0;
    }
  }

  void PortTypeBody::add_provider_attribute()
  {
    port_type = PT_PROVIDER;
    for (size_t i = 0; i < provider_refs.size(); i++) {
    delete provider_refs[i];
    }
    provider_refs.clear();
    provider_types.clear();
    delete in_mappings;
    in_mappings = 0;
    delete out_mappings;
    out_mappings = 0;
  }

  void PortTypeBody::add_user_attribute(Ttcn::Reference **p_provider_refs,
    size_t n_provider_refs, TypeMappings *p_in_mappings, TypeMappings *p_out_mappings,
    bool p_legacy)
  {
    if (!p_provider_refs || n_provider_refs == 0 || !my_type)
      FATAL_ERROR("PortTypeBody::add_user_attribute()");
    Scope *t_scope = my_type->get_my_scope();
    const string& t_fullname = get_fullname();
    port_type = PT_USER;
    for (size_t i = 0; i < provider_refs.size(); i++) {
      delete provider_refs[i];
    }
    provider_refs.clear();
    for (size_t i = 0; i < n_provider_refs; i++) {
      provider_refs.add(p_provider_refs[i]);
      provider_refs[i]->set_fullname(t_fullname + ".<provider_ref>");
      provider_refs[i]->set_my_scope(t_scope);
    }
    provider_types.clear(); // Do not delete provider_types
    delete in_mappings;
    in_mappings = p_in_mappings;
    if (in_mappings) {
      in_mappings->set_fullname(t_fullname + ".<in_mappings>");
      in_mappings->set_my_scope(t_scope);
    }
    delete out_mappings;
    out_mappings = p_out_mappings;
    if (out_mappings) {
      out_mappings->set_fullname(t_fullname + ".<out_mappings>");
      out_mappings->set_my_scope(t_scope);
    }
    legacy = p_legacy;
  }

  Type *PortTypeBody::get_provider_type() const
  {
    if (!checked || port_type != PT_USER)
      FATAL_ERROR("PortTypeBody::get_provider_type()");
    return provider_types[0];
  }

  void PortTypeBody::chk_list(const Types *list, bool is_in, bool is_out)
  {
    const char *err_msg;
    if (is_in) {
      if (is_out) err_msg = "sent or received";
      else err_msg = "received";
    } else if (is_out) err_msg = "sent";
    else {
      FATAL_ERROR("PortTypeBody::chk_list()");
      err_msg = 0;
    }
    size_t nof_types = list->get_nof_types();
    for (size_t i = 0; i < nof_types; i++) {
      Type *t = list->get_type_byIndex(i);
      t->chk();
      // check if a value/template of this type can leave the component
      if (t->is_component_internal()) {
        map<Type*,void> type_chain;
        t->chk_component_internal(type_chain, "sent or received on a port");
      }
      Type *t_last = t->get_type_refd_last();
      switch (t_last->get_typetype()) {
      case Type::T_ERROR:
        // silently ignore
        break;
      case Type::T_SIGNATURE:
        if (operation_mode == PO_MESSAGE) {
          t->error("Signature `%s' cannot be used on a message based port",
            t_last->get_typename().c_str());
        }
        if (is_in) {
          if (in_sigs && in_sigs->has_type(t_last)) {
            const string& type_name = t_last->get_typename();
            t->error("Duplicate incoming signature `%s'", type_name.c_str());
            in_sigs->get_type_byName(type_name)->note("Signature `%s' is "
              "already listed here", type_name.c_str());
          } else {
            if (!in_sigs) {
              in_sigs = new TypeSet;
              in_sigs->set_fullname(get_fullname() + ".<incoming_signatures>");
            }
            in_sigs->add_type(t);
          }
        }
        if (is_out) {
          if (out_sigs && out_sigs->has_type(t_last)) {
            const string& type_name = t_last->get_typename();
            t->error("Duplicate outgoing signature `%s'", type_name.c_str());
            out_sigs->get_type_byName(type_name)->note("Signature `%s' is "
              "already listed here", type_name.c_str());
          } else {
            if (!out_sigs) {
              out_sigs = new TypeSet;
              out_sigs->set_fullname(get_fullname() + ".<outgoing_signatures>");
            }
            out_sigs->add_type(t);
          }
        }
        break;
      default:
        // t is a normal data type
        if (operation_mode == PO_PROCEDURE) {
          t->error("Data type `%s' cannot be %s on a procedure based port",
            t_last->get_typename().c_str(), err_msg);
        }
        if (is_in) {
          if (in_msgs && in_msgs->has_type(t_last)) {
            const string& type_name = t_last->get_typename();
            t->error("Duplicate incoming message type `%s'", type_name.c_str());
            in_msgs->get_type_byName(type_name)->note("Type `%s' is already "
              "listed here", type_name.c_str());
          } else {
            if (!in_msgs) {
              in_msgs = new TypeSet;
              in_msgs->set_fullname(get_fullname() + ".<incoming_messages>");
            }
            in_msgs->add_type(t);
          }
        }
        if (is_out) {
          if (out_msgs && out_msgs->has_type(t_last)) {
            const string& type_name = t_last->get_typename();
            t->error("Duplicate outgoing message type `%s'", type_name.c_str());
            out_msgs->get_type_byName(type_name)->note("Type `%s' is already "
              "listed here", type_name.c_str());
          } else {
            if (!out_msgs) {
              out_msgs = new TypeSet;
              out_msgs->set_fullname(get_fullname() + ".<outgoing_messages>");
            }
            out_msgs->add_type(t);
          }
        }
      }
    }
  }

  void PortTypeBody::chk_map_translation() {
    
    if (port_type != PT_USER || legacy) {
      FATAL_ERROR("PortTypeBody::chk_map_translation()");
    }
    
    TypeSet mapping_ins;
    
    if (in_mappings) {
      for (size_t i = 0; i < in_mappings->get_nof_mappings(); i++) {
        TypeMapping * mapping = in_mappings->get_mapping_byIndex(i);
        for (size_t j = 0; j < mapping->get_nof_targets(); j++) {
          if (!mapping_ins.has_type(mapping->get_target_byIndex(j)->get_target_type())) {
            mapping_ins.add_type(mapping->get_target_byIndex(j)->get_target_type());
          }
        }
      }
    }
    
    for (size_t i = 0; i < provider_types.size(); i++) {
      PortTypeBody *provider_body = provider_types[i]->get_PortBody();
      if (provider_body->in_msgs) {
        for (size_t j = 0; j < provider_body->in_msgs->get_nof_types(); j++) {
          bool found = false;
          if (inout_list) {
            for (size_t k = 0; k < inout_list->get_nof_types(); k++) {
              if (provider_body->in_msgs->has_type(inout_list->get_type_byIndex(k))) {
                found = true;
                break;
              }
            }
          }
          if (((in_msgs && in_msgs->has_type(provider_body->in_msgs->get_type_byIndex(j))) // Provider in message is present in the port in message
              || mapping_ins.has_type(provider_body->in_msgs->get_type_byIndex(j)) // Provider in message is present in one of the in mappings
              || found // Provider in message is present in the inout list of the port 
              ) == false) {
            error("Incoming message type `%s' is not present in the in(out) message list or in the from mapping types, coming from port: `%s'.",
              provider_body->in_msgs->get_type_byIndex(j)->get_typename().c_str(),
              provider_types[i]->get_dispname().c_str());
          }
        }
      } // if provider_body->in_msgs
      
      if (inout_list) {
        for (size_t j = 0; j < inout_list->get_nof_types(); j++) {
          bool found_in = false;
          if (provider_body->in_msgs) {
            // If the inout message of the port is present on the provider in message list
            for (size_t k = 0; k < provider_body->in_msgs->get_nof_types(); k++) {
              if (inout_list->get_type_byIndex(j)->get_typename() ==
                  provider_body->in_msgs->get_type_byIndex(k)->get_typename()) {
                found_in = true;
                break;
              }
            }
          }
          bool found_out = false;
          if (provider_body->out_msgs) {
            // If the inout message of the port is present on the provider out message list
            for (size_t k = 0; k < provider_body->out_msgs->get_nof_types(); k++) {
              if (inout_list->get_type_byIndex(j)->get_typename() ==
                  provider_body->out_msgs->get_type_byIndex(k)->get_typename()) {
                found_out = true;
                break;
              }
            }
          }
          
          // Error if the inout message of the port is not present on the in and out message list
          if (!found_in || !found_out) {
            error("Inout message type `%s' is not present on the in and out messages or the inout messages of port `%s'.",
              inout_list->get_type_byIndex(j)->get_typename().c_str(),
              provider_types[i]->get_dispname().c_str());
          }
        }
      } // if inout_list
      
      if (out_list) {
        for (size_t j = 0; j < out_list->get_nof_types(); j++) {
          bool found = false;
          if (provider_body->out_msgs) {
            for (size_t k = 0; k < provider_body->out_msgs->get_nof_types(); k++) {
              if (out_msgs->has_type(provider_body->out_msgs->get_type_byIndex(k))) {
                found = true;
                break;
              }
            }
          }
          
          // Check if the port's out message list contains at least one of the 
          // type's target mappings.
          if (!found) {
            if (out_mappings->has_mapping_for_type(out_msgs->get_type_byIndex(j))) {
              TypeMapping* type_mapping = out_mappings->get_mapping_byType(out_msgs->get_type_byIndex(j));
              for (size_t k = 0; k < type_mapping->get_nof_targets(); k++) {
                if (provider_body->out_msgs->has_type(type_mapping->get_target_byIndex(k)->get_target_type())) {
                  found = true;
                  break;
                }
              }
            }
          }
          
          if (!found) {
            error("Neither out message type `%s', nor one of its target"
              "mappings are present in the out or inout message list of the port `%s'.",
              out_list->get_type_byIndex(j)->get_typename().c_str(),
              provider_types[i]->get_dispname().c_str());
          }
        }
      } // if out_list
    } // for provider_types.size()
  }
  
  void PortTypeBody::chk_connect_translation() {
    FATAL_ERROR("PortTypeBody::chk_connect_translation(): Not implemented");
  }
  
  void PortTypeBody::chk_user_attribute()
  {
    Error_Context cntxt(this,
      legacy ? "In extension attribute `user'" : "In translation capability");
    // check the reference that points to the provider type
    provider_types.clear();
    PortTypeBody *provider_body = 0;
    size_t n_prov_t = 0;
    for (size_t p = 0; p < provider_refs.size(); p++) {
      provider_body = 0;
      Common::Assignment *t_ass = provider_refs[p]->get_refd_assignment(); // provider port
      if (t_ass) {
        if (t_ass->get_asstype() == Common::Assignment::A_TYPE) {
          Type *t = t_ass->get_Type()->get_type_refd_last();
          if (t->get_typetype() == Type::T_PORT) {
            bool found = false;
            // Provider types can only be given once.
            for (size_t i = 0; i < provider_types.size(); i++) {
              if (provider_types[i] == t) {
                found = true;
                my_type->error("Duplicate port mappings, the type `%s' appears more than once.",
                  t->get_dispname().c_str());
                break;
              }
            }
            if (!found) {
              provider_types.add(t);
              n_prov_t++;
              provider_body = t->get_PortBody();
            }
          } else {
            provider_refs[p]->error("Type reference `%s' does not refer to a port "
              "type", provider_refs[p]->get_dispname().c_str());
          }
        } else {
          provider_refs[p]->error("Reference `%s' does not refer to a type",
            provider_refs[p]->get_dispname().c_str());
        }
      }

      // checking the consistency of attributes in this and provider_body
      if (provider_body && testport_type != TP_INTERNAL) {
        if (provider_body->port_type != PT_PROVIDER) {
          provider_refs[p]->error("The referenced port type `%s' must have the "
            "`provider' attribute", provider_types[n_prov_t-1]->get_typename().c_str());
        }
        switch (provider_body->testport_type) {
        case TP_REGULAR:
          if (testport_type == TP_ADDRESS) {
            provider_refs[p]->error("Attribute `address' cannot be used because the "
              "provider port type `%s' does not have attribute `address'",
              provider_types[n_prov_t-1]->get_typename().c_str());
          }
          break;
        case TP_INTERNAL:
          provider_refs[p]->error("Missing attribute `internal'. Provider port "
            "type `%s' has attribute `internal', which must be also present here",
            provider_types[n_prov_t-1]->get_typename().c_str());
        case TP_ADDRESS:
          break;
        default:
          FATAL_ERROR("PortTypeBody::chk_attributes()");
        }
        // inherit the test port API type from the provider
        testport_type = provider_body->testport_type;
      }
    } // for
    
    // checking the incoming mappings
    if (legacy && in_mappings) {
      Error_Context cntxt2(in_mappings, "In `in' mappings");
      in_mappings->chk(my_type, legacy, true);
      // checking source types
      if (provider_body) {
        if (provider_body->in_msgs) {
          // check if all source types are present on the `in' list of the
          // provider
          size_t nof_mappings = in_mappings->get_nof_mappings();
          for (size_t i = 0; i < nof_mappings; i++) {
            Type *source_type =
              in_mappings->get_mapping_byIndex(i)->get_source_type();
            if (!provider_body->in_msgs->has_type(source_type)) {
              source_type->error("Source type `%s' of the `in' mapping is not "
                "present on the list of incoming messages in provider port "
                "type `%s'", source_type->get_typename().c_str(),
                provider_types[0]->get_typename().c_str());
            }
          }
          // check if all types of the `in' list of the provider are handled by
          // the mappings
          size_t nof_msgs = provider_body->in_msgs->get_nof_types();
          for (size_t i = 0; i < nof_msgs; i++) {
            Type *msg_type = provider_body->in_msgs->get_type_byIndex(i);
            if (!in_mappings->has_mapping_for_type(msg_type)) {
              in_mappings->error("Incoming message type `%s' of provider "
                "port type `%s' is not handled by the incoming mappings",
                msg_type->get_typename().c_str(),
                provider_types[0]->get_typename().c_str());
            }
          }
        } else {
          in_mappings->error("Invalid incoming mappings. Provider port type "
            "`%s' does not have incoming message types",
            provider_types[0]->get_typename().c_str());
        }
      }
      // checking target types
      size_t nof_mappings = in_mappings->get_nof_mappings();
      for (size_t i = 0; i < nof_mappings; i++) {
        TypeMapping *mapping = in_mappings->get_mapping_byIndex(i);
        size_t nof_targets = mapping->get_nof_targets();
        for (size_t j = 0; j < nof_targets; j++) {
          Type *target_type =
            mapping->get_target_byIndex(j)->get_target_type();
          if (target_type && (!in_msgs || !in_msgs->has_type(target_type))) {
            target_type->error("Target type `%s' of the `in' mapping is not "
              "present on the list of incoming messages in user port type "
              "`%s'", target_type->get_typename().c_str(),
              my_type->get_typename().c_str());
          }
        }
      }
    } else if (legacy && provider_body && provider_body->in_msgs) {
      error("Missing `in' mappings to handle the incoming message types of "
        "provider port type `%s'", provider_types[0]->get_typename().c_str());
    }

    // checking the outgoing mappings
    if (legacy && out_mappings) {
      Error_Context cntxt2(out_mappings, "In `out' mappings");
      out_mappings->chk(my_type, legacy, false);
      // checking source types
      if (out_msgs) {
        // check if all source types are present on the `out' list
        size_t nof_mappings = out_mappings->get_nof_mappings();
        for (size_t i = 0; i < nof_mappings; i++) {
          Type *source_type =
            out_mappings->get_mapping_byIndex(i)->get_source_type();
          if (!out_msgs->has_type(source_type)) {
            source_type->error("Source type `%s' of the `out' mapping is not "
              "present on the list of outgoing messages in user port type `%s'",
              source_type->get_typename().c_str(),
              my_type->get_typename().c_str());
          }
        }
        // check if all types of the `out' list are handled by the mappings
        size_t nof_msgs = out_msgs->get_nof_types();
        for (size_t i = 0; i < nof_msgs; i++) {
          Type *msg_type = out_msgs->get_type_byIndex(i);
          if (!out_mappings->has_mapping_for_type(msg_type)) {
            out_mappings->error("Outgoing message type `%s' of user port type "
              "`%s' is not handled by the outgoing mappings",
              msg_type->get_typename().c_str(),
              my_type->get_typename().c_str());
          }
        }
      } else {
        out_mappings->error("Invalid outgoing mappings. User port type "
          "`%s' does not have outgoing message types",
          my_type->get_typename().c_str());
      }
      // checking target types
      if (provider_body) {
        size_t nof_mappings = out_mappings->get_nof_mappings();
        for (size_t i = 0; i < nof_mappings; i++) {
          TypeMapping *mapping = out_mappings->get_mapping_byIndex(i);
          size_t nof_targets = mapping->get_nof_targets();
          for (size_t j = 0; j < nof_targets; j++) {
            Type *target_type =
              mapping->get_target_byIndex(j)->get_target_type();
            if (target_type && (!provider_body->out_msgs ||
                !provider_body->out_msgs->has_type(target_type))) {
              target_type->error("Target type `%s' of the `out' mapping is "
                "not present on the list of outgoing messages in provider "
                "port type `%s'", target_type->get_typename().c_str(),
                provider_types[0]->get_typename().c_str());
            }
          }
        }
      }
    } else if (legacy && out_msgs) {
      error("Missing `out' mappings to handle the outgoing message types of "
        "user port type `%s'", my_type->get_typename().c_str());
    }

    // checking the compatibility of signature lists
    if (legacy && provider_body) {
      if (in_sigs) {
        size_t nof_sigs = in_sigs->get_nof_types();
        for (size_t i = 0; i < nof_sigs; i++) {
          Type *t_sig = in_sigs->get_type_byIndex(i);
          if (!provider_body->in_sigs ||
              !provider_body->in_sigs->has_type(t_sig)) {
            Type *t_sig_last = t_sig->get_type_refd_last();
            if (!t_sig_last->is_nonblocking_signature() ||
                t_sig_last->get_signature_exceptions())
              t_sig->error("Incoming signature `%s' of user port type `%s' is "
                "not present on the list of incoming signatures in provider "
                "port type `%s'", t_sig->get_typename().c_str(),
                my_type->get_typename().c_str(),
                provider_types[0]->get_typename().c_str());
          }
        }
      }

      if (provider_body->in_sigs) {
        size_t nof_sigs = provider_body->in_sigs->get_nof_types();
        for (size_t i = 0; i < nof_sigs; i++) {
          Type *t_sig = provider_body->in_sigs->get_type_byIndex(i);
          if (!in_sigs || !in_sigs->has_type(t_sig)) {
            error("Incoming signature `%s' of provider port type `%s' is not "
              "present on the list of incoming signatures in user port type "
              "`%s'", t_sig->get_typename().c_str(),
              provider_types[0]->get_typename().c_str(),
              my_type->get_typename().c_str());
          }
        }
      }

      if (out_sigs) {
        size_t nof_sigs = out_sigs->get_nof_types();
        for (size_t i = 0; i < nof_sigs; i++) {
          Type *t_sig = out_sigs->get_type_byIndex(i);
          if (!provider_body->out_sigs ||
              !provider_body->out_sigs->has_type(t_sig)) {
            t_sig->error("Outgoing signature `%s' of user port type `%s' is "
              "not present on the list of outgoing signatures in provider port "
              "type `%s'", t_sig->get_typename().c_str(),
              my_type->get_typename().c_str(),
              provider_types[0]->get_typename().c_str());
          }
        }
      }

      if (provider_body->out_sigs) {
        size_t nof_sigs = provider_body->out_sigs->get_nof_types();
        for (size_t i = 0; i < nof_sigs; i++) {
          Type *t_sig = provider_body->out_sigs->get_type_byIndex(i);
          if (!out_sigs || !out_sigs->has_type(t_sig)) {
            Type *t_sig_last = t_sig->get_type_refd_last();
            if (!t_sig_last->is_nonblocking_signature() ||
                t_sig_last->get_signature_exceptions())
              error("Outgoing signature `%s' of provider port type `%s' is not "
                "present on the list of outgoing signatures in user port type "
                "`%s'", t_sig->get_typename().c_str(),
                provider_types[0]->get_typename().c_str(),
                my_type->get_typename().c_str());
          }
        }
      }
    }
    if (!legacy) {
      if (out_mappings) {
        out_mappings->chk(my_type, legacy, false);
      }
      if (in_mappings) {
        in_mappings->chk(my_type, legacy, true);
      }
      chk_map_translation();
      vardefs->chk_uniq();
      vardefs->chk();
    }
  }

  void PortTypeBody::chk()
  {
    if (checked) return;
    checked = true;

    // checking 'all' directives
    if (inout_all) {
      if (in_all) {
        warning("Redundant `in all' and `inout all' directives");
        in_all = false;
      }
      if (out_all) {
        warning("Redundant `out all' and `inout all' directives");
        out_all = false;
      }
      warning("Unsupported `inout all' directive was ignored");
    } else {
      if (in_all) warning("Unsupported `in all' directive was ignored");
      if (out_all) warning("Unsupported `out all' directive was ignored");
    }

    // checking message/signature lists
    if (in_list) {
      Error_Context cntxt(in_list, "In `in' list");
      chk_list(in_list, true, false);
    }
    if (out_list) {
      Error_Context cntxt(out_list, "In `out' list");
      chk_list(out_list, false, true);
    }
    if (inout_list) {
      Error_Context cntxt(inout_list, "In `inout' list");
      chk_list(inout_list, true, true);
    }
    
    if (provider_refs.size() == 0 && vardefs->get_nof_asss() > 0) {
      error("Port variables can only be used when the port is a translation port.");
    }
    
    if (map_params != NULL) {
      Error_Context cntxt(map_params, "In `map' parameters");
      map_params->chk(Definition::A_PORT);
    }
    if (unmap_params != NULL) {
      Error_Context cntxt(unmap_params, "In `unmap' parameters");
      unmap_params->chk(Definition::A_PORT);
    }
  }

  void PortTypeBody::chk_attributes(Ttcn::WithAttribPath *w_attrib_path)
  {
    if (!w_attrib_path || !checked || !my_type)
      FATAL_ERROR("PortTypeBody::chk_attributes()");
    if (attributes_checked) {
      return;
    }
    attributes_checked = true;

    Ttcn::ExtensionAttributes * extarts = parse_extattributes(w_attrib_path);
    if (extarts != 0) { // NULL means parsing error
    size_t num_atrs = extarts->size();
    for (size_t k = 0; k < num_atrs; ++k) {
      ExtensionAttribute &ea = extarts->get(k);
      switch (ea.get_type()) {
      case ExtensionAttribute::PORT_API: { // internal or address
        const PortTypeBody::TestPortAPI_t api = ea.get_api();
        if (api == PortTypeBody::TP_INTERNAL) {
          switch (testport_type) {
          case PortTypeBody::TP_REGULAR:
            break;
          case PortTypeBody::TP_INTERNAL: {
            ea.warning("Duplicate attribute `internal'");
            break; }
          case PortTypeBody::TP_ADDRESS: {
            ea.error("Attributes `address' and `internal' "
              "cannot be used at the same time");
            break; }
          default:
            FATAL_ERROR("coding_attrib_parse(): invalid testport type");
          }
          set_testport_type(PortTypeBody::TP_INTERNAL);
        }
        else if (api == PortTypeBody::TP_ADDRESS) {
          switch (testport_type) {
          case PortTypeBody::TP_REGULAR:
            break;
          case PortTypeBody::TP_INTERNAL: {
            ea.error("Attributes `internal' and `address' "
              "cannot be used at the same time");
            break; }
          case PortTypeBody::TP_ADDRESS: {
            ea.warning("Duplicate attribute `address'");
            break; }
          default:
            FATAL_ERROR("coding_attrib_parse(): invalid testport type");
          }
          set_testport_type(PortTypeBody::TP_ADDRESS);
        }
        break; }

      case ExtensionAttribute::PORT_TYPE_PROVIDER:
        switch (port_type) {
        case PortTypeBody::PT_REGULAR:
          break;
        case PortTypeBody::PT_PROVIDER: {
          ea.warning("Duplicate attribute `provider'");
          break; }
        case PortTypeBody::PT_USER: {
          if (legacy) {
          ea.error("Attributes `user' and `provider' "
            "cannot be used at the same time");
          } else {
            ea.error("The `provider' attribute "
            "cannot be used on translation ports");
          }
          break; }
        default:
          FATAL_ERROR("coding_attrib_parse(): invalid testport type");
        }
        add_provider_attribute();
        break;

      case ExtensionAttribute::PORT_TYPE_USER: {
        switch (port_type) {
        case PortTypeBody::PT_REGULAR:
          break;
        case PortTypeBody::PT_PROVIDER: {
          ea.error("Attributes `provider' and `user' "
            "cannot be used at the same time");
          break; }
        case PortTypeBody::PT_USER: {
          if (legacy) {
            ea.error("Duplicate attribute `user'");
          } else {
            ea.error("Attribute `user' cannot be used on translation ports.");
          }
          break; }
        default:
          FATAL_ERROR("coding_attrib_parse(): invalid testport type");
        }
        Reference *ref;
        TypeMappings *in;
        TypeMappings *out;
        ea.get_user(ref, in, out);
        Reference **refs = (Reference**)Malloc(sizeof(Reference*));
        refs[0] = ref;
        add_user_attribute(refs, 1, in, out);
        Free(refs);
        break; }

      case ExtensionAttribute::ANYTYPELIST:
        break; // ignore it

      case ExtensionAttribute::NONE:
        break; // ignore, do not issue "wrong type" error

      default:
        ea.error("Port can only have the following extension attributes: "
          "'provider', 'user', 'internal' or 'address'");
        break;
      } // switch
    } // next attribute
    delete extarts;
    } // if not null

    if (port_type == PT_USER) chk_user_attribute();
    else if (testport_type == TP_ADDRESS) {
      Error_Context cntxt(this, "In extension attribute `address'");
      Common::Module *my_mod = my_type->get_my_scope()->get_scope_mod();
      if (!my_mod->get_address_type())
        error("Type `address' is not defined in module `%s'",
          my_mod->get_modid().get_dispname().c_str());
    }
  }

  bool PortTypeBody::is_connectable(PortTypeBody *p_other) const
  {
    if (!checked || !p_other || !p_other->checked)
      FATAL_ERROR("Type::is_connectable()");
    if (out_msgs) {
      if (!p_other->in_msgs) return false;
      size_t nof_msgs = out_msgs->get_nof_types();
      for (size_t i = 0; i < nof_msgs; i++) {
        if (!p_other->in_msgs->has_type(out_msgs->get_type_byIndex(i)))
          return false;
      }
    }
    if (out_sigs) {
      if (!p_other->in_sigs) return false;
      size_t nof_sigs = out_sigs->get_nof_types();
      for (size_t i = 0; i < nof_sigs; i++) {
        if (!p_other->in_sigs->has_type(out_sigs->get_type_byIndex(i)))
          return false;
      }
    }
    return true;
  }

  void PortTypeBody::report_connection_errors(PortTypeBody *p_other) const
  {
    if (!checked || !my_type || !p_other || !p_other->checked ||
        !p_other->my_type)
      FATAL_ERROR("PortTypeBody::report_connection_errors()");
    const string& my_typename = my_type->get_typename();
    const char *my_typename_str = my_typename.c_str();
    const string& other_typename = p_other->my_type->get_typename();
    const char *other_typename_str = other_typename.c_str();
    if (out_msgs) {
      size_t nof_msgs = out_msgs->get_nof_types();
      for (size_t i = 0; i < nof_msgs; i++) {
        Type *t_msg = out_msgs->get_type_byIndex(i);
        if (!p_other->in_msgs || !p_other->in_msgs->has_type(t_msg)) {
          t_msg->note("Outgoing message type `%s' of port type `%s' is not "
            "present on the incoming list of port type `%s'",
            t_msg->get_typename().c_str(), my_typename_str, other_typename_str);
        }
      }
    }
    if (out_sigs) {
      size_t nof_sigs = out_sigs->get_nof_types();
      for (size_t i = 0; i < nof_sigs; i++) {
        Type *t_sig = out_sigs->get_type_byIndex(i);
        if (!p_other->in_sigs || !p_other->in_sigs->has_type(t_sig)) {
          t_sig->note("Outgoing signature `%s' of port type `%s' is not "
            "present on the incoming list of port type `%s'",
            t_sig->get_typename().c_str(), my_typename_str, other_typename_str);
        }
      }
    }
  }
  
  bool PortTypeBody::is_translate(PortTypeBody *p_other) const {
    for (size_t i = 0; i < provider_types.size(); i++) {
      if (provider_types[i]->get_fullname() == p_other->get_my_type()->get_fullname()) {
        return true;
      }
    }
    return false;
  }
  
  bool PortTypeBody::map_can_receive_or_send(PortTypeBody *p_other) const {
    if (operation_mode == PortTypeBody::PO_MESSAGE &&
        (out_msgs == NULL || out_msgs->get_nof_types() == 0) &&
        (p_other->in_msgs == NULL  || p_other->in_msgs->get_nof_types() == 0)) {
      return false;
    }
    if (operation_mode == PortTypeBody::PO_PROCEDURE &&
        (out_sigs == NULL || out_sigs->get_nof_types() == 0) &&
        (p_other->in_sigs == NULL  || p_other->in_sigs->get_nof_types() == 0)) {
      return false;
    }
    if (operation_mode == PortTypeBody::PO_MIXED &&
        (out_msgs == NULL || out_msgs->get_nof_types() == 0) &&
        (p_other->in_msgs == NULL  || p_other->in_msgs->get_nof_types() == 0) &&
        (out_sigs == NULL || out_sigs->get_nof_types() == 0) &&
        (p_other->in_sigs == NULL  || p_other->in_sigs->get_nof_types() == 0)) {
      return false;
    }
    return true;
  }
  
  bool PortTypeBody::connect_can_receive_or_send(PortTypeBody *p_other) const {
    if (operation_mode == PortTypeBody::PO_MESSAGE &&
        (out_msgs == NULL || out_msgs->get_nof_types() == 0) &&
        (p_other->out_msgs == NULL || p_other->out_msgs->get_nof_types() == 0)) {
      return false;
    }
    if (operation_mode == PortTypeBody::PO_PROCEDURE &&
        (out_sigs == NULL || out_sigs->get_nof_types() == 0) &&
        (p_other->out_sigs == NULL || p_other->out_sigs->get_nof_types() == 0)) {
      return false;
    }
    if (operation_mode == PortTypeBody::PO_MIXED &&
        (out_msgs == NULL || out_msgs->get_nof_types() == 0) &&
        (p_other->out_msgs == NULL || p_other->out_msgs->get_nof_types() == 0) &&
        (out_sigs == NULL || out_sigs->get_nof_types() == 0) &&
        (p_other->out_sigs == NULL || p_other->out_sigs->get_nof_types() == 0)) {
      return false;
    }
    return true;
  }

  bool PortTypeBody::is_mappable(PortTypeBody *p_other) const
  {
    if (!checked || !p_other || !p_other->checked)
      FATAL_ERROR("PortTypeBody::is_mappable()");
    // shortcut: every port type can be mapped to itself
    if (this == p_other) return true;
    // outgoing lists of this should be covered by outgoing lists of p_other
    if (out_msgs) {
      if (!p_other->out_msgs) return false;
      size_t nof_msgs = out_msgs->get_nof_types();
      for (size_t i = 0; i < nof_msgs; i++) {
        if (!p_other->out_msgs->has_type(out_msgs->get_type_byIndex(i)))
          return false;
      }
    }
    if (out_sigs) {
      if (!p_other->out_sigs) return false;
      size_t nof_sigs = out_sigs->get_nof_types();
      for (size_t i = 0; i < nof_sigs; i++) {
        if (!p_other->out_sigs->has_type(out_sigs->get_type_byIndex(i)))
          return false;
      }
    }
    // incoming lists of p_other should be covered by incoming lists of this
    if (p_other->in_msgs) {
      if (!in_msgs) return false;
      size_t nof_msgs = p_other->in_msgs->get_nof_types();
      for (size_t i = 0; i < nof_msgs; i++) {
        if (!in_msgs->has_type(p_other->in_msgs->get_type_byIndex(i)))
          return false;
      }
    }
    if (p_other->in_sigs) {
      if (!in_sigs) return false;
      size_t nof_sigs = p_other->in_sigs->get_nof_types();
      for (size_t i = 0; i < nof_sigs; i++) {
        if (!in_sigs->has_type(p_other->in_sigs->get_type_byIndex(i)))
          return false;
      }
    }
    return true;
  }

  void PortTypeBody::report_mapping_errors(PortTypeBody *p_other) const
  {
    if (!checked || !my_type || !p_other || !p_other->checked ||
        !p_other->my_type) FATAL_ERROR("PortTypeBody::report_mapping_errors()");
    const string& my_typename = my_type->get_typename();
    const char *my_typename_str = my_typename.c_str();
    const string& other_typename = p_other->my_type->get_typename();
    const char *other_typename_str = other_typename.c_str();
    if (out_msgs) {
      size_t nof_msgs = out_msgs->get_nof_types();
      for (size_t i = 0; i < nof_msgs; i++) {
        Type *t_msg = out_msgs->get_type_byIndex(i);
        if (!p_other->out_msgs || !p_other->out_msgs->has_type(t_msg)) {
          t_msg->note("Outgoing message type `%s' of test component "
            "port type `%s' is not present on the outgoing list of "
            "system port type `%s'", t_msg->get_typename().c_str(),
            my_typename_str, other_typename_str);
        }
      }
    }
    if (out_sigs) {
      size_t nof_sigs = out_sigs->get_nof_types();
      for (size_t i = 0; i < nof_sigs; i++) {
        Type *t_sig = out_sigs->get_type_byIndex(i);
        if (!p_other->out_sigs || !p_other->out_sigs->has_type(t_sig)) {
          t_sig->note("Outgoing signature `%s' of test component port type "
            "`%s' is not present on the outgoing list of system port type "
            "`%s'", t_sig->get_typename().c_str(), my_typename_str,
            other_typename_str);
        }
      }
    }
    if (p_other->in_msgs) {
      size_t nof_msgs = p_other->in_msgs->get_nof_types();
      for (size_t i = 0; i < nof_msgs; i++) {
        Type *t_msg = p_other->in_msgs->get_type_byIndex(i);
        if (!in_msgs || !in_msgs->has_type(t_msg)) {
          t_msg->note("Incoming message type `%s' of system port type `%s' "
            "is not present on the incoming list of test component port type "
            "`%s'", t_msg->get_typename().c_str(), other_typename_str,
            my_typename_str);
        }
      }
    }
    if (p_other->in_sigs) {
      size_t nof_sigs = p_other->in_sigs->get_nof_types();
      for (size_t i = 0; i < nof_sigs; i++) {
        Type *t_sig = p_other->in_sigs->get_type_byIndex(i);
        if (!in_sigs || !in_sigs->has_type(t_sig)) {
          t_sig->note("Incoming signature `%s' of system port type `%s' is "
            "not present on the incoming list of test component port type "
            "`%s'", t_sig->get_typename().c_str(), other_typename_str,
            my_typename_str);
        }
      }
    }
  }

  void PortTypeBody::generate_code(output_struct *target)
  {
    if (!checked || !my_type) FATAL_ERROR("PortTypeBody::generate_code()");
    if (map_params != NULL) {
      map_params->generate_code_defval(target);
    }
    if (unmap_params != NULL) {
      unmap_params->generate_code_defval(target);
    }
    stringpool pool;
    port_def pdef;
    memset(&pdef, 0, sizeof(pdef));
    pdef.legacy = legacy;
    pdef.realtime = realtime;
    const string& t_genname = my_type->get_genname_own();
    pdef.name = t_genname.c_str();
    pdef.dispname = my_type->get_fullname().c_str();
    pdef.filename = pool.add(Identifier::name_2_ttcn(t_genname));
    Scope *my_scope = my_type->get_my_scope();
    const Identifier& modid = my_scope->get_scope_mod_gen()->get_modid();
    pdef.module_name = modid.get_name().c_str();
    pdef.module_dispname = modid.get_ttcnname().c_str();
    if (testport_type == TP_ADDRESS) {
      Type *t_address = get_address_type();
      if (!t_address) FATAL_ERROR("PortTypeBody::generate_code()");
      pdef.address_name = pool.add(t_address->get_genname_value(my_scope));
    } else pdef.address_name = NULL;

    if (in_msgs) {
      pdef.msg_in.nElements = in_msgs->get_nof_types();
      pdef.msg_in.elements = (port_msg_type*)
        Malloc(pdef.msg_in.nElements * sizeof(*pdef.msg_in.elements));
      for (size_t i = 0; i < pdef.msg_in.nElements; i++) {
        Type *type = in_msgs->get_type_byIndex(i);
        pdef.msg_in.elements[i].name =
          pool.add(type->get_genname_value(my_scope));
        pdef.msg_in.elements[i].dispname = pool.add(type->get_typename());
        pdef.msg_in.elements[i].name_w_no_prefix = pool.add(type->get_genname_value(
          type->get_type_refd_last()->get_my_scope()));
      }
    } else {
      pdef.msg_in.nElements = 0;
      pdef.msg_in.elements = NULL;
    }
    if (out_msgs) {
      pdef.msg_out.nElements = out_msgs->get_nof_types();
      pdef.msg_out.elements = (port_msg_mapped_type*)
        Malloc(pdef.msg_out.nElements * sizeof(*pdef.msg_out.elements));
      for (size_t i = 0; i < pdef.msg_out.nElements; i++) {
        Type *type = out_msgs->get_type_byIndex(i);
        port_msg_mapped_type *mapped_type = pdef.msg_out.elements + i;
        mapped_type->name = pool.add(type->get_genname_value(my_scope));
        mapped_type->dispname = pool.add(type->get_typename());
        // When legacy behaviour is used and out_mappings is null then error later
        if (port_type == PT_USER && (legacy || out_mappings)) {
          if (!out_mappings) FATAL_ERROR("PortTypeBody::generate_code()");
          if (legacy || out_mappings->has_mapping_for_type(type)) {
            TypeMapping *mapping = out_mappings->get_mapping_byType(type);
            mapped_type->nTargets = mapping->get_nof_targets();
            mapped_type->targets = (port_msg_type_mapping_target*)
              Malloc(mapped_type->nTargets * sizeof(*mapped_type->targets));
            for (size_t j = 0; j < mapped_type->nTargets; j++) {
              mapping->get_target_byIndex(j)->fill_type_mapping_target(
                mapped_type->targets + j, type, my_scope, pool);
              mapped_type->targets[j].target_index = static_cast<size_t>(-1);
            }
          } else if (!legacy) {
            mapped_type->nTargets = 1;
            mapped_type->targets = (port_msg_type_mapping_target*)
                  Malloc(mapped_type->nTargets * sizeof(*mapped_type->targets));
            mapped_type->targets[0].target_name = pool.add(type->get_genname_value(my_scope));
            mapped_type->targets[0].target_dispname = pool.add(type->get_typename());
            mapped_type->targets[0].mapping_type = M_SIMPLE;
          }
        } else {
          mapped_type->nTargets = 0;
          mapped_type->targets = NULL;
        }
      }
    } else {
      pdef.msg_out.nElements = 0;
      pdef.msg_out.elements = NULL;
    }

    if (in_sigs) {
      pdef.proc_in.nElements = in_sigs->get_nof_types();
      pdef.proc_in.elements = (port_proc_signature*)
        Malloc(pdef.proc_in.nElements * sizeof(*pdef.proc_in.elements));
      for (size_t i = 0; i < pdef.proc_in.nElements; i++) {
        Type *signature = in_sigs->get_type_byIndex(i)->get_type_refd_last();
        pdef.proc_in.elements[i].name =
          pool.add(signature->get_genname_value(my_scope));
        pdef.proc_in.elements[i].dispname =
          pool.add(signature->get_typename());
        pdef.proc_in.elements[i].is_noblock =
          signature->is_nonblocking_signature();
        pdef.proc_in.elements[i].has_exceptions =
          signature->get_signature_exceptions() ? TRUE : FALSE;
        pdef.proc_in.elements[i].has_return_value = FALSE;
      }
    } else {
      pdef.proc_in.nElements = 0;
      pdef.proc_in.elements = NULL;
    }
    if (out_sigs) {
      pdef.proc_out.nElements = out_sigs->get_nof_types();
      pdef.proc_out.elements = (port_proc_signature*)
        Malloc(pdef.proc_out.nElements * sizeof(*pdef.proc_out.elements));
      for (size_t i = 0; i < pdef.proc_out.nElements; i++) {
        Type *signature = out_sigs->get_type_byIndex(i)->get_type_refd_last();
        pdef.proc_out.elements[i].name =
          pool.add(signature->get_genname_value(my_scope));
        pdef.proc_out.elements[i].dispname =
          pool.add(signature->get_typename());
        pdef.proc_out.elements[i].is_noblock =
          signature->is_nonblocking_signature();
        pdef.proc_out.elements[i].has_exceptions =
          signature->get_signature_exceptions() ? TRUE : FALSE;
        pdef.proc_out.elements[i].has_return_value =
          signature->get_signature_return_type() != NULL ? TRUE : FALSE;
      }
    } else {
      pdef.proc_out.nElements = 0;
      pdef.proc_out.elements = NULL;
    }

    switch (testport_type) {
    case TP_REGULAR:
      pdef.testport_type = NORMAL;
      break;
    case TP_INTERNAL:
      pdef.testport_type = INTERNAL;
      break;
    case TP_ADDRESS:
      pdef.testport_type = ADDRESS;
      break;
    default:
      FATAL_ERROR("PortTypeBody::generate_code()");
    }

    if (port_type == PT_USER) {
      pdef.port_type = USER;
      if (provider_types.size() == 0) FATAL_ERROR("PortTypeBody::generate_code()");
      if (legacy) {
        pdef.provider_msg_outlist.nElements = 1;
        pdef.provider_msg_outlist.elements = (port_msg_prov*)Malloc(sizeof(*pdef.provider_msg_outlist.elements));
        pdef.provider_msg_outlist.elements[0].name =
           pool.add(provider_types[0]->get_genname_value(my_scope));
        pdef.provider_msg_outlist.elements[0].n_out_msg_type_names = 0;
        pdef.provider_msg_outlist.elements[0].out_msg_type_names = NULL;
        PortTypeBody *provider_body = provider_types[0]->get_PortBody();
        pdef.provider_msg_outlist.elements[0].realtime = provider_body->realtime;
        if (provider_body->in_msgs) {
          if (!in_mappings) // !this->in_msgs OK for an all-discard mapping
            FATAL_ERROR("PortTypeBody::generate_code()");
          pdef.provider_msg_in.nElements =
            provider_body->in_msgs->get_nof_types();
          pdef.provider_msg_in.elements = (port_msg_mapped_type*)
            Malloc(pdef.provider_msg_in.nElements *
              sizeof(*pdef.provider_msg_in.elements));
          for (size_t i = 0; i < pdef.provider_msg_in.nElements; i++) {
            Type *type = provider_body->in_msgs->get_type_byIndex(i);
            port_msg_mapped_type *mapped_type = pdef.provider_msg_in.elements + i;
            mapped_type->name = pool.add(type->get_genname_value(my_scope));
            mapped_type->dispname = pool.add(type->get_typename());
            TypeMapping *mapping = in_mappings->get_mapping_byType(type);
            mapped_type->nTargets = mapping->get_nof_targets();
            mapped_type->targets = (port_msg_type_mapping_target*)
              Malloc(mapped_type->nTargets * sizeof(*mapped_type->targets));
            for (size_t j = 0; j < mapped_type->nTargets; j++) {
              TypeMappingTarget *t_target = mapping->get_target_byIndex(j);
              pdef.has_sliding |= t_target->fill_type_mapping_target(
                mapped_type->targets + j, type, my_scope, pool);
              Type *target_type = t_target->get_target_type();
              if (target_type) {
                if (!in_msgs) FATAL_ERROR("PortTypeBody::generate_code()");
                if (in_msgs->has_type(target_type)) {
                  mapped_type->targets[j].target_index =
                    in_msgs->get_index_byType(target_type);
                } else {
                  mapped_type->targets[j].target_index = static_cast<size_t>(-1);
                }
              } else {
                // the message will be discarded: fill in a dummy index
                mapped_type->targets[j].target_index = static_cast<size_t>(-1);
              }
            }
          }
        } else {
          pdef.provider_msg_in.nElements = 0;
          pdef.provider_msg_in.elements = NULL;
        }
      } else { // non-legacy standard like behaviour
        // Collect the out message lists of the provider ports along with the
        // name of the provider port type
        pdef.provider_msg_outlist.nElements = provider_types.size();
        pdef.provider_msg_outlist.elements = (port_msg_prov*)Malloc(provider_types.size() * sizeof(*pdef.provider_msg_outlist.elements));
        for (size_t i = 0; i < pdef.provider_msg_outlist.nElements; i++) {
          port_msg_prov * msg_prov = pdef.provider_msg_outlist.elements + i;
          msg_prov->name = pool.add(provider_types[i]->get_genname_value(my_scope));
          PortTypeBody * ptb = provider_types[i]->get_PortBody();
          msg_prov->realtime = ptb->realtime;
          if (ptb->out_msgs) {
            // Collect out message list type names
            msg_prov->n_out_msg_type_names = ptb->out_msgs->get_nof_types();
            msg_prov->out_msg_type_names = (const char**)Malloc(msg_prov->n_out_msg_type_names * sizeof(*msg_prov->out_msg_type_names));
            for (size_t j = 0; j < msg_prov->n_out_msg_type_names; j++) {
              msg_prov->out_msg_type_names[j] = pool.add(ptb->out_msgs->get_type_byIndex(j)->get_genname_value(my_scope));
            }
          } else {
            msg_prov->n_out_msg_type_names = 0;
            msg_prov->out_msg_type_names = NULL;
          }
        }
        pdef.provider_msg_in.nElements = 0;
        pdef.provider_msg_in.elements = NULL;
        if (in_msgs) {
          // First we insert the in messages with simple conversion (no conversion)
          // into a set called pdef.provider_msg_in.elements
          for (size_t i = 0; i < in_msgs->get_nof_types(); i++) {
            Type* type = in_msgs->get_type_byIndex(i);
            pdef.provider_msg_in.nElements++;
            pdef.provider_msg_in.elements = (port_msg_mapped_type*)Realloc(pdef.provider_msg_in.elements, pdef.provider_msg_in.nElements * sizeof(*pdef.provider_msg_in.elements));
            port_msg_mapped_type* mapped_type = pdef.provider_msg_in.elements + (pdef.provider_msg_in.nElements - 1);
            mapped_type->name = pool.add(type->get_genname_value(my_scope));
            mapped_type->dispname = pool.add(type->get_typename());
            mapped_type->nTargets = 1;
            mapped_type->targets = (port_msg_type_mapping_target*)
              Malloc(mapped_type->nTargets * sizeof(*mapped_type->targets));
            mapped_type->targets[0].target_name = pool.add(type->get_genname_value(my_scope));
            mapped_type->targets[0].target_dispname = pool.add(type->get_typename());
            mapped_type->targets[0].mapping_type = M_SIMPLE;
            mapped_type->targets[0].target_index =
              in_msgs->get_index_byType(type);
          } // for in_msgs->get_nof_types()
        } // if in_msgs
        
        if (in_mappings) {
          // Secondly we insert the mappings into the pdef.
          // We collect the mapping sources for each distinct mapping targets.
          // Kind of reverse what we did in the legacy behaviour.
          for (size_t j = 0; j < in_mappings->get_nof_mappings(); j++) {
            TypeMapping* mapping = in_mappings->get_mapping_byIndex(j);
            for (size_t u = 0; u < mapping->get_nof_targets(); u++) {
              TypeMappingTarget* mapping_target = mapping->get_target_byIndex(u);

              port_msg_mapped_type *mapped_type = NULL;
              // Do not insert the same mapped_type. The key is the dispname.
              for (size_t k = 0; k < pdef.provider_msg_in.nElements; k++) {
                if (pdef.provider_msg_in.elements[k].dispname == mapping_target->get_target_type()->get_typename()) {
                  mapped_type = pdef.provider_msg_in.elements + k;
                  break;
                }
              }
              
              // Mapping target not found. Create new port_msg_mapped_type
              if (mapped_type == NULL) {
                pdef.provider_msg_in.nElements++;
                pdef.provider_msg_in.elements = (port_msg_mapped_type*)Realloc(pdef.provider_msg_in.elements, pdef.provider_msg_in.nElements * sizeof(*pdef.provider_msg_in.elements));
                mapped_type = pdef.provider_msg_in.elements + (pdef.provider_msg_in.nElements - 1);
                mapped_type->name = pool.add(mapping_target->get_target_type()->get_genname_value(my_scope));
                mapped_type->dispname = pool.add(mapping_target->get_target_type()->get_typename());
                mapped_type->nTargets = 0;
                mapped_type->targets = NULL;
              }
              
              // Insert the mapping source as the mapped target's target.
              mapped_type->nTargets++;
              mapped_type->targets = (port_msg_type_mapping_target*)
                Realloc(mapped_type->targets, mapped_type->nTargets * sizeof(*mapped_type->targets));
              size_t ind = mapped_type->nTargets - 1;

              mapped_type->targets[ind].mapping_type = M_FUNCTION;
              mapped_type->targets[ind].mapping.function.prototype = PT_FAST;
              mapped_type->targets[ind].mapping.function.name =
                pool.add(mapping_target->get_function()->get_genname_from_scope(my_scope));
              mapped_type->targets[ind].mapping.function.dispname =
                pool.add(mapping_target->get_function()->get_fullname());

              mapped_type->targets[ind].target_name =
                pool.add(mapping->get_source_type()->get_genname_value(my_scope));
              mapped_type->targets[ind].target_dispname =
                pool.add(mapping->get_source_type()->get_typename());
              
              if (in_msgs->has_type(mapping->get_source_type())) {
                mapped_type->targets[ind].target_index =
                  in_msgs->get_index_byType(mapping->get_source_type());
              } else {
                FATAL_ERROR("PortTypeBody::generate_code()");
              }
            } // for mapping->get_nof_targets()
          } // for in_mappings->get_nof_mappings()
        } // if in_mappings // todo inout?
        
        // Variable declarations and definitions
        for (size_t i = 0; i < vardefs->get_nof_asss(); i++) {
          Definition* def = static_cast<Definition*>(vardefs->get_ass_byIndex(i));
          string type;
          switch (def->get_asstype()) {
            case Common::Assignment::A_VAR:
            case Common::Assignment::A_CONST:
              type = def->get_Type()->get_genname_value(my_scope);
              break;
            case Common::Assignment::A_VAR_TEMPLATE:
              type = def->get_Type()->get_genname_template(my_scope);
              break;
            default:
              FATAL_ERROR("PortTypeBody::generate_code()");
          }
          
          pdef.var_decls = 
            mputprintf(pdef.var_decls,
              "%s %s;\n",
              type.c_str(),
              def->get_genname().c_str());

          size_t len = pdef.var_defs ? strlen(pdef.var_defs) : 0;
          pdef.var_defs = def->generate_code_init_comp(pdef.var_defs, def);
          
          // If the def does not have default value then clean it up to
          // restore to unbound. (for constants the generate_code_init_comp does nothing)
          if ((pdef.var_defs == NULL || len == strlen(pdef.var_defs)) && def->get_asstype() != Common::Assignment::A_CONST) {
            pdef.var_defs = mputprintf(pdef.var_defs, "%s.clean_up();\n",
              def->get_genname().c_str());
          }
        }
        
        
        // Collect the generated code of functions with 'port' clause 
        // which belongs to this port type
        output_struct os;
        Code::init_output(&os);
        map<Def_Function_Base*, int> funs;
        if (out_mappings) {
          for (size_t i = 0; i < out_mappings->get_nof_mappings(); i++) {
            TypeMapping* tm = out_mappings->get_mapping_byIndex(i);
            for (size_t j = 0; j < tm->get_nof_targets(); j++) {
              TypeMappingTarget* tmt = tm->get_target_byIndex(j);
              if (tmt->get_mapping_type() == TypeMappingTarget::TM_FUNCTION) {
                Def_Function_Base* fun = tmt->get_function();
                Type * fun_port_type = fun->get_PortType();
                if (fun_port_type && fun_port_type == my_type && !funs.has_key(fun)) {
                  // Reuse of clean_up parameter here.
                  fun->generate_code(&os, true);
                  funs.add(fun, 0);
                }
              }
            }
          }
        }
        if (in_mappings) {
          for (size_t i = 0; i < in_mappings->get_nof_mappings(); i++) {
            TypeMapping* tm = in_mappings->get_mapping_byIndex(i);
            for (size_t j = 0; j < tm->get_nof_targets(); j++) {
              TypeMappingTarget* tmt = tm->get_target_byIndex(j);
              if (tmt->get_mapping_type() == TypeMappingTarget::TM_FUNCTION) {
                Def_Function_Base* fun = tmt->get_function();
                Type * fun_port_type = fun->get_PortType();
                if (fun_port_type && fun_port_type == my_type && !funs.has_key(fun)) {
                  // Reuse of clean_up parameter here.
                  fun->generate_code(&os, true);
                  funs.add(fun, 0);
                }
              }
            }
          }
        }
        pdef.mapping_func_decls = mcopystr(os.header.function_prototypes);
        pdef.mapping_func_defs = mcopystr(os.source.function_bodies);
        funs.clear();
        Code::free_output(&os);
      } // if legacy
    } else {
      // "internal provider" is the same as "internal"
      if (port_type == PT_PROVIDER && testport_type != TP_INTERNAL)
        pdef.port_type = PROVIDER;
      else pdef.port_type = REGULAR;
      pdef.provider_msg_outlist.nElements = 0;
      pdef.provider_msg_outlist.elements = NULL;
      pdef.provider_msg_in.nElements = 0;
      pdef.provider_msg_in.elements = NULL;
    }
    
    defPortClass(&pdef, target);
    if (generate_skeleton && testport_type != TP_INTERNAL &&
        (port_type != PT_USER || !legacy)) generateTestPortSkeleton(&pdef);

    Free(pdef.msg_in.elements);
    for (size_t i = 0; i < pdef.msg_out.nElements; i++)
      Free(pdef.msg_out.elements[i].targets);
    Free(pdef.msg_out.elements);
    Free(pdef.proc_in.elements);
    Free(pdef.proc_out.elements);
    for (size_t i = 0; i < pdef.provider_msg_in.nElements; i++)
      Free(pdef.provider_msg_in.elements[i].targets);
    Free(pdef.provider_msg_in.elements);
    for (size_t i = 0; i < pdef.provider_msg_outlist.nElements; i++)
      Free(pdef.provider_msg_outlist.elements[i].out_msg_type_names);
    Free(pdef.provider_msg_outlist.elements);
    Free(pdef.var_decls);
    Free(pdef.var_defs);
    Free(pdef.mapping_func_decls);
    Free(pdef.mapping_func_defs);
  }

  void PortTypeBody::dump(unsigned level) const
  {
    switch (operation_mode) {
    case PO_MESSAGE:
      DEBUG(level, "mode: message");
      break;
    case PO_PROCEDURE:
      DEBUG(level, "mode: procedure");
      break;
    case PO_MIXED:
      DEBUG(level, "mode: mixed");
      break;
    default:
      DEBUG(level, "mode: <invalid>");
    }
    if (in_list) {
      DEBUG(level, "in list:");
      in_list->dump(level + 1);
    }
    if (in_all) DEBUG(level, "in all");
    if (out_list) {
      DEBUG(level, "out list:");
      out_list->dump(level + 1);
    }
    if (out_all) DEBUG(level, "out all");
    if (inout_list) {
      DEBUG(level, "inout list:");
      inout_list->dump(level + 1);
    }
    if (inout_all) DEBUG(level, "inout all");
    if (in_msgs) {
      DEBUG(level, "incoming messages:");
      in_msgs->dump(level + 1);
    }
    if (out_msgs) {
      DEBUG(level, "outgoing messages:");
      out_msgs->dump(level + 1);
    }
    if (in_sigs) {
      DEBUG(level, "incoming signatures:");
      in_sigs->dump(level + 1);
    }
    if (out_sigs) {
      DEBUG(level, "outgoing signatures:");
      out_sigs->dump(level + 1);
    }
    switch (testport_type) {
    case TP_REGULAR:
      break;
    case TP_INTERNAL:
      DEBUG(level, "attribute: internal");
      break;
    case TP_ADDRESS:
      DEBUG(level, "attribute: address");
      break;
    default:
      DEBUG(level, "attribute: <unknown>");
    }
    switch (port_type) {
    case PT_REGULAR:
      break;
    case PT_PROVIDER:
      DEBUG(level, "attribute: provider");
      break;
    case PT_USER:
      DEBUG(level, "attribute: user");
      break;
    default:
      DEBUG(level, "attribute: <unknown>");
    }
    if (provider_refs.size() > 0)
      for (size_t i = 0; i < provider_refs.size(); i++) {
        DEBUG(level, "provider type %i: %s", (int)i, provider_refs[i]->get_dispname().c_str());
      }
    if (in_mappings) {
      DEBUG(level, "in mappings:");
      in_mappings->dump(level + 1);
    }
    if (out_mappings) {
      DEBUG(level, "out mappings:");
      out_mappings->dump(level + 1);
    }
    if (map_params != NULL) {
      DEBUG(level, "map parameters:");
      map_params->dump(level + 1);
    }
    if (unmap_params != NULL) {
      DEBUG(level, "unmap parameters:");
      unmap_params->dump(level + 1);
    }
  }

  // =================================
  // ===== ExtensionAttribute
  // =================================
  static const string version_template("RnXnn");

  void ExtensionAttribute::check_product_number(const char* ABCClass, int type_number, int sequence) {
    if (NULL == ABCClass && 0 == type_number && 0 == sequence) {
      return;
    }

    size_t ABCLength = strlen(ABCClass);
    if (ABCLength < 3 || ABCLength > 5) {
      // incorrect ABC number
      type_ = NONE;
      return;
    }

    if (type_number >= 1000) {
      type_ = NONE;
      return;
    }

    if (sequence >= 10000 ) {
      type_ = NONE;
    }
  }

  void ExtensionAttribute::parse_version(Identifier *ver) {
    unsigned int rel, bld = 99;
    char patch[4] = {0,0,0,0};
    // The limit of max three characters is an optimization: any more than two
    // is already an error and we can quit scanning. We can't limit the field
    // to two by using "%2[...] because then we can't distinguish between R1AA
    // and R1AAA. The switch below makes the distinction between one character
    // like R1A (valid), two characters like R1AA (also valid), and
    // R1AAA (invalid).
    // in addition after the RmXnn part some characters may come (needed by TitanSim)
    char extra_junk[64];
    int scanned = sscanf(ver->get_dispname().c_str(), "R%u%3[A-HJ-NS-VX-Z]%u%63s",
      &rel, patch, &bld, extra_junk);
    value_.version_.extra_ = NULL;
    switch (scanned * (patch[2]==0)) {
    case 4: // all fields + titansim's extra stuff scanned
      if (strlen(extra_junk)>0) {
        value_.version_.extra_ = mprintf("%s", extra_junk);
      }
    case 3: // all fields scanned
    case 2: // 2 scanned, bld remained unmodified
      value_.version_.release_ = rel;
      value_.version_.build_ = bld;
      switch (patch[0]) {
      case 'A': case 'B': case 'C': case 'D':
      case 'E': case 'F': case 'G': case 'H':
        value_.version_.patch_ = patch[0] - 'A';
        break;
      case 'J': case 'K': case 'L': case 'M': case 'N':
        value_.version_.patch_ = patch[0] - 'A' - 1;
        break;
      case 'S': case 'T': case 'U': case 'V':
        value_.version_.patch_ = patch[0] - 'A' - 5;
        break;
      case 'X': case 'Y': case 'Z':
        value_.version_.patch_ = patch[0] - 'A' - 6;
        break;

      case 'I': case 'W': // forbidden
      case 'O': case 'P': case 'Q': case 'R': // forbidden
      default: // incorrect
        type_ = NONE;
        break;
      }
      break;
    default: // there was an error
      type_ = NONE;
      break;
    }
  }

  ExtensionAttribute::ExtensionAttribute(const char* ABCClass, int type_number,
    int sequence, int suffix, Identifier *ver, enum version_t version_type)
  : Location(), type_(VERSION), value_()
  {
	if (version_type != DOT_SEPARATED && ver == NULL) FATAL_ERROR("ExtensionAttribute::ExtensionAttribute()");
    value_.version_.module_ = NULL;
    value_.version_.version_type_ = version_type;

    switch (version_type) {
	case DOT_SEPARATED:
	  // In this case, 'type_number' and 'sequence' are used to pass the minor and patch part of the version
	  //  major         minor/release      patch
	  if (suffix < 0 || type_number < 0 || sequence < 0) {
		type_ = NONE;
		return;
	  }
	  value_.version_.productNumber_ = mputc(value_.version_.productNumber_, '\0');
	  value_.version_.suffix_ = suffix;
	  value_.version_.release_ = type_number;
	  value_.version_.patch_ = sequence;
	  value_.version_.build_ = 99;
	  value_.version_.extra_ = NULL;
	  break;

	case UNKNOWN:
	case LEGACY_CRL:
	case LEGACY_CAX:
	  check_product_number(ABCClass, type_number, sequence);
	  parse_version(ver);
	  if (type_ != NONE) {
		value_.version_.productNumber_ =
		  NULL == ABCClass ? NULL : mprintf("%s %d %d", ABCClass, type_number, sequence);
		value_.version_.suffix_ = suffix;
		delete ver; // "took care of it"
	  }
	  break;

	default:
	  FATAL_ERROR("ExtensionAttribute::ExtensionAttribute()");
	  break;
	}
  }

  ExtensionAttribute::ExtensionAttribute(Identifier *mod, const char* ABCClass,
    int type_number, int sequence, int suffix, Identifier *ver, enum version_t version_type)
  : Location(), type_(REQUIRES), value_()
  {
    if (version_type != DOT_SEPARATED && (mod == NULL || ver == NULL))
      FATAL_ERROR("ExtensionAttribute::ExtensionAttribute()");
    // store the module identifier
    value_.version_.module_ = mod;
    value_.version_.version_type_ = version_type;

    switch (version_type) {
	case DOT_SEPARATED:
	  // In this case, 'type_number' and 'sequence' are used to pass the minor and patch part of the version
	  //  major         minor/release      patch
	  if (suffix < 0 || type_number < 0 || sequence < 0) {
		type_ = NONE;
		return;
	  }
	  value_.version_.productNumber_ = mputc(value_.version_.productNumber_, '\0');
	  value_.version_.suffix_ = suffix;
	  value_.version_.release_ = type_number;
	  value_.version_.patch_ = sequence;
	  value_.version_.build_ = 99;
	  value_.version_.extra_ = NULL;
	  break;

	case UNKNOWN:
	case LEGACY_CRL:
	case LEGACY_CAX:
	  check_product_number(ABCClass, type_number, sequence);
	  parse_version(ver);
	  if (type_ == NONE) { // parse_version reported an error
		value_.version_.module_ = NULL; // disown it; caller will free
		value_.version_.suffix_ = suffix;
	  } else {
		value_.version_.productNumber_ =
		  NULL == ABCClass ? NULL : mprintf("%s %d %d", ABCClass, type_number, sequence);
		value_.version_.suffix_ = suffix;
		delete ver;
	  }
	  break;

	default:
	  FATAL_ERROR("ExtensionAttribute::ExtensionAttribute()");
	  break;
	}
  }

  ExtensionAttribute::ExtensionAttribute(const char* ABCClass, int type_number,
    int sequence, int suffix, Identifier* ver, extension_t et, enum version_t version_type)
  : Location(), type_(et), value_()
  {
    if (version_type != DOT_SEPARATED && ver == NULL) FATAL_ERROR("ExtensionAttribute::ExtensionAttribute()");
    value_.version_.version_type_ = version_type;

    switch (et) {
    case REQ_TITAN:
      switch (version_type) {
      case DOT_SEPARATED:
        // In this case, 'type_number' and 'sequence' are used to pass the minor and patch part of the version
        //  major         minor/release      patch
        if (suffix < 0 || type_number < 0 || sequence < 0) {
    	  type_ = NONE;
    	  return;
    	}
    	value_.version_.productNumber_ = mputc(value_.version_.productNumber_, '\0');
    	value_.version_.suffix_ = suffix;
    	value_.version_.release_ = type_number;
    	value_.version_.patch_ = sequence;
    	value_.version_.build_ = 99;
    	value_.version_.extra_ = NULL;
    	break;

      case UNKNOWN:
      case LEGACY_CRL:
      case LEGACY_CAX:
    	check_product_number(ABCClass, type_number, sequence);
    	parse_version(ver);
    	if (type_ != NONE) {
    	  value_.version_.productNumber_ =
    		NULL == ABCClass ? NULL : mprintf("%s %d %d", ABCClass, type_number, sequence);
    	  value_.version_.suffix_ = suffix;
    	  delete ver; // "took care of it"
    	} else {
    	  value_.version_.suffix_ = suffix;
    	}
    	break;

      default:
    	FATAL_ERROR("ExtensionAttribute::ExtensionAttribute()");
    	break;
      }
      break;

    case VERSION_TEMPLATE: // RnXnn without the <>, must match exactly
      if (ver->get_name() == version_template) {
        value_.version_.productNumber_ = NULL;
        value_.version_.suffix_ = UINT_MAX;
        value_.version_.release_ = UINT_MAX;
        value_.version_.patch_   = UINT_MAX;
        value_.version_.build_   = UINT_MAX;
        delete ver;
        return;
      }
      else type_ = NONE;
      break;

    default:
      FATAL_ERROR("ExtensionAttribute::ExtensionAttribute()");
      break;
    }

  }


  ExtensionAttribute::~ExtensionAttribute()
  {
    switch (type_) {
    case NONE:
      break; // nothing to do, data has been extracted
      // TODO perhaps NONE should be the only acceptable option?
      // (no unprocessed attribute allowed)
    case PROTOTYPE:
      break;
    case ENCODE: case DECODE:
      delete value_.encdec_.s_;
      break;
    case PORT_API:
      break;
    case PORT_TYPE_USER:
      delete value_.user_.ref_;
      delete value_.user_.inmaps_;
      delete value_.user_.outmaps_;
      break;
    case PORT_TYPE_PROVIDER:
      break;
    case ERRORBEHAVIOR:
      delete value_.ebl_;
      break;
    case ANYTYPELIST:
      delete value_.anytypes_;
      break;
    case ENCDECVALUE:
      delete value_.encdecvalue_.inmaps_;
      delete value_.encdecvalue_.outmaps_;
      break;
    case VERSION:
    case VERSION_TEMPLATE:
      Free(value_.version_.productNumber_);
      Free(value_.version_.extra_);
      break;
    case REQUIRES:
      Free(value_.version_.productNumber_);
      Free(value_.version_.extra_);
      delete value_.version_.module_;
      break;
    case REQ_TITAN:
      Free(value_.version_.productNumber_);
      Free(value_.version_.extra_);
      break;
    case TRANSPARENT:
      break;
    case PRINTING:
      delete value_.pt_;
      break;
    default: // can't happen
      FATAL_ERROR("ExtensionAttribute::~ExtensionAttribute(%X)", type_);
    }
  }

  Types *ExtensionAttribute::get_types()
  {
    if (type_ != ANYTYPELIST) FATAL_ERROR("ExtensionAttribute::get_types()");
    type_ = NONE;
    return value_.anytypes_;
  }

  Ttcn::Def_Function_Base::prototype_t ExtensionAttribute::get_proto()
  {
    if (type_ != PROTOTYPE) FATAL_ERROR("ExtensionAttribute::get_proto()");
    type_ = NONE;
    return value_.proto_;
  }

  void ExtensionAttribute::get_encdec_parameters(
    Type::MessageEncodingType_t &p_encoding_type, string *&p_encoding_options)
  {
    if (type_ != ENCODE && type_ != DECODE)
      FATAL_ERROR("ExtensionAttribute::get_encdec_parameters()");
    type_ = NONE;
    p_encoding_type = value_.encdec_.mess_;
    p_encoding_options = value_.encdec_.s_;
    const_cast<ExtensionAttribute*>(this)->value_.encdec_.s_ = 0;
  }

  ErrorBehaviorList *ExtensionAttribute::get_eb_list()
  {
    if (type_ != ERRORBEHAVIOR)
      FATAL_ERROR("ExtensionAttribute::get_eb_list()");
    type_ = NONE;
    ErrorBehaviorList *retval = value_.ebl_;
    const_cast<ExtensionAttribute*>(this)->value_.ebl_ = 0;
    return retval;
  }
  
  PrintingType *ExtensionAttribute::get_printing()
  {
    if (type_ != PRINTING)
      FATAL_ERROR("ExtensionAttribute::get_printing()");
    type_ = NONE;
    PrintingType *retval =  value_.pt_;
    const_cast<ExtensionAttribute*>(this)->value_.pt_ = 0;
    return retval;
  }

  PortTypeBody::TestPortAPI_t ExtensionAttribute::get_api()
  {
    if (type_ != PORT_API)
      FATAL_ERROR("ExtensionAttribute::get_api()");
    type_ = NONE;
    return value_.api_;
  }

  void ExtensionAttribute::get_user(Reference *&ref,
    TypeMappings *&in, TypeMappings *&out)
  {
    if (type_ != PORT_TYPE_USER)
      FATAL_ERROR("ExtensionAttribute::get_user()");
    type_ = NONE;

    ref = value_.user_.ref_;
    in  = value_.user_.inmaps_;
    out = value_.user_.outmaps_;

    value_.user_.ref_     = 0;
    value_.user_.inmaps_  = 0;
    value_.user_.outmaps_ = 0;
  }

  void ExtensionAttribute::get_encdecvalue_mappings(
    TypeMappings *&in, TypeMappings *&out)
  {
    if (type_ != ENCDECVALUE)
      FATAL_ERROR("ExtensionAttribute::get_encdecvalue_mappings()");

    in = value_.encdecvalue_.inmaps_;
    out = value_.encdecvalue_.outmaps_;
  }

  //FIXME ot is update -elni kell.
  Common::Identifier *ExtensionAttribute::get_id(
      char*& product_number, unsigned int& suffix,
    unsigned int& rel, unsigned int& patch, unsigned int& bld, char*& extra, enum version_t& version_type)
  {
    if ( type_ != REQUIRES && type_ != REQ_TITAN
      && type_ != VERSION  && type_ != VERSION_TEMPLATE) {
      FATAL_ERROR("ExtensionAttribute::get_id()");
    }
    product_number = value_.version_.productNumber_;
    suffix = value_.version_.suffix_;
    rel   = value_.version_.release_;
    patch = value_.version_.patch_;
    bld   = value_.version_.build_;
    extra = value_.version_.extra_;
    version_type = value_.version_.version_type_;
    return  value_.version_.module_;
  }

  // =================================
  // ===== ExtensionAttributes
  // =================================
  void ExtensionAttributes::add(ExtensionAttribute *a)
  {
    vector<ExtensionAttribute>::add(a);
  }

  ExtensionAttributes::~ExtensionAttributes()
  {
    for (int i = size()-1; i >= 0; --i) {
      delete operator [](i);
    }

    clear();
  }

  void ExtensionAttributes::import(ExtensionAttributes *other)
  {
    size_t n = other->size();
    for (size_t i = 0; i < n; ++i) {
      vector<ExtensionAttribute>::add((*other)[i]);
    }
    other->clear();
  }
  
  // =================================
  // ===== ClassTypeBody
  // =================================
  
  ClassTypeBody::ClassTypeBody(Common::Identifier* p_class_id, boolean p_external, boolean p_final,
                               boolean p_abstract, boolean p_trait, Types* p_base_types,
                               Reference* p_runs_on_ref, Reference* p_mtc_ref, Reference* p_system_ref,
                               Definitions* p_members, StatementBlock* p_finally_block)
  : Scope(), Location(), class_id(p_class_id), my_def(NULL), external(p_external), final(p_final),
    abstract(p_abstract), trait(p_trait), built_in(FALSE), base_type(NULL), base_traits(p_base_types), base_class(NULL),
    runs_on_ref(p_runs_on_ref), runs_on_type(NULL), mtc_ref(p_mtc_ref),
    mtc_type(NULL), system_ref(p_system_ref), system_type(NULL),
    members(p_members), finally_block(p_finally_block), constructor(NULL), checked(false),
    default_constructor(false)
  {
    // constructor for user-defined classes
    if (members == NULL) {
      FATAL_ERROR("ClassTypeBody::ClassTypeBody");
    }
    if (finally_block != NULL) {
      finally_block->set_exception_handling(StatementBlock::EH_OOP_FINALLY);
    }
  }
  
  ClassTypeBody::ClassTypeBody()
  : Scope(), Location(), class_id(NULL), my_def(NULL), external(FALSE), final(FALSE),
    abstract(TRUE), trait(FALSE), built_in(TRUE), base_type(NULL), base_class(NULL),
    base_traits(NULL), runs_on_ref(NULL), runs_on_type(NULL), mtc_ref(NULL),
    mtc_type(NULL), system_ref(NULL), system_type(NULL),
    members(NULL), finally_block(NULL), constructor(NULL), checked(false),
    default_constructor(false)
  {
    // constructor for the built-in class 'object'
  }
  
  ClassTypeBody::ClassTypeBody(const ClassTypeBody& p)
  {
    built_in = p.built_in;
    class_id = p.class_id != NULL ? p.class_id->clone() : NULL;
    my_def = p.my_def;
    external = p.external;
    final = p.final;
    abstract = p.abstract;
    trait = p.trait;
    base_type = p.base_type != NULL ? p.base_type->clone() : NULL;
    base_class = p.base_class;
    base_traits = p.base_traits != NULL ? p.base_traits->clone() : NULL;
    runs_on_ref = p.runs_on_ref != NULL ? p.runs_on_ref->clone() : NULL;
    runs_on_type = p.runs_on_type;
    mtc_ref = p.mtc_ref != NULL ? p.mtc_ref->clone() : NULL;
    mtc_type = p.mtc_type;
    system_ref = p.system_ref != NULL ? p.system_ref->clone() : NULL;
    system_type = p.system_type;
    members = p.members != NULL ? p.members->clone() : NULL;
    finally_block = p.finally_block != NULL ? p.finally_block->clone() : NULL;
    default_constructor = p.default_constructor;
    constructor = default_constructor ? p.constructor->clone() : p.constructor;
    checked = p.checked;
  }
  
  ClassTypeBody* ClassTypeBody::clone() const
  {
    return new ClassTypeBody(*this);
  }
  
  ClassTypeBody::~ClassTypeBody()
  {
    if (built_in) {
      return;
    }
    delete base_type;
    delete base_traits;
    delete finally_block;
    delete members;
    delete mtc_ref;
    delete runs_on_ref;
    delete system_ref;
    if (default_constructor) {
      delete constructor;
    }
    abstract_functions.clear();
  }
  
  void ClassTypeBody::set_my_def(Def_Type* p_def)
  {
    my_def = p_def;
    if (finally_block != NULL) {
      finally_block->set_my_def(p_def);
    }
  }
  
  void ClassTypeBody::set_fullname(const string& p_fullname)
  {
    if (built_in) {
      return;
    }
    Common::Scope::set_fullname(p_fullname);
    if (base_type != NULL) {
      base_type->set_fullname(p_fullname + ".<superclass>");
    }
    if (base_traits != NULL) {
      base_traits->set_fullname(p_fullname + ".<supertraits>");
    }
    if (runs_on_ref != NULL) {
      runs_on_ref->set_fullname(p_fullname + ".<runs_on_type>");
    }
    if (mtc_ref != NULL) {
      mtc_ref->set_fullname(p_fullname + ".<mtc_type>");
    }
    if (system_ref != NULL) {
      system_ref->set_fullname(p_fullname + ".<system_type>");
    }
    members->set_fullname(p_fullname);
    if (finally_block != NULL) {
      finally_block->set_fullname(p_fullname + ".<finally_block>");
    }
  }
  
  void ClassTypeBody::set_my_scope(Scope* p_scope)
  {
    if (built_in) {
      return;
    }
    set_parent_scope(p_scope);
    if (base_type != NULL) {
      base_type->set_my_scope(p_scope);
    }
    if (base_traits != NULL) {
      base_traits->set_my_scope(p_scope);
    }
    if (runs_on_ref != NULL) {
      runs_on_ref->set_my_scope(p_scope);
    }
    if (mtc_ref != NULL) {
      mtc_ref->set_my_scope(p_scope);
    }
    if (system_ref != NULL) {
      system_ref->set_my_scope(p_scope);
    }
    members->set_parent_scope(this);
    if (finally_block != NULL) {
      finally_block->set_parent_scope(this);
    }
  }
  
  void ClassTypeBody::dump(unsigned level) const
  {
    if (built_in) {
      DEBUG(level, "Built-in class 'object'");
      return;
    }
    DEBUG(level, "Modifiers:%s%s%s", external ? "external " : "",
      final ? " @final " : "", abstract ? " @abstract" : "");
    DEBUG(level, "Base classes:");
    if (base_type != NULL) {
      DEBUG(level + 1, "%s", base_type->get_typename().c_str());
    }
    if (base_traits != NULL) {
      for (size_t i = 0; i < base_traits->get_nof_types(); ++i) {
        Type* base_trait = base_traits->get_type_byIndex(i);
        if (base_trait != NULL) {
          DEBUG(level + 1, "%s", base_trait->get_typename().c_str());
        }
      }
    }
    if (runs_on_ref != NULL) {
      DEBUG(level, "Runs on clause:");
      runs_on_ref->dump(level + 1);
    }
    if (mtc_ref != NULL) {
      DEBUG(level, "MTC clause:");
      mtc_ref->dump(level + 1);
    }
    if (system_ref != NULL) {
      DEBUG(level, "System clause:");
      system_ref->dump(level + 1);
    }
    DEBUG(level, "Members:");
    members->dump(level + 1);
    DEBUG(level, "%s `finally' block", finally_block != NULL ?
      "Has" : "Doesn't have");
  }
  
  Def_Constructor* ClassTypeBody::get_constructor()
  {
    if (!checked) {
      chk();
    }
    return constructor;
  }
  
  ClassTypeBody* ClassTypeBody::get_base_class()
  {
    if (!checked) {
      chk();
    }
    return base_class;
  }
  
  Type* ClassTypeBody::get_RunsOnType()
  {
    if (!checked) {
      chk();
    }
    return runs_on_type;
  }
  
  Type* ClassTypeBody::get_MtcType()
  {
    if (!checked) {
      chk();
    }
    return mtc_type;
  }
  
  Type* ClassTypeBody::get_SystemType()
  {
    if (!checked) {
      chk();
    }
    return system_type;
  }
  
  bool ClassTypeBody::is_parent_class(const ClassTypeBody* p_class)
  {
    if (!checked) {
      chk();
    }
    if (this == p_class || (!trait && p_class->built_in)) {
      return true;
    }
    if (base_class != NULL && base_class->is_parent_class(p_class)) {
      return true;
    }
    if (base_traits != NULL) {
      for (size_t i = 0; i < base_traits->get_nof_types(); ++i) {
        Type* base_trait = base_traits->get_type_byIndex(i);
        if (base_trait != NULL &&
            base_trait->get_type_refd_last()->get_class_type_body()->is_parent_class(p_class)) {
          return true;
        }
      }
    }
    return false;
  }
  
  bool ClassTypeBody::has_local_ass_withId(const Identifier& p_id)
  {
    if (!checked) {
      chk();
    }
    if (built_in) {
      return false;
    }
    if (members->has_local_ass_withId(p_id)) {
      return true;
    }
    if (base_class != NULL && base_class->has_local_ass_withId(p_id)) {
      return true;
    }
    if (base_traits) {
      for (size_t i = 0; i < base_traits->get_nof_types(); ++i) {
        Type* base_trait = base_traits->get_type_byIndex(i);
        if (base_trait != NULL &&
            base_trait->get_type_refd_last()->get_class_type_body()->has_local_ass_withId(p_id)) {
          return true;
        }
      }
    }
    return false;
  }
  
  Common::Assignment* ClassTypeBody::get_local_ass_byId(const Identifier& p_id)
  {
    if (built_in) {
      FATAL_ERROR("ClassTypeBody::get_local_ass_byId");
    }
    if (!checked) {
      chk();
    }
    Common::Assignment* ass = NULL;
    if (members->has_local_ass_withId(p_id)) {
      ass = members->get_local_ass_byId(p_id);
    }
    if (ass == NULL && base_class != NULL) {
      ass = base_class->get_local_ass_byId(p_id);
    }
    if (ass == NULL && base_traits != NULL) {
      for (size_t i = 0; i < base_traits->get_nof_types(); ++i) {
        Type* base_trait = base_traits->get_type_byIndex(i);
        if (base_trait != NULL) {
          ass = base_trait->get_type_refd_last()->get_class_type_body()->get_local_ass_byId(p_id);
          if (ass != NULL) {
            break;
          }
        }
      }
    }
    return ass;
  }
  
  bool ClassTypeBody::chk_visibility(Common::Assignment* ass,
                                     Common::Location* usage_loc,
                                     Common::Scope* usage_scope)
  {
    if (built_in) {
      FATAL_ERROR("ClassTypeBody::chk_visibility");
    }
    if (ass->get_visibility() == PUBLIC) {
      // it's public, so it doesn't matter where it's accessed from
      return true;
    }

    ClassTypeBody* ref_scope_class = usage_scope->get_scope_class();
    ClassTypeBody* ass_scope_class = ass->get_my_scope()->get_scope_class();
    if (ass_scope_class == NULL) {
      FATAL_ERROR("ClassTypeBody::chk_visibility()");
    }
    if (ref_scope_class == ass_scope_class) {
      // the reference is inside the same class as the assignment => any visibility is fine
      return true;
    }

    if (ref_scope_class != NULL &&
        ass->get_visibility() == NOCHANGE && // i.e. protected
        ref_scope_class->is_parent_class(ass_scope_class)) {
      return true;
    }

    usage_loc->error("The %s definition `%s' in class type `%s' is not visible "
      "in this scope", ass->get_FormalParList() != NULL ? "method" : "member",
      ass->get_id().get_dispname().c_str(), class_id->get_dispname().c_str());
    return false;
  }

  Common::Assignment* ClassTypeBody::get_ass_bySRef(Common::Ref_simple* p_ref)
  {
    if (built_in) {
      return NULL;
    }
    if (p_ref == NULL || parent_scope == NULL) {
      FATAL_ERROR("ClassTypeBody::get_ass_bySRef()");
    }
    chk();
    if (p_ref->get_modid() == NULL) {
      const Common::Identifier* id = p_ref->get_id();
      if (p_ref->get_reftype() == Ref_simple::REF_SUPER) {
        if (base_type == NULL) {
          p_ref->error("Reference to `super' in class type `%s', which has "
            "no base class", class_id->get_dispname().c_str());
          return NULL;
        }
        else {
          // send the reference to the base type, with the reftype changed to 'this'
          p_ref->set_reftype(Ref_simple::REF_THIS);
          Common::Assignment* ass = base_class->get_ass_bySRef(p_ref);
          p_ref->set_reftype(Ref_simple::REF_SUPER);
          return ass;
        }
      }
      else if (id == NULL && p_ref->get_reftype() == Ref_simple::REF_THIS) {
        // reference is just 'this'
        return my_def;
        // nothing special is needed for 'this.field' or 'this.method'
        // (it's already been handled at the lower scopes)
      }

      if (id != NULL && has_local_ass_withId(*id)) {
        Common::Assignment* ass = get_local_ass_byId(*id);
        if (ass == NULL) {
          FATAL_ERROR("ClassTypeBody::get_ass_bySRef()");
        }

        if (chk_visibility(ass, p_ref, p_ref->get_my_scope())) {
          return ass;
        }
        else {
          return NULL;
        }
      }
    }
    return parent_scope->get_ass_bySRef(p_ref);
  }
  
  bool ClassTypeBody::compare_members(ClassTypeBody* c1, ClassTypeBody* c2, Location* subclass_loc)
  {
    // if subclass_loc is NULL, then c1 is the subclass and c2 is the base class or a base trait
    // otherwise both c1 and c2 are inherited by the subclass
    if (subclass_loc != NULL && (c1->is_parent_class(c2) || c2->is_parent_class(c1))) {
      return false;
    }
    bool name_clash = false;
    for (size_t i = 0; i < c1->members->get_nof_asss(); ++i) {
      Common::Assignment* def1 = c1->members->get_ass_byIndex(i, false);
      if (def1->get_asstype() == Common::Assignment::A_CONSTRUCTOR) {
        continue;
      }
      const Common::Identifier& id1 = def1->get_id();
      if (c2->has_local_ass_withId(id1)) {
        Common::Assignment* def2 = c2->get_local_ass_byId(id1);
        ClassTypeBody* def2_class = def2->get_my_scope()->get_scope_class();
        if (subclass_loc != NULL &&
            def2_class != c2 && def2_class != c1 && c1->is_parent_class(def2_class)) {
          // def2 is defined in a trait class that is inherited by both c1 and c2,
          // ignore this comparison
          continue;
        }
        switch (def1->get_asstype()) {
        case Common::Assignment::A_FUNCTION:
        case Common::Assignment::A_FUNCTION_RVAL:
        case Common::Assignment::A_FUNCTION_RTEMP:
        case Common::Assignment::A_EXT_FUNCTION:
        case Common::Assignment::A_EXT_FUNCTION_RVAL:
        case Common::Assignment::A_EXT_FUNCTION_RTEMP:
          switch (def2->get_asstype()) {
          case Common::Assignment::A_FUNCTION:
          case Common::Assignment::A_FUNCTION_RVAL:
          case Common::Assignment::A_FUNCTION_RTEMP:
          case Common::Assignment::A_EXT_FUNCTION:
          case Common::Assignment::A_EXT_FUNCTION_RVAL:
          case Common::Assignment::A_EXT_FUNCTION_RTEMP: {
            Def_Function_Base* func1 = dynamic_cast<Def_Function_Base*>(def1);
            Def_Function_Base* func2 = dynamic_cast<Def_Function_Base*>(def2);
            bool func1_is_abstract = dynamic_cast<Def_AbsFunction*>(func1) != NULL;
            bool func2_is_abstract = dynamic_cast<Def_AbsFunction*>(func2) != NULL;
            bool functions_are_identical = func1->is_identical(func2);
            if (func2->get_visibility() != PRIVATE &&
                (!functions_are_identical || func1_is_abstract != func2_is_abstract)) {
              if (subclass_loc == NULL && !functions_are_identical) {
                def1->error("The prototype of method `%s' is not identical "
                  "to that of inherited method `%s'",
                  id1.get_dispname().c_str(), def2->get_fullname().c_str());
              }
              else if (subclass_loc != NULL && func1->get_visibility() != PRIVATE) {
                subclass_loc->error("The prototypes of methods `%s' inherited from "
                  "classes `%s' and `%s' are not identical",
                  id1.get_dispname().c_str(), c1->get_id()->get_dispname().c_str(),
                  c2->get_id()->get_dispname().c_str());
              }
            }
            else if (subclass_loc == NULL && func2->is_final()) {
              def1->error("Cannot override final method `%s'",
                def2->get_fullname().c_str());
            }
            else if (subclass_loc == NULL && func1->is_identical(func2)) {
              if (func2->get_visibility() == PUBLIC && func1->get_visibility() != PUBLIC) {
                def1->error("Public methods can be only overridden by public methods `%s'",
                  id1.get_dispname().c_str());
              }
              else if (func2->get_visibility() == NOCHANGE &&
                       func1->get_visibility() != PUBLIC && func1->get_visibility() != NOCHANGE) {
                def1->error("Protected methods can be only overridden by "
                  "public or protected methods `%s'", id1.get_dispname().c_str());
              }
            }
            break; }
          default:
            def1->error("%s shadows inherited member `%s'",
              def1->get_description().c_str(), def2->get_fullname().c_str());
            name_clash = true;
            break;
          }
          break;
        default:
          def1->error("%s shadows inherited %s `%s'",
            def1->get_description().c_str(),
            dynamic_cast<Def_Function_Base*>(def2) != NULL ? "method" : "member",
            def2->get_fullname().c_str());
          name_clash = true;
          break;
        }
      }
    }
    if (subclass_loc != NULL) {
      // the code above only goes through the local members of c1,
      // comparing them to local or inherited members of c2;
      // the members in the superclass and supertraits of c1 must also be compared to c2
      if (c1->base_class != NULL) {
        name_clash |= compare_members(c1->base_class, c2, subclass_loc);
      }
      if (c1->base_traits != NULL) {
        for (size_t i = 0; i < c1->base_traits->get_nof_types(); ++i) {
          Type* base_trait = c1->base_traits->get_type_byIndex(i);
          if (base_trait != NULL) {
            ClassTypeBody* base_trait_class = base_trait->get_type_refd_last()->get_class_type_body();
            name_clash |= compare_members(base_trait_class, c2, subclass_loc);
          }
        }
      }
    }
    return name_clash;
  }
  
  void ClassTypeBody::chk()
  {
    if (checked || built_in) {
      return;
    }
    checked = true;
    if ((final && abstract) || (final && trait) || (abstract && trait)) {
      error("A class cannot have more than one of the @final, @abstract and @trait modifiers");
    }
    if (external && abstract) { // todo
      error("External classes cannot be abstract");
    }
    if (base_traits != NULL) {
      for (size_t i = 0; i < base_traits->get_nof_types(); ++i) {
        Type* t = base_traits->get_type_byIndex(i);
        Error_Context cntxt(t, "In superclass or supertrait definition");
        t->chk();
        if (t->get_type_refd_last()->get_typetype() != Common::Type::T_CLASS) {
          if (t->get_typetype() != Common::Type::T_ERROR) {
            t->error("Class type expected instead of `%s'", t->get_typename().c_str());
          }
          base_traits->extract_type_byIndex(i);
          delete t;
        }
        else {
          ClassTypeBody* t_class = t->get_type_refd_last()->get_class_type_body();
          if (!t_class->trait) {
            if (trait) {
              t->error("A trait class cannot extend a non-trait class");
            }
            else if (base_type != NULL) {
              t->error("A class cannot extend more than one non-trait class");
              base_type->note("Previous extended non-trait class is here");
            }
            else {
              base_traits->extract_type_byIndex(i);
              base_type = t;
              base_class = t_class;
              if (base_class->final) {
                base_type->error("The superclass cannot be final");
              }
              if (external && !base_class->external) {
                base_type->error("An external class cannot extend an internal class");
              }
            }
          }
          for (size_t j = 0; j < i; ++j) {
            Type* t2 = base_traits->get_type_byIndex(j);
            if (t2 != NULL && t2->get_typename() == t->get_typename()) {
              t->error("Duplicate class type in list of classes being extended");
              t2->note("Class type `%s' is already given here", t->get_typename().c_str());
            }
          }
        }
      }
    }
    if (base_class != NULL && base_class->built_in) {
      // if the base class is 'object', then just delete it and set it to NULL,
      // so it functions the same way as not specifying a base class
      delete base_type;
      base_type = NULL;
      base_class = NULL;
    }
    
    if (runs_on_ref != NULL) {
      Error_Context cntxt(runs_on_ref, "In `runs on' clause");
      runs_on_type = runs_on_ref->chk_comptype_ref();
      if (base_class != NULL) {
        Type* base_runs_on_type = base_class->get_RunsOnType();
        if (base_runs_on_type != NULL &&
            !base_runs_on_type->is_compatible(runs_on_type, NULL, NULL)) {
          runs_on_ref->error("The `runs on' component type of the subclass, "
            "`%s', is not compatible with the `runs on' component type of the "
            "superclass, `%s'",
            runs_on_type->get_typename().c_str(),
            base_runs_on_type->get_typename().c_str());
        }
      }
    }
    else if (base_class != NULL) {
      // inherit `runs on' component from the superclass
      runs_on_type = base_class->get_RunsOnType();
    }
    
    if (mtc_ref != NULL) {
      Error_Context cntxt(mtc_ref, "In `mtc' clause");
      mtc_type = mtc_ref->chk_comptype_ref();
      if (base_class != NULL) {
        Type* base_mtc_type = base_class->get_MtcType();
        if (base_mtc_type != NULL &&
            !base_mtc_type->is_compatible(mtc_type, NULL, NULL)) {
          mtc_ref->error("The `mtc' component type of the subclass, `%s', is not "
            "compatible with the `mtc' component type of the superclass, `%s'",
            mtc_type->get_typename().c_str(),
            base_mtc_type->get_typename().c_str());
        }
      }
    }
    else if (base_class != NULL) {
      // inherit `mtc' component from the superclass
      mtc_type = base_class->get_MtcType();
    }
    
    if (system_ref != NULL) {
      Error_Context cntxt(system_ref, "In `system' clause");
      system_type = system_ref->chk_comptype_ref();
      if (base_class != NULL) {
        Type* base_system_type = base_class->get_SystemType();
        if (base_system_type != NULL &&
            !base_system_type->is_compatible(system_type, NULL, NULL)) {
          system_ref->error("The `system' component type of the subclass, `%s', is "
            "not compatible with the `system' component type of the superclass, `%s'",
            system_type->get_typename().c_str(),
            base_system_type->get_typename().c_str());
        }
      }
    }
    else if (base_class != NULL) {
      // inherit `system' component from the superclass
      system_type = base_class->get_SystemType();
    }
    
    if (base_traits != NULL) {
      for (size_t i = 0; i < base_traits->get_nof_types(); ++i) {
        Type* base_trait = base_traits->get_type_byIndex(i);
        if (base_trait != NULL) {
          ClassTypeBody* base_trait_class = base_trait->get_type_refd_last()->
            get_class_type_body();
          Type* base_runs_on_type = base_trait_class->get_RunsOnType();
          if (base_runs_on_type != NULL) {
            if (runs_on_type == NULL) {
              error("Supertrait `%s' has a `runs on` component type, "
                "but the subclass and its superclass does not",
                base_trait_class->class_id->get_dispname().c_str());
            }
            else if(!base_runs_on_type->is_compatible(runs_on_type, NULL, NULL)) {
              runs_on_ref->error("The `runs on' component type of the subclass "
                "(`%s') is not compatible with the `runs on' component type of "
                "supertrait `%s' (`%s')",
                runs_on_type->get_typename().c_str(),
                base_trait_class->class_id->get_dispname().c_str(),
                base_runs_on_type->get_typename().c_str());
            }
          }
          Type* base_mtc_type = base_trait_class->get_MtcType();
          if (base_mtc_type != NULL) {
            if (mtc_type == NULL) {
              error("Supertrait `%s' has an `mtc` component type, "
                "but the subclass and its superclass does not",
                base_trait_class->class_id->get_dispname().c_str());
            }
            else if(!base_mtc_type->is_compatible(mtc_type, NULL, NULL)) {
              mtc_ref->error("The `mtc' component type of the subclass "
                "(`%s') is not compatible with the `mtc' component type of "
                "supertrait `%s' (`%s')",
                mtc_type->get_typename().c_str(),
                base_trait_class->class_id->get_dispname().c_str(),
                base_mtc_type->get_typename().c_str());
            }
          }
          Type* base_system_type = base_trait_class->get_SystemType();
          if (base_system_type != NULL) {
            if (system_type == NULL) {
              error("Supertrait `%s' has an `system` component type, "
                "but the subclass and its superclass does not",
                base_trait_class->class_id->get_dispname().c_str());
            }
            else if(!base_system_type->is_compatible(system_type, NULL, NULL)) {
              system_ref->error("The `system' component type of the subclass "
                "(`%s') is not compatible with the `system' component type of "
                "supertrait `%s' (`%s')",
                system_type->get_typename().c_str(),
                base_trait_class->class_id->get_dispname().c_str(),
                base_system_type->get_typename().c_str());
            }
          }
        }
      }
    }

    for (size_t i = 0; i < members->get_nof_asss(); ++i) {
      Common::Assignment* ass = members->get_ass_byIndex(i, false);
      if (ass->get_asstype() == Common::Assignment::A_CONSTRUCTOR) {
        constructor = dynamic_cast<Def_Constructor*>(ass);
        if (constructor == NULL) {
          FATAL_ERROR("ClassTypeBody::chk");
        }
        break;
      }
    }

    members->chk_uniq();
    members->chk();
    
    for (size_t i = 0; i < members->get_nof_asss(); ++i) {
      Common::Assignment* ass = members->get_ass_byIndex(i, false);
      switch (ass->get_asstype()) {
      case Common::Assignment::A_CONST:
      case Common::Assignment::A_VAR:
      case Common::Assignment::A_TEMPLATE:
      case Common::Assignment::A_VAR_TEMPLATE:
        if (ass->get_visibility() == PUBLIC) {
          ass->error("Class members cannot be public");
        }
        break;
      default:
        break;
      }
    }

    if (runs_on_type != NULL || mtc_type != NULL || system_type != NULL) {
      for (size_t i = 0; i < members->get_nof_asss(); ++i) {
        Common::Assignment* local_def = members->get_ass_byIndex(i, false);
        const Common::Identifier& local_id = local_def->get_id();
        if (runs_on_type != NULL && runs_on_type->get_CompBody()->has_local_ass_withId(local_id)) {
          local_def->error("%s shadows a definition in runs-on component type `%s'",
            local_def->get_description().c_str(), runs_on_type->get_typename().c_str());
        }
        if (mtc_type != NULL && mtc_type->get_CompBody()->has_local_ass_withId(local_id)) {
          local_def->error("%s shadows a definition in mtc component type `%s'",
            local_def->get_description().c_str(), mtc_type->get_typename().c_str());
        }
        if (system_type != NULL && system_type->get_CompBody()->has_local_ass_withId(local_id)) {
          local_def->error("%s shadows a definition in system component type `%s'",
            local_def->get_description().c_str(), system_type->get_typename().c_str());
        }
      }
    }
    
    bool name_clash = false;
    if (base_class != NULL) {
      name_clash = compare_members(this, base_class);
    }
    if (base_traits != NULL) {
      for (size_t i = 0; i < base_traits->get_nof_types(); ++i) {
        Type* base_trait = base_traits->get_type_byIndex(i);
        if (base_trait != NULL) {
          ClassTypeBody* base_trait_class = base_trait->get_type_refd_last()->
            get_class_type_body();
          name_clash |= compare_members(this, base_trait_class);
          if (base_class != NULL) {
            name_clash |= compare_members(base_class, base_trait_class, this);
          }
          for (size_t j = 0; j < i; ++j) {
            Type* base_trait2 = base_traits->get_type_byIndex(j);
            if (base_trait2 != NULL) {
              ClassTypeBody* base_trait_class2 = base_trait2->get_type_refd_last()->
                get_class_type_body();
              name_clash |= compare_members(base_trait_class, base_trait_class2, this);
            }
          }
        }
      }
    }

    if (constructor != NULL && trait) {
      constructor->error("Trait class type `%s' cannot have a constructor",
        my_def->get_Type()->get_typename().c_str());
    }
    if (constructor == NULL && !name_clash && !trait) {
      // create a default constructor
      Reference* base_call = NULL;
      FormalParList* fp_list = NULL;
      if (!external && base_class != NULL) {
        Def_Constructor* base_constructor = base_class->get_constructor();
        if (base_constructor != NULL) {
          FormalParList* base_fp_list = base_constructor->get_FormalParList();
          fp_list = base_fp_list->clone();
          ParsedActualParameters* parsed_ap_list = new ParsedActualParameters();
          for (size_t i = 0; i < base_fp_list->get_nof_fps(); ++i) {
            // the actual parameters are references to the formal parameters of
            // the base constructor (also present in this constructor)
            Reference* ref = new Reference(NULL,
              base_fp_list->get_fp_byIndex(i)->get_id().clone());
            Common::Value* val = new Value(Common::Value::V_REFD, ref);
            Template* temp = new Template(val);
            TemplateInstance* ti = new TemplateInstance(NULL, NULL, temp);
            parsed_ap_list->add_ti(ti);
          }
          base_call = new Reference(base_class->get_scope_mod()->get_modid().clone(),
            base_class->get_id()->clone(), parsed_ap_list);
        }
      }
      if (fp_list == NULL) {
        fp_list = new FormalParList;
      }
      StatementBlock* block = NULL;
      if (!external) {
        block = new StatementBlock();
        for (size_t i = 0; i < members->get_nof_asss(); ++i) {
          // note: the Definitions class rearranges its elements alphabetically;
          // here the members must be accessed in their original order
          Common::Assignment* member = members->get_ass_byIndex(i, false);
          bool is_template = false;
          TemplateInstance* def_val = NULL;
          switch (member->get_asstype()) {
          case Common::Assignment::A_CONST:
            if (member->get_Value() != NULL) {
              continue; // the constant has already been initialized at its definition
            }
            break;
          case Common::Assignment::A_TEMPLATE:
            if (member->get_Template() != NULL) {
              continue; // the template has already been initialized at its definition
            }
            is_template = true;
            break;
          case Common::Assignment::A_VAR:
            if (member->get_Value() != NULL) {
              // set the variable's initial value as the constructor parameter's default value
              Def_Var* var_member = dynamic_cast<Def_Var*>(member);
              if (var_member == NULL) {
                FATAL_ERROR("ClassTypeBody::chk - Def_Var cast");
              }
              def_val = new TemplateInstance(NULL, NULL, new Template(var_member->steal_Value()));
            }
            break;
          case Common::Assignment::A_VAR_TEMPLATE:
            is_template = true;
            if (member->get_Template() != NULL) {
              // set the template variable's initial value as the constructor parameter's default value
              Def_Var_Template* var_temp_member = dynamic_cast<Def_Var_Template*>(member);
              if (var_temp_member == NULL) {
                FATAL_ERROR("ClassTypeBody::chk - Def_Var_Template cast");
              }
              def_val = new TemplateInstance(NULL, NULL, var_temp_member->steal_Template());
            }
            break;
          default:
            continue;
            break;
          }
          // add a formal parameter for this member if we've gotten this far
          Common::Identifier* id = member->get_id().clone();
          FormalPar* fp = new FormalPar(is_template ?
            Common::Assignment::A_PAR_TEMPL_IN : Common::Assignment::A_PAR_VAL_IN,
            member->get_Type()->clone(), id, def_val);
          fp_list->add_fp(fp);
          // add a statement, that assigns the parameter's value to the member
          Reference* ref_lhs = new Reference(NULL, id->clone(), Ref_simple::REF_THIS);
          Reference* ref_rhs = new Reference(NULL, id->clone());
          Common::Value* val_rhs = new Value(Common::Value::V_REFD, ref_rhs);
          Template* temp_rhs = new Template(val_rhs);
          Assignment* par_ass = new Assignment(ref_lhs, temp_rhs);
          Statement* stmt = new Statement(Statement::S_ASSIGNMENT, par_ass);
          block->add_stmt(stmt);
        }
      }
      constructor = new Def_Constructor(fp_list, base_call, block);
      constructor->set_my_scope(this);
      constructor->set_fullname(get_fullname() + ".<default_constructor>");
      constructor->chk();
      default_constructor = true;
    }

    if (constructor != NULL && !default_constructor && !name_clash && !trait) {
      // make sure constants and templates are initialized
      for (size_t i = 0; i < members->get_nof_asss(); ++i) {
        Common::Assignment* member = members->get_ass_byIndex(i, false);
        bool needs_init_check = false;
        bool is_template = false;
        switch (member->get_asstype()) {
        case Common::Assignment::A_CONST:
          needs_init_check = member->get_Value() == NULL;
          break;
        case Common::Assignment::A_TEMPLATE:
          needs_init_check = member->get_Template() == NULL;
          is_template = true;
          break;
        default:
          break;
        }
        if (needs_init_check) {
          constructor->add_uninit_member(&member->get_id(), is_template);
        }
      }
    }

    if (finally_block != NULL) {
      Error_Context cntxt(finally_block, "In class destructor");
      finally_block->chk();
      if (trait) {
        finally_block->error("Trait class type `%s' cannot have a destructor",
          my_def->get_Type()->get_typename().c_str());
      }
    }
    
    if (external) {
      for (size_t i = 0; i < members->get_nof_asss(); ++i) {
        Common::Assignment* ass = members->get_ass_byIndex(i, false);
        switch (ass->get_asstype()) {
        case Common::Assignment::A_EXT_FUNCTION:
        case Common::Assignment::A_EXT_FUNCTION_RVAL:
        case Common::Assignment::A_EXT_FUNCTION_RTEMP:
        case Common::Assignment::A_CONSTRUCTOR:
          // OK
          break;
        default:
          ass->error("An external class cannot contain a %s", ass->get_assname());
          break;
        }
      }
      if (finally_block != NULL) {
        finally_block->error("An external class cannot have a destructor");
      }
    }
    
    if (abstract || trait) {
      // create a map of all abstract functions (including inherited ones)
      if (base_class != NULL && base_class->abstract) {
        for (size_t i = 0; i < base_class->abstract_functions.size(); ++i) {
          abstract_functions.add(base_class->abstract_functions.get_nth_key(i),
            base_class->abstract_functions.get_nth_elem(i));
        }
      }
      if (base_traits != NULL) {
        for (size_t i = 0; i < base_traits->get_nof_types(); ++i) {
          Type* base_trait = base_traits->get_type_byIndex(i);
          if (base_trait != NULL) {
            ClassTypeBody* base_trait_class = base_trait->get_type_refd_last()->
              get_class_type_body();
            for (size_t j = 0; j < base_trait_class->abstract_functions.size(); ++j) {
              const string& key = base_trait_class->abstract_functions.get_nth_key(j);
              if (!abstract_functions.has_key(key)) {
                abstract_functions.add(key, base_trait_class->abstract_functions.get_nth_elem(j));
              }
            }
          }
        }
      }
      for (size_t i = 0; i < members->get_nof_asss(); ++i) {
        Common::Assignment* ass = members->get_ass_byIndex(i, false);
        switch (ass->get_asstype()) {
        case Common::Assignment::A_FUNCTION:
        case Common::Assignment::A_FUNCTION_RVAL:
        case Common::Assignment::A_FUNCTION_RTEMP: {
          Def_AbsFunction* def_abs_func = dynamic_cast<Def_AbsFunction*>(ass);
          if (def_abs_func != NULL) {
            const string& def_name = def_abs_func->get_id().get_name();
            if (!abstract_functions.has_key(def_name)) {
              abstract_functions.add(def_name, def_abs_func);
            }
          }
          break; }
        default:
          break;
        }
      }
    }
    
    if (!abstract && !trait) {
      // all abstract methods from the base class and base traits have to be implemented in this class
      if (base_class != NULL && base_class->abstract) {
        for (size_t i = 0; i < base_class->abstract_functions.size(); ++i) {
          Def_AbsFunction* def_abs_func = base_class->abstract_functions.get_nth_elem(i);
          Common::Assignment* ass = get_local_ass_byId(def_abs_func->get_id());
          switch (ass->get_asstype()) {
          case Common::Assignment::A_FUNCTION:
          case Common::Assignment::A_FUNCTION_RVAL:
          case Common::Assignment::A_FUNCTION_RTEMP: {
            if (dynamic_cast<Def_AbsFunction*>(ass) != NULL) {
              error("Missing implementation of abstract method `%s'",
                def_abs_func->get_fullname().c_str());
            }
            // whether the new function is identical to the abstract one has
            // already been checked
            break; }
          default:
            // it's either an external function (which is OK), or
            // it's shadowed by a member (error has already been reported)
            break;
          }
        }
      }
      if (base_traits != NULL) {
        for (size_t k = 0; k < base_traits->get_nof_types(); ++k) {
          Type* base_trait = base_traits->get_type_byIndex(k);
          if (base_trait != NULL) {
            ClassTypeBody* base_trait_class = base_trait->get_type_refd_last()->
              get_class_type_body();
            for (size_t i = 0; i < base_trait_class->abstract_functions.size(); ++i) {
              Def_AbsFunction* def_abs_func = base_trait_class->abstract_functions.get_nth_elem(i);
              Common::Assignment* ass = get_local_ass_byId(def_abs_func->get_id());
              switch (ass->get_asstype()) {
              case Common::Assignment::A_FUNCTION:
              case Common::Assignment::A_FUNCTION_RVAL:
              case Common::Assignment::A_FUNCTION_RTEMP: {
                if (dynamic_cast<Def_AbsFunction*>(ass) != NULL) {
                  error("Missing implementation of abstract method `%s'",
                    def_abs_func->get_fullname().c_str());
                }
                // whether the new function is identical to the abstract one has
                // already been checked
                break; }
              default:
                // it's either an external function (which is OK), or
                // it's shadowed by a member (error has already been reported)
                break;
              }
            }
          }
        }
      }
    }
  }
  
  void ClassTypeBody::chk_recursions(ReferenceChain& refch)
  {
    if (built_in) {
      return;
    }
    if (base_type != NULL) {
      refch.mark_state();
      base_type->chk_recursions(refch);
      refch.prev_state();
    }
    if (base_traits != NULL) {
      for (size_t i = 0; i < base_traits->get_nof_types(); ++i) {
        Type* base_trait = base_traits->get_type_byIndex(i);
        if (base_trait != NULL) {
          refch.mark_state();
          base_trait->chk_recursions(refch);
          refch.prev_state();
        }
      }
    }
    
    for (size_t i = 0; i < members->get_nof_asss(); ++i) {
      Common::Assignment* def = members->get_ass_byIndex(i, false);
      switch (def->get_asstype()) {
      case Common::Assignment::A_VAR:
        if (def->get_Type()->get_type_refd_last()->get_typetype() == Common::Type::T_CLASS) {
          break;
        }
        // else fall through
      case Common::Assignment::A_CONST:
      case Common::Assignment::A_TEMPLATE:
      case Common::Assignment::A_VAR_TEMPLATE:
        refch.mark_state();
        def->get_Type()->chk_recursions(refch);
        refch.prev_state();
        break;
      default:
        break;
      }
    }
  }
  
  void ClassTypeBody::generate_code(output_struct* target)
  {
    if (built_in) {
      return;
    }
    target->header.class_decls = mputprintf(target->header.class_decls,
      "class %s;\n", class_id->get_name().c_str());
    if (!external || generate_skeleton) {
      string base_type_name = base_type != NULL ?
        base_type->get_type_refd_last()->get_genname_own(this) :
        trait ? string("CLASS_BASE") : string("OBJECT");
      output_struct* local_struct;
      if (external) {
        local_struct = new output_struct;
        Code::init_output(local_struct, TRUE);
      }
      else {
        local_struct = target;
      }
      
      local_struct->header.class_defs = mputprintf(local_struct->header.class_defs,
        "class %s : ",
        class_id->get_name().c_str());
      bool has_base = false;
      if (base_traits != NULL) {
        for (size_t i = 0; i < base_traits->get_nof_types(); ++i) {
          Type* base_trait = base_traits->get_type_byIndex(i);
          if (base_trait != NULL) {
            local_struct->header.class_defs = mputprintf(local_struct->header.class_defs,
              "%spublic %s", has_base ? ", " : "",
              base_trait->get_type_refd_last()->get_genname_own(this).c_str());
            has_base = true;
          }
        }
      }
      if (!trait || !has_base) {
        local_struct->header.class_defs = mputprintf(local_struct->header.class_defs,
          "%s public %s", has_base ? ", " : "", base_type_name.c_str());
        has_base |= base_type != NULL;
      }
      local_struct->header.class_defs = mputstr(local_struct->header.class_defs,
        " {\n");
      
      // class name
      local_struct->header.class_defs = mputprintf(local_struct->header.class_defs,
        "public:\n"
        "static const char* class_name() { return \"%s\"; }\n",
        class_id->get_dispname().c_str());
      
      // members
      members->generate_code(local_struct);
      if (default_constructor) {
        // code has not been generated for the constructor yet, since it's not
        // in 'members' in this case
        constructor->generate_code(local_struct);
      }
      
      // constructor
      if (constructor != NULL) {
        char* formal_par_list_str = NULL;
        expression_struct_t base_call_expr;
        Code::init_expr(&base_call_expr);
        Reference* base_call = constructor->get_base_call();
        if (base_call != NULL) {
          base_call->generate_code(&base_call_expr);
        }
        // generate code for the base call first, so the formal parameter list
        // knows which parameters are used and which aren't
        formal_par_list_str = constructor->get_FormalParList()->generate_code(
          memptystr(), external ? constructor->get_FormalParList()->get_nof_fps() : 0);
        if (base_call_expr.expr == NULL) {
          base_call_expr.expr = mprintf("%s()", base_type_name.c_str());
        }
        local_struct->header.class_defs = mputprintf(local_struct->header.class_defs,
          "\npublic:\n%s"
          "%s(%s);\n\n",
          default_constructor ? "/* default constructor */\n" : "",
          class_id->get_name().c_str(),
          formal_par_list_str != NULL ? formal_par_list_str : "");
        local_struct->source.methods = mputprintf(local_struct->source.methods,
          "%s::%s(%s)\n"
          ": %s",
          class_id->get_name().c_str(), class_id->get_name().c_str(),
          formal_par_list_str != NULL ? formal_par_list_str : "",
          base_call_expr.expr);
        Free(formal_par_list_str);
        Code::free_expr(&base_call_expr);
        if (local_struct->temp.constructor_init != NULL) {
          local_struct->source.methods = mputstr(local_struct->source.methods,
            local_struct->temp.constructor_init);
          Free(local_struct->temp.constructor_init);
          local_struct->temp.constructor_init = NULL;
        }
        local_struct->source.methods = mputstr(local_struct->source.methods, "\n{\n");
        if (local_struct->temp.constructor_preamble != NULL ||
            local_struct->temp.constructor_block != NULL) {
          local_struct->source.methods = create_location_object(
            local_struct->source.methods, "FUNCTION", class_id->get_name().c_str());
          local_struct->source.methods = mputstr(local_struct->source.methods,
            local_struct->temp.constructor_preamble);
          local_struct->source.methods = mputstr(local_struct->source.methods,
            local_struct->temp.constructor_block);
          Free(local_struct->temp.constructor_preamble);
          Free(local_struct->temp.constructor_block);
          local_struct->temp.constructor_preamble = NULL;
          local_struct->temp.constructor_block = NULL;
        }
        else if (external) {
          local_struct->source.methods = mputc(local_struct->source.methods, '\n');
        }
        local_struct->source.methods = mputstr(local_struct->source.methods, "}\n\n");
      }

      // destructor
      bool desctructor_body = finally_block != NULL || external;
      local_struct->header.class_defs = mputprintf(local_struct->header.class_defs,
        "public:\n"
        "virtual ~%s()%s\n\n",
        class_id->get_name().c_str(), desctructor_body ? ";" : " { }");
      if (desctructor_body) {
        local_struct->source.methods = mputprintf(local_struct->source.methods,
          "%s::~%s()\n"
          "{\n", class_id->get_name().c_str(), class_id->get_name().c_str());
        char* destructor_str = mprintf("~%s", class_id->get_name().c_str());
        if (finally_block != NULL) {
          local_struct->source.methods = finally_block->create_location_object(
            local_struct->source.methods, "FUNCTION", destructor_str);
        }
        Free(destructor_str);
        local_struct->source.methods = mputstr(local_struct->source.methods, "try {\n");
        if (finally_block != NULL) {
          local_struct->source.methods = finally_block->generate_code(
            local_struct->source.methods, local_struct->header.global_vars,
            local_struct->source.global_vars);
        }
        else {
          local_struct->source.methods = mputc(local_struct->source.methods, '\n');
        }
        local_struct->source.methods = mputprintf(local_struct->source.methods,
          "} catch (...) {\n"
          "fprintf(stderr, \"Unhandled exception or dynamic test case error in "
          "the destructor of class `%s'. Terminating application.\\n\");\n"
          "exit(EXIT_FAILURE);\n"
          "}\n"
          "}\n\n", class_id->get_name().c_str());
      }
      
      // logging function (similar to logging a record value, but with the 
      // class name and the base class' log at the beginning)
      // members are no longer logged, as this could cause infinite loops
      local_struct->header.class_defs = mputstr(local_struct->header.class_defs,
        "public:\n"
        "virtual void log() const;\n");
      local_struct->source.methods = mputprintf(local_struct->source.methods,
        "void %s::log() const\n"
        "{\n"
        "TTCN_Logger::log_event_str(\"%s%s\");\n",
        class_id->get_name().c_str(), class_id->get_dispname().c_str(), has_base ? " ( " : "");
      if (base_type != NULL) {
        local_struct->source.methods = mputprintf(local_struct->source.methods,
          "%s::log();\n", base_type_name.c_str());
      }
      if (base_traits != NULL) {
        for (size_t i = 0; i < base_traits->get_nof_types(); ++i) {
          Type* base_trait = base_traits->get_type_byIndex(i);
          if (base_trait != NULL) {
            if (i > 0 || base_type != NULL) {
              local_struct->source.methods = mputstr(local_struct->source.methods,
                "TTCN_Logger::log_event_str(\", \");\n");
            }
            local_struct->source.methods = mputprintf(local_struct->source.methods,
              "%s::log();\n", base_trait->get_type_refd_last()->get_genname_own(this).c_str());
          }
        }
      }
      if (has_base) {
        local_struct->source.methods = mputstr(local_struct->source.methods,
          "TTCN_Logger::log_event_str(\" )\");\n");
      }
      local_struct->source.methods = mputstr(local_struct->source.methods,
        "}\n\n");
      
      
      local_struct->header.class_defs = mputstr(local_struct->header.class_defs, "};\n\n");

      if (external) {
        generate_class_skeleton(local_struct);
        Code::free_output(local_struct);
        delete local_struct;
      }
    }
    if (external) {
      target->header.class_defs = mputprintf(target->header.class_defs,
        "#include \"%s.hh\"\n\n",
        duplicate_underscores ? class_id->get_name().c_str() :
          class_id->get_dispname().c_str());
    }
  }

#undef COMMENT_PREFIX
#define COMMENT_PREFIX "// "

  void ClassTypeBody::generate_class_skeleton(output_struct* target)
  {
    string file_prefix = duplicate_underscores ? class_id->get_name() :
      class_id->get_dispname();

    DEBUG(1, "Generating external class skeleton for class type `%s' ...",
      class_id->get_dispname().c_str());

    string header_name;
    string source_name;
    string base_header_include;
    if (output_dir != NULL) {
      header_name = string(output_dir) + string("/");
      source_name = string(output_dir) + string("/");
    }
    header_name += file_prefix + string(".hh");
    source_name += file_prefix + string(".cc");

    if (base_class != NULL) {
      base_header_include = string("#include \"");
      base_header_include += duplicate_underscores ? base_class->class_id->get_name() :
          base_class->class_id->get_dispname();
      base_header_include += string(".hh\"\n\n");
    }

    char* user_info = NULL;
    if (disable_user_info == FALSE) {
      user_info = get_user_info();
    } else {
      user_info = mputstr(user_info, "The generation of user and time information were disabled by the -D flag.\n");
    }

    if (force_overwrite || get_path_status(header_name.c_str()) == PS_NONEXISTENT) {
      FILE *fp = fopen(header_name.c_str(), "w");
      if (fp == NULL) {
        ERROR("Cannot open output external class skeleton header file `%s' for "
          "writing: %s", header_name.c_str(), strerror(errno));
        exit(EXIT_FAILURE);
      }
      fprintf(fp,
        "// This external class skeleton header file was generated by the\n"
        "// TTCN-3 Compiler of the TTCN-3 Test Executor version "
        PRODUCT_NUMBER "\n"
        "// %s%s\n"
        COPYRIGHT_STRING "\n\n"
        "// You may modify this file. Add your attributes and prototypes of your\n"
        "// member functions here.\n\n"
        "#ifndef %s_HH\n"
        "#define %s_HH\n\n"
        "%s%s"
        "#endif\n",
        disable_user_info == FALSE ? "for " : "", user_info,
        class_id->get_name().c_str(), class_id->get_name().c_str(),
        base_header_include.c_str(), target->header.class_defs);
      fclose(fp);
      NOTIFY("External class skeleton header file `%s' was generated for class "
        "type `%s'.", header_name.c_str(), class_id->get_dispname().c_str());
    }
    else {
      DEBUG(1, "External class header file `%s' already exists. It was not "
        "overwritten.", header_name.c_str());
    }

    if (force_overwrite || get_path_status(source_name.c_str()) == PS_NONEXISTENT) {
      FILE *fp = fopen(source_name.c_str(), "w");
      if (fp == NULL) {
        ERROR("Cannot open output external class skeleton source file `%s' for "
          "writing: %s", source_name.c_str(), strerror(errno));
        exit(EXIT_FAILURE);
      }
      fprintf(fp,
        "// This external class skeleton source file was generated by the\n"
        "// TTCN-3 Compiler of the TTCN-3 Test Executor version "
        PRODUCT_NUMBER "\n"
        "// %s%s\n"
        COPYRIGHT_STRING "\n\n"
        "// You may modify this file. Complete the bodies of empty functions and\n"
        "// add your member functions here.\n\n"
        "#include <TTCN3.hh>\n\n"
        "namespace %s {\n\n"
        "#include \"%s.hh\"\n\n"
        "%s\n"
        "} /* end of namespace */\n\n",
        disable_user_info == FALSE ? "for " : "", user_info,
        get_scope_mod_gen()->get_modid().get_name().c_str(), class_id->get_dispname().c_str(),
        target->source.methods);
      fclose(fp);
      NOTIFY("External class skeleton source file `%s' was generated for class "
        "type `%s'.", source_name.c_str(), file_prefix.c_str());
    }
    else {
      DEBUG(1, "External class source file `%s' already exists. It was not "
        "overwritten.", source_name.c_str());
    }

    Free(user_info);
  }

} // namespace Ttcn
