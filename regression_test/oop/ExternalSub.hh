// This external class skeleton header file was generated by the
// TTCN-3 Compiler of the TTCN-3 Test Executor version 8.2.0
// for ebotbar (ebotbar@ebotbar-VirtualBox) on Wed Jun  8 16:31:03 2022

// Copyright (c) 2000-2021 Ericsson Telecom AB

// This file is not re-generated after a clean build, since the generated version does not compile
// (the external parameter needed to be added to the base constructor call manually)

#ifndef ExternalSub_HH
#define ExternalSub_HH

class ExternalSub :  public ExternalBase {
public:
static const char* class_name() { return "ExternalSub"; }

private:
const INTEGER& c1;
INTEGER const_c1;

private:
const FLOAT& c2;
FLOAT const_c2;

// manually added code for testing:
CHARSTRING ret_val;

public:
virtual CHARSTRING test__result();

public:
/* default constructor */
ExternalSub(const INTEGER& par1, const CHARSTRING& par2, const INTEGER& c1, const FLOAT& c2, const OCTETSTRING& extpar);

public:
virtual ~ExternalSub();

public:
virtual void log() const;
};

#endif