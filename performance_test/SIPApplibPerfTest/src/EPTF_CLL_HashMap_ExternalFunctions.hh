/******************************************************************************
 * Copyright (c) 2000-2021 Ericsson Telecom AB
 * All rights reserved. This program and the accompanying materials
 * are made available under the terms of the Eclipse Public License v2.0
 * which accompanies this distribution, and is available at
 * https://www.eclipse.org/org/documents/epl-2.0/EPL-2.0.html
 ******************************************************************************/
//  File:     EPTF_CLL_HashMap_ExternalFunctions.hh
//  Rev:      <RnXnn>
//  Prodnr:   CNL 113 512
//  Updated:  2011-11-17
//  Contact:  http://ttcn.ericsson.se
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////
//  File: EPTF_CLL_HashMap_ExternalFunctions.hh
//
//  Purpose:
//    Provide implementation of template c++ functions of HashMaps.
//    These functions are used by the int2int, str2int, oct2int
//    variations of the HashMap.
//
///////////////////////////////////////////////////////////////
#ifndef EPTF_CLL_HashMap_ExternalFunctions_hh
#define EPTF_CLL_HashMap_ExternalFunctions_hh

#ifdef EPTF_DEBUG
#define HASHMAP_INT2INT_DEBUG_MEMLEAK // enable memleak debug
#define HASHMAP_STR2INT_DEBUG_MEMLEAK
#define HASHMAP_OCT2INT_DEBUG_MEMLEAK
#endif

#include <signal.h>

#include <iostream>
#include <string>
#include <stdio.h>

#ifdef EPTF_DEBUG
#include "EPTF_CLL_Common_Definitions.hh"
#endif

#include "EPTF_CLL_HashMap_Functions.hh"
/*
//#if defined(SOLARIS8) || defined(SOLARIS)
#if (__GNUC__ < 4 )
#if(__GNUC_MINOR__ < 1)
using std::hash_map;
using std::hash;
#else
using __gnu_cxx::hash_map;
using __gnu_cxx::hash;
#endif
#else
#if(__GNUC_MINOR__ < 3)
using __gnu_cxx::hash_map;
using __gnu_cxx::hash;
#else
//using std::tr1;
#endif
#endif
*/

#if (__GNUC__ < 4)//earlier than gcc 4.0
 #include <ext/hash_map>
 #if (__GNUC__ == 3 && __GNUC_MINOR__ >=1) //gcc [3.1 , 4.0[
  using __gnu_cxx::hash_map;
  using __gnu_cxx::hash;
  #define hash_ns __gnu_cxx::hash_map 
 #else //earlier than gcc 3.1
  using std::hash_map;
  using std::hash; 
  #define hash_ns std::hash_map
 #endif
#else //from gcc 4.0
 #if (__GNUC__ == 4 && __GNUC_MINOR__ < 3 ) //gcc [4.0 , 4.3[
  #include <ext/hash_map>
  using __gnu_cxx::hash_map;
  using __gnu_cxx::hash; 
  #define hash_ns __gnu_cxx::hash_map
 #else //from gcc 4.3
  #include <tr1/unordered_map>
  using std::tr1::unordered_map;  
  #define hash_ns std::tr1::unordered_map
 #endif
#endif

namespace EPTF__CLL__HashMap__Functions {

struct EPTF_HashMap_hash
{
size_t
      operator()(const CHARSTRING& cs) const
  {
    unsigned long __h = 0;
    const char* __s=cs;
    for ( ; *__s; ++__s)
      __h = 5 * __h + *__s;
    return size_t(__h);
  }
};

///////////////////////////////////////////////////////////
// Function: EPTF_HashMap_EqDef
//
// Purpose:
//   The hashmap key equality function. Compares two keys.
//
// Parameters:
//   str1 - *in* *integer* - key1
//   str2 - *in* *integer* - key2
//
// Return Value:
//   boolean - equal
//////////////////////////////////////////////////////////

template<class C>  class EPTF_HashMap_EqDef
{
public:
  bool operator()(const C& c1, const C& c2) const {
    if(c1 == c2) return true;
    return false;
  }
};

typedef hash_ns<CHARSTRING, int, EPTF_HashMap_hash, EPTF_HashMap_EqDef<CHARSTRING> > NamesHashMapType;

///////////////////////////////////////////////////////////
// Type: HashMapType
//
// Purpose:
//   Type of hashmap. int -> int
//
// Elements:
//   key - key of hashmap
//   data - data of hashmap
//   hashfcn - hashfunction of hashmap, built in, can be changed
//   equalkey -  equality function of hashmap, EPTF_HashMap_Eqint
//
///////////////////////////////////////////////////////////
template<class T, class H, class F>  class HashMapType : public
hash_ns<T, INTEGER, H, F>
{
  int allowedMaxSize;
  std::string name;
public:
  HashMapType() : hash_ns<T, INTEGER, H, F>(), allowedMaxSize(-1), name("") { }
  ~HashMapType() { name=""; }
  void initDebug(const char *p_name)
  {
    allowedMaxSize = (int)EPTF__CLL__Common__Definitions::tsp__CLL__debug__acceptableMaxSizeOfGrowingVariables;
    name=p_name;
  }
  inline void checkAllowedMaxSize()
  {
#if defined(EPTF_DEBUG) | defined(HASHMAP_INT2INT_DEBUG_MEMLEAK)
    if(allowedMaxSize >= 0) {
      if(this->size() > (unsigned)allowedMaxSize) {
        TTCN_Logger::log(TTCN_WARNING, "hashmap `%s' exceeds acceptable max size of %d elements.", name.c_str(), allowedMaxSize);
        TTCN_Logger::log(TTCN_WARNING, "increasing acceptable max size by a factor of %g",
          (double)EPTF__CLL__Common__Definitions::tsp__CLL__debug__increasePercentage4AcceptableMaxSize);
        allowedMaxSize += (int) ((double)allowedMaxSize * (double)EPTF__CLL__Common__Definitions::tsp__CLL__debug__increasePercentage4AcceptableMaxSize);
        TTCN_Logger::log(TTCN_WARNING, "new acceptable max size: %d", allowedMaxSize);
      }
    }
#endif
  }
  const char* getName() const { return name.c_str(); }
  const HashMapType<T,H,F>& operator=(const HashMapType<T,H,F>& other)
  {
    allowedMaxSize = other.allowedMaxSize;
    name=other.name;
    hash_ns<T, INTEGER, H, F>::operator=(other);
    return *this;
  }
};

///////////////////////////////////////////////////////////
// Function: ASSERT_VALID_ID
//
// Purpose:
//   Assert macro, for checking whether the HashMap ID is correct.
//
// Parameters:
//   pl_id - *in* *integer* - the ID of HashMap
//
// Return Value:
//   -
//////////////////////////////////////////////////////////
//FIXME
#define ASSERT_VALID_ID(pl_id) \
\
  if(pl_id<0){TTCN_error("HashMap Error: pl_id<0");};\
  if(pl_id>=v_sizeOfHashMaps){TTCN_error("HashMap Error: pl_id>=v_sizeOfHashMaps");};\
  if(v_HashMap[pl_id]==NULL){TTCN_error("HashMap Error: v_HashMap[pl_id]==NULL");};\


//typedef HashMapType<CHARSTRING, int, EPTF_HashMap_strHash > Str2IntHashMapType;

template<class T, class H, class F>  class HashMapTypeCT
{
private:
	typedef HashMapType<T,H,F>* HashMapTypePtr;
	typedef typename HashMapType<T,H,F>::iterator HashMapTypeIter;
	typedef typename HashMapType<T,H,F>::hasher HashMapTypeHasher;
	  int v_sizeOfHashMaps;
	  int v_numberOfHashMaps;
	  NamesHashMapType v_HashNames;
	  HashMapTypePtr* v_HashMap;
public:
	HashMapTypeCT(){
		  v_sizeOfHashMaps = 0;
		  v_numberOfHashMaps = 0;
		  v_HashMap = NULL;
	}
	~HashMapTypeCT(){
	  NamesHashMapType::iterator Hashmap_Iter;
	  Hashmap_Iter = v_HashNames.begin();
	  while(Hashmap_Iter != v_HashNames.end()) {
	    try {
	      TTCN_Logger::begin_event(TTCN_WARNING);
	      TTCN_Logger::log_event_str("f_EPTF_int2int_HashMap_Cleanup: deleting hashmap ");
	      TTCN_Logger::log_event_str(Hashmap_Iter->first);
	      TTCN_Logger::end_event();
	    }
	    catch (...)
	    {
	      TTCN_Logger::finish_event();
	      throw;
	    }
	    v_HashMap[Hashmap_Iter->second]->clear();
	    if(v_HashMap[Hashmap_Iter->second]!=NULL)
	    {
	      delete v_HashMap[Hashmap_Iter->second];
	      v_HashMap[Hashmap_Iter->second] = NULL;
	    }
	    v_HashNames.erase(Hashmap_Iter);
	    v_numberOfHashMaps--;
	    Hashmap_Iter = v_HashNames.begin();
	  }
	  v_HashMap = NULL;
	  v_sizeOfHashMaps = 0;
	  v_numberOfHashMaps = 0;
	}

	inline void assertValid(const INTEGER& pl_id){
	  if(pl_id<0){TTCN_error("HashMap Error: pl_id<0");};
	  if(pl_id>=v_sizeOfHashMaps){TTCN_error("HashMap Error: pl_id>=v_sizeOfHashMaps");};
	  if(v_HashMap[pl_id]==NULL){TTCN_error("HashMap Error: v_HashMap[pl_id]==NULL");};
	}
	inline bool checkValid(const INTEGER& pl_id){
	  if(pl_id<0){return false;};
	  if(pl_id>=v_sizeOfHashMaps){return false;};
	  if(v_HashMap[pl_id]==NULL){return false;};
          return true;
	}
    INTEGER newHashmap (const CHARSTRING& pl_name)
	{
    	NamesHashMapType::iterator Hashmap_Iter = v_HashNames.find(pl_name);

	  if ( Hashmap_Iter!=v_HashNames.end() )
	  {
	    try {
	      TTCN_Logger::begin_event(TTCN_WARNING);
	      TTCN_Logger::log_event_str("Can't create new HashMap. HashMap name is not unique: ");
	      pl_name.log();
	      TTCN_Logger::end_event();
	    }
	    catch (...)
	    {
	      TTCN_Logger::finish_event();
	      throw;
	    }
	    return -1;
	  }
	  v_numberOfHashMaps++;
	  if(v_numberOfHashMaps<v_sizeOfHashMaps)
	  {
	    for(int i=0; i<v_sizeOfHashMaps; i++) {
	      if (v_HashMap[i]== NULL)
	      {
	        v_HashMap[i]=new HashMapType<T,H,F>;
	        v_HashMap[i]->initDebug(pl_name);
	        v_HashNames[pl_name] = i;
	        return i;
	      }
	    }
	  }
	  else
	  {
	    v_HashNames[pl_name] = v_sizeOfHashMaps;

	    // create a new array for hashMaps with new size (size+1)
	    HashMapTypePtr* v_HashMap_new;
	    v_HashMap_new = new HashMapTypePtr[v_sizeOfHashMaps+1];
	    for(int i=0; i<v_sizeOfHashMaps; i++) {
	      v_HashMap_new[i] = v_HashMap[i];
	    }
	    v_HashMap_new[v_sizeOfHashMaps] = new HashMapType<T,H,F>;
	    v_HashMap_new[v_sizeOfHashMaps]->initDebug(pl_name);
	    if (v_HashMap!=NULL) delete[] v_HashMap;
	    v_HashMap = v_HashMap_new;

	    return v_sizeOfHashMaps++;
	  }
	  //never reaches
	  return -1;
	}

	BOOLEAN getID(
	  const CHARSTRING& pl_name,
	  INTEGER& pl_id)
	{

		NamesHashMapType::iterator Hashmap_Iter;
	  Hashmap_Iter = v_HashNames.find(pl_name);


	  if ( Hashmap_Iter==v_HashNames.end() )
	  {
	    try {
	      TTCN_Logger::begin_event(TTCN_WARNING);
	      TTCN_Logger::log_event_str("Can't get ID. No HashMap found with this name: ");
	      pl_name.log();
	      TTCN_Logger::end_event();
	    }
	    catch (...)
	    {
	      TTCN_Logger::finish_event();
	      throw;
	    }
	    pl_id = -1;
	    return false;
	  }

	  pl_id = Hashmap_Iter->second;
	  return true;
	}

	void deleteHashMap(
	  const CHARSTRING& pl_name)
	{
		NamesHashMapType::iterator Hashmap_Iter;
	  Hashmap_Iter = v_HashNames.find(pl_name);
	  if ( Hashmap_Iter==v_HashNames.end() )
	  {
	    try {
	      TTCN_Logger::begin_event(TTCN_WARNING);
	      TTCN_Logger::log_event_str("Can't get ID. HashMap not deleted. No HashMap found with this name: ");
	      pl_name.log();
	      TTCN_Logger::end_event();
	    }
	    catch (...)
	    {
	      TTCN_Logger::finish_event();
	      throw;
	    }

	    return;
	  }
	  if(v_HashMap[Hashmap_Iter->second]!=NULL)
	  {
            v_HashMap[Hashmap_Iter->second]->clear();
	    delete v_HashMap[Hashmap_Iter->second];
	    v_HashMap[Hashmap_Iter->second] = NULL;
	  }
	  v_HashNames.erase(Hashmap_Iter);
	  v_numberOfHashMaps--;
	}

	void deleteHashMap(
	  const INTEGER& pl_id)
	{
          if (!checkValid(pl_id)) {
	    try {
	      TTCN_Logger::begin_event(TTCN_WARNING);
	      TTCN_Logger::log_event_str("Can't delete hasmap: No HashMap found with this id: ");
	      pl_id.log();
	      TTCN_Logger::end_event();
	    }
	    catch (...)
	    {
	      TTCN_Logger::finish_event();
	      throw;
	    }

	    return;
          }
          v_HashNames.erase(v_HashMap[pl_id]->getName());
	  if(v_HashMap[pl_id]!=NULL)
	  {
            v_HashMap[pl_id]->clear();
	    delete v_HashMap[pl_id];
	    v_HashMap[pl_id] = NULL;
	  }
	  v_numberOfHashMaps--;
	}

	void dumpByID (const INTEGER& pl_id)
	{
		assertValid(pl_id);
	  try {
	    TTCN_Logger::begin_event(TTCN_USER);
	    TTCN_Logger::log_event_str("----Dumping HashMap with ID: ");
	    pl_id.log();
	    TTCN_Logger::log_event_str(" begin----");
	  }
	  catch (...)
	  {
	    TTCN_Logger::finish_event();
	    throw;
	  }

	  HashMapTypeIter Hashmap_Iter;

	  for ( Hashmap_Iter = v_HashMap[pl_id]->begin();
	        Hashmap_Iter != v_HashMap[pl_id]->end(); Hashmap_Iter++ )
	  {
	    //std::cout << Hashmap_Iter->first<<"  "<< Hashmap_Iter->second <<std::endl;
		  /*
	    INTEGER key = (INTEGER)Hashmap_Iter->first;
	    T data = (T)Hashmap_Iter->second;
	    try {
	      TTCN_Logger::log_event_str("\n (key,data): ");
	      key.log();
	      TTCN_Logger::log_char(' ');
	      data.log();
	    }
	    catch (...) {
	      TTCN_Logger::finish_event();
	      throw;
	    }
	    */
	  }

	  //std::cout <<"----Dumping HashMap with id: "<<pl_id<<" end----"<<std::endl;
	  try {
	    TTCN_Logger::log_event_str("\n----Dumping HashMap with ID: ");
	    pl_id.log();
	    TTCN_Logger::log_event_str(" end----");
	    TTCN_Logger::end_event();
	  }
	  catch (...)
	  {
	    TTCN_Logger::finish_event();
	    throw;
	  }

	}

	void dump(const CHARSTRING& pl_name){
	  try {
	    TTCN_Logger::begin_event(TTCN_USER);
	    TTCN_Logger::log_event_str("----Dumping HashMap called: ");
	    pl_name.log();
	    TTCN_Logger::log_event_str(" begin----");
	    TTCN_Logger::end_event();
	  }
	  catch (...)
	  {
	    TTCN_Logger::finish_event();
	    throw;
	  }

	  INTEGER pl_id;
	  if(getID(pl_name,pl_id))
	  {

	    dumpByID(pl_id);

	  }

	  //std::cout <<"----Dumping HashMap called: "<<pl_name<<" end----"<<std::endl;
	  try {
	    TTCN_Logger::begin_event(TTCN_USER);
	    TTCN_Logger::log_event_str("----Dumping HashMap called: ");
	    pl_name.log();
	    TTCN_Logger::log_event_str(" end----");
	    TTCN_Logger::end_event();
	  }
	  catch (...)
	  {
	    TTCN_Logger::finish_event();
	    throw;
	  }
	}

	void dumpAll ()
	{
	  TTCN_Logger::log_str(TTCN_USER,"----Dumping all begin----");

	  NamesHashMapType::iterator v_HashNames_Iter;

	  for ( v_HashNames_Iter = v_HashNames.begin();
	        v_HashNames_Iter != v_HashNames.end(); v_HashNames_Iter++ )
	  {
	    CHARSTRING pl_name = (CHARSTRING)v_HashNames_Iter->first;
	    dump (pl_name);
	  }
	  TTCN_Logger::log_str(TTCN_USER,"----Dumping all end----");
	}

	void assign (
	  const INTEGER& pl_id1,
	  const INTEGER& pl_id2)
	{
		assertValid(pl_id1);
		assertValid(pl_id2);
		dumpAll ();
	  *(v_HashMap[pl_id1]) = *(v_HashMap[pl_id2]);
	}

	void swapHashmaps (
	  const INTEGER& pl_id1,
	  const INTEGER& pl_id2)
	{
		assertValid(pl_id1);
		assertValid(pl_id2);

	  swap(v_HashMap[pl_id1],v_HashMap[pl_id2]);
	}

	template <class KeytType> void insert (
	  const INTEGER& pl_id,
	  const KeytType& pl_key,
	  const INTEGER& pl_data )
	{
		assertValid(pl_id);

	  typedef std::pair<KeytType, INTEGER> hashMap_pair;
	  //FIXME Error handling?!
	  v_HashMap[pl_id]->insert(hashMap_pair(pl_key, pl_data));

	  //v_HashMap[pl_id]->checkAllowedMaxSize();
	}

	template <class KeytType> void update (
	  const INTEGER& pl_id,
	  const KeytType& pl_key,
	  const INTEGER& pl_data )
	{
		assertValid(pl_id);

	  HashMapTypeIter Hashmap_Iter = v_HashMap[pl_id]->find(pl_key);

	  if ( v_HashMap[pl_id]->find(pl_key)!=v_HashMap[pl_id]->end() )
	  {
	    Hashmap_Iter->second=pl_data;
	  }
	  else
	  {
	    typedef std::pair<KeytType, INTEGER> hashMap_pair;
	    v_HashMap[pl_id]->insert(hashMap_pair(pl_key, pl_data));
	    //v_HashMap[pl_id]->checkAllowedMaxSize();
	  }
	}

	template <class KeytType> BOOLEAN find (
	  const INTEGER& pl_id,
	  const KeytType& pl_key,
	  INTEGER& pl_data )
	{
		assertValid(pl_id);

	  HashMapTypeIter Hashmap_Iter = v_HashMap[pl_id]->find(pl_key);

	  if ( Hashmap_Iter==v_HashMap[pl_id]->end() )
	  {
	    pl_data = -1;
	    return false;
	  }

	  pl_data = Hashmap_Iter->second;
	  return true;
	}

	template <class KeytType> void erase (
	  const INTEGER& pl_id,
	  const KeytType& pl_key )
	{
		assertValid(pl_id);

	  int key_is_found;

	  key_is_found=v_HashMap[pl_id]->erase(pl_key);

	  if (!key_is_found)
	  {
	    try {
	      TTCN_Logger::begin_event(TTCN_WARNING);
	      TTCN_Logger::log_event_str("Could not erase. Could not find the key in HashMap. Key: ");
	      pl_key.log();
	      TTCN_Logger::end_event();
	    }
	    catch (...)
	    {
	      TTCN_Logger::finish_event();
	      throw;
	    }
	  }
	}

	void clear (
	  const INTEGER& pl_id )
	{
		assertValid(pl_id);

	  v_HashMap[pl_id]->clear();
	}

	INTEGER size (
	 const INTEGER& pl_id )
	{
		assertValid(pl_id);

	  return v_HashMap[pl_id]->size();
	}

	FLOAT maxSize (
	 const INTEGER& pl_id )
	{
		assertValid(pl_id);

	  //std::cout <<"Maxsize: "<<v_HashMap[pl_id]->max_size() <<std::endl;
	  return v_HashMap[pl_id]->max_size();
	}

	template <class KeyType> INTEGER callHashFunc(
	  const INTEGER& pl_id,
	  const KeyType& pl_key)
	{
		assertValid(pl_id);

	  HashMapTypeHasher hashvalue;
#if (__GNUC__ == 4 && __GNUC_MINOR__ >= 3 || __GNUC__ > 4)          
	  hashvalue=v_HashMap[pl_id]->hash_function();
#else
          hashvalue=v_HashMap[pl_id]->hash_funct();
#endif
	  return hashvalue(pl_key);
	}

	BOOLEAN empty (
	  const INTEGER& pl_id )
	{
		assertValid(pl_id);

	  return v_HashMap[pl_id]->empty();
	}

	/*void resize (
	  const INTEGER& pl_id,
	  const INTEGER& pl_hashsize)
	{
		assertValid(pl_id);

	  v_HashMap[pl_id]->resize(pl_hashsize);
	  //v_HashMap[pl_id]->checkAllowedMaxSize();
	}*/

	INTEGER bucketCount (
	  const INTEGER& pl_id )
	{
		assertValid(pl_id);

	  return v_HashMap[pl_id]->bucket_count();
	}

	template <class KeytType> BOOLEAN begin(
	  const INTEGER& pl_id,
	  KeytType& pl_key)
	{
		assertValid(pl_id);

	  if ( v_HashMap[pl_id]->empty() )
	  {
	    //pl_key =0;
	    return false;
	  }

	  HashMapTypeIter Hashmap_Iter;
	  Hashmap_Iter = v_HashMap[pl_id]->begin();
	  pl_key = (KeytType)Hashmap_Iter->first;
	  return true;
	}

	template <class KeytType> BOOLEAN next(
	  const INTEGER& pl_id,
	  KeytType& pl_iter)
	{
		assertValid(pl_id);

	  if ( v_HashMap[pl_id]->empty() )
	  {
	    return false;
	  }

	  HashMapTypeIter Hashmap_Iter;
	  Hashmap_Iter = v_HashMap[pl_id]->find(pl_iter);
	  if (++Hashmap_Iter!=v_HashMap[pl_id]->end())
	  {
	    pl_iter = (KeytType)Hashmap_Iter->first;
	    return true;
	  }
	  else
	  {
	    return false;
	  }
	}
};

template<class T, class H, class F> HashMapTypeCT<T,H,F>* f_EPTF_HashMap_InitComponent(){
  return new HashMapTypeCT<T,H,F>();
};

///////////////////////////////////////////////////////////
// Function: f_EPTF_int2int_HashMap_Cleanup
//
// Purpose:
//   External function for deleting ALL hashmaps.
//
// Parameters:
//   -
// Return Value:
//   -
//
//  Errors:
//   -
///////////////////////////////////////////////////////////

template<class T, class H, class F> void f_EPTF_HashMap_Cleanup(HashMapTypeCT<T,H,F>* pl_comp)
{
  delete pl_comp;
  pl_comp = NULL;
}

///////////////////////////////////////////////////////////
// Function: f_EPTF_int2int_HashMap_New
//
// Purpose:
//   Creates a new hashmap.
//
// Parameters:
//   pl_name - *in* *charstring* - the name of the hashmap
//
// Return Value:
//   integer - the ID of the hashmap
///////////////////////////////////////////////////////////

template<class T, class H, class F> INTEGER f_EPTF_HashMap_New (
		HashMapTypeCT<T,H,F>* pl_comp,
  const CHARSTRING& pl_name)
{
	return pl_comp->newHashmap(pl_name);
}

///////////////////////////////////////////////////////////
// Function: f_EPTF_int2int_HashMap_GetID
//
// Purpose:
//   To get the ID of the hashmap from the name.
//
// Parameters:
//   pl_name - *in* *charstring* - the name of the hashmap
//   pl_id - *inout* *integer* - the ID of the hashmap
//
// Return Value:
//   boolean - true if ID found
///////////////////////////////////////////////////////////

template<class T, class H, class F> BOOLEAN f_EPTF_HashMap_GetID (
		HashMapTypeCT<T,H,F>* pl_comp,
  const CHARSTRING& pl_name,
  INTEGER& pl_id)
{
  return pl_comp->getID(pl_name, pl_id);
}

///////////////////////////////////////////////////////////
// Function: f_EPTF_int2int_HashMap_Delete
//
// Purpose:
//   Deletes the hashmap.
//
// Parameters:
//   pl_name - *in* *charstring* - the name of the hashmap
//
// Return Value:
//   -
///////////////////////////////////////////////////////////

template<class T, class H, class F> void f_EPTF_HashMap_Delete (
		HashMapTypeCT<T,H,F>* pl_comp,
  const CHARSTRING& pl_name)
{
  pl_comp->deleteHashMap(pl_name);
}

///////////////////////////////////////////////////////////
// Function: f_EPTF_int2int_HashMap_DumpByID
//
// Purpose:
//   Dump the content of a specific hashmap by ID.
//
// Parameters:
//   pl_id - *in* *integer* - the ID of the hashmap
//
// Return Value:
//   -
///////////////////////////////////////////////////////////

template<class T, class H, class F> void f_EPTF_HashMap_DumpByID (
		HashMapTypeCT<T,H,F>* pl_comp,
  const INTEGER& pl_id)
{
  pl_comp->dumpByID(pl_id);
}

///////////////////////////////////////////////////////////
// Function: f_EPTF_int2int_HashMap_Dump
//
// Purpose:
//   Dump the content of a specific hashmap by name.
//
// Parameters:
//   pl_name - *in* *charstring* - the name of the hashmap
//
// Return Value:
//   -
///////////////////////////////////////////////////////////

template<class T, class H, class F> void f_EPTF_HashMap_Dump (
		HashMapTypeCT<T,H,F>* pl_comp,
  const CHARSTRING& pl_name)
{
  pl_comp->dump(pl_name);
}

///////////////////////////////////////////////////////////
// Function: f_EPTF_int2int_HashMap_DumpAll
//
// Purpose:
//   Dump the content of all hashmaps.
//
// Parameters:
//   -
//
// Return Value:
//   -
///////////////////////////////////////////////////////////

template<class T, class H, class F> void f_EPTF_HashMap_DumpAll (HashMapTypeCT<T,H,F>* pl_comp)
{
  pl_comp->dumpAll();
}

///////////////////////////////////////////////////////////
// Function: f_EPTF_int2int_HashMap_Assign
//
// Purpose:
//   Assignment of one hashmap to another. pl_id1 := pl_id2.
//
// Parameters:
//   pl_id1 - *in* *integer* - the ID of the hashmap1
//   pl_id2 - *in* *integer* - the ID of the hashmap2
//
// Return Value:
//   -
///////////////////////////////////////////////////////////

template<class T, class H, class F> void f_EPTF_HashMap_Assign (
		HashMapTypeCT<T,H,F>* pl_comp,
  const INTEGER& pl_id1,
  const INTEGER& pl_id2)
{
	pl_comp->assign(pl_id1,pl_id2);
}



///////////////////////////////////////////////////////////
// Function: f_EPTF_int2int_HashMap_Insert
//
// Purpose:
//   Inserts an element to hashmap.
//   In C++ STL:
//               std::pair<iterator, bool>
//               insert(const value_type& x)
//
// Parameters:
//   pl_id - *in* *integer* - the ID of the hashmap
//   pl_key - *in* *integer* - the key of the hashmap
//   pl_data - *in* *integer* - the data to be inserted
//
// Return Value:
//   -
///////////////////////////////////////////////////////////

template<class T, class H, class F> void f_EPTF_HashMap_Insert (
		HashMapTypeCT<T,H,F>* pl_comp,
  const INTEGER& pl_id,
  const INTEGER& pl_key,
  const INTEGER& pl_data )
{
  pl_comp->insert(pl_id, pl_key, pl_data);
};

///////////////////////////////////////////////////////////
// Function: f_EPTF_int2int_HashMap_Update
//
// Purpose:
//   Updates an element in hashmap.
//
// Parameters:
//   pl_id - *in* *integer* - the ID of the hashmap
//   pl_key - *in* *integer* - the key of the hashmap
//   pl_data - *in* *integer* - the data to be inserted
//
// Return Value:
//   -
///////////////////////////////////////////////////////////

template<class T, class H, class F> void f_EPTF_HashMap_Update (
		HashMapTypeCT<T,H,F>* pl_comp,
  const INTEGER& pl_id,
  const INTEGER& pl_key,
  const INTEGER& pl_data )
{
  pl_comp->update(pl_id, pl_key, pl_data);
};

///////////////////////////////////////////////////////////
// Function: f_EPTF_int2int_HashMap_Find
//
// Purpose:
//   Finds an element in hashmap.
//   In C++ STL:
//               iterator find(const key_type& k)
//
// Parameters:
//   pl_id - *in* *integer* - the ID of the hashmap
//   pl_key - *in* *integer* - the key of the hashmap
//   pl_data - *out* *integer* - the searched data
//
// Return Value:
//   boolean - if we found the data
///////////////////////////////////////////////////////////

template<class T, class H, class F> BOOLEAN f_EPTF_HashMap_Find (
		HashMapTypeCT<T,H,F>* pl_comp,
  const INTEGER& pl_id,
  const INTEGER& pl_key,
  INTEGER& pl_data )
{
  return pl_comp->find(pl_id, pl_key, pl_data);
}

///////////////////////////////////////////////////////////
// Function: f_EPTF_int2int_HashMap_Erase
//
// Purpose:
//   Erases an element from hashmap.
//   In C++ STL:
//               size_type erase(const key_type& k)
//
// Parameters:
//   pl_id - *in* *integer* - the ID of the hashmap
//   pl_key - *in* *integer* - the key of the hashmap
//
// Return Value:
//   -
/////////////////////////////////////////////////////////

template<class T, class H, class F> void f_EPTF_HashMap_Erase (
		HashMapTypeCT<T,H,F>* pl_comp,
  const INTEGER& pl_id,
  const INTEGER& pl_key )
{
  pl_comp->erase(pl_id, pl_key);
}

///////////////////////////////////////////////////////////
// Function: f_EPTF_int2int_HashMap_Clear
//
// Purpose:
//   Clear all elements from hashmap.
//   In C++ STL:
//               void clear()
//
// Parameters:
//   pl_id - *in* *integer* - the ID of the hashmap
//
// Return Value:
//   -
/////////////////////////////////////////////////////////

template<class T, class H, class F> void f_EPTF_HashMap_Clear (
		HashMapTypeCT<T,H,F>* pl_comp,
  const INTEGER& pl_id )
{
  pl_comp->clear(pl_id);
}

///////////////////////////////////////////////////////////
// Function: f_EPTF_int2int_HashMap_Size
//
// Purpose:
//   Returns the size of hashmap. Size = the used buckets.
//   In C++ STL:
//               size_type size() const
//
// Parameters:
//   pl_id - *in* *integer* - the ID of the hashmap
//
// Return Value:
//   integer - the size of hashmap
//////////////////////////////////////////////////////////

template<class T, class H, class F> INTEGER f_EPTF_HashMap_Size (
		HashMapTypeCT<T,H,F>* pl_comp,
  const INTEGER& pl_id )
{
  return pl_comp->size(pl_id);
}

///////////////////////////////////////////////////////////
// Function: f_EPTF_int2int_HashMap_MaxSize
//
// Purpose:
//   Returns the maximum size of hashmap.
//   In C++ STL:
//               size_type max_size() const
//
// Parameters:
//   pl_id - *in* *integer* - the ID of the hashmap
//
// Return Value:
//   float - the maximum size of hashmap
//////////////////////////////////////////////////////////

template<class T, class H, class F> FLOAT f_EPTF_HashMap_MaxSize (
		HashMapTypeCT<T,H,F>* pl_comp,
  const INTEGER& pl_id )
{
  return pl_comp->maxSize(pl_id);
}

///////////////////////////////////////////////////////////
// Function: f_EPTF_int2int_HashMap_CallHashFunc
//
// Purpose:
//   Calls the hash function of a given hashmap.
//
// Parameters:
//   pl_id - *in* *integer* - the ID of the hashmap, which hashfunction we use
//   pl_key - *in* *integer* - the key of the hashmap
//
// Return Value:
//   integer - the hashvalue of key
//////////////////////////////////////////////////////////

template<class T, class H, class F> INTEGER f_EPTF_HashMap_CallHashFunc(
		HashMapTypeCT<T,H,F>* pl_comp,
  const INTEGER& pl_id,
  const INTEGER& pl_key)
{
  return pl_comp->callHashFunc(pl_id, pl_key);
}

///////////////////////////////////////////////////////////
// Function: f_EPTF_int2int_HashMap_Empty
//
// Purpose:
//   Check whether the hashmap is empty. true if the unordered_map's size is 0.
//   In C++ STL:
//               bool empty() cons
//
// Parameters:
//   pl_id - *in* *integer* - the ID of the hashmap
//
// Return Value:
//   boolean - true if the unordered_map's size is 0.
//////////////////////////////////////////////////////////

template<class T, class H, class F> BOOLEAN f_EPTF_HashMap_Empty (
		HashMapTypeCT<T,H,F>* pl_comp,
  const INTEGER& pl_id )
{
  return pl_comp->empty();
}

///////////////////////////////////////////////////////////
// Function: f_EPTF_int2int_HashMap_Resize
//
// Purpose:
//   Increases the bucket count to at least pl_hashsize.
//   In C++ STL:
//               void resize(size_type n)
//
// Parameters:
//   pl_id - *in* *integer* - the ID of the hashmap
//   pl_hashsize - *in* *integer* - the new bucketsize
//
// Return Value:
//   -
//////////////////////////////////////////////////////////

template<class T, class H, class F> void f_EPTF_HashMap_Resize (
		HashMapTypeCT<T,H,F>* pl_comp,
  const INTEGER& pl_id,
  const INTEGER& pl_hashsize)
{
  pl_comp->resize(pl_id, pl_hashsize);
};

///////////////////////////////////////////////////////////
// Function: f_EPTF_int2int_HashMap_BucketCount
//
// Purpose:
//   Returns the number of buckets used by the unordered_map. This is the booked size.
//   It contains empty buckets also.
//   In C++ STL:
//               size_type bucket_count() const
//
// Parameters:
//   pl_id - *in* *integer* - the ID of the hashmap
//
// Return Value:
//   integer - the bucketsize
//////////////////////////////////////////////////////////

template<class T, class H, class F> INTEGER f_EPTF_HashMap_BucketCount (
		HashMapTypeCT<T,H,F>* pl_comp,
  const INTEGER& pl_id )
{
  return pl_comp->bucketCount(pl_id);
};

///////////////////////////////////////////////////////////
// Function: f_EPTF_int2int_HashMap_Begin
//
// Purpose:
//   External function call to get the first element of hashmap.
//   In TTCN we can't use iterators, so we get back the first key element.
//
// Parameters:
//   pl_id - *in* *integer* - the ID of the hashmap
//   pl_key - *inout* *integer* - the first key in hashmap
//
// Return Value:
//   boolean - false if the hashmap is empty
//////////////////////////////////////////////////////////

template<class T, class H, class F> BOOLEAN f_EPTF_HashMap_Begin(
		HashMapTypeCT<T,H,F>* pl_comp,
  const INTEGER& pl_id,
  INTEGER& pl_key)
{
  return pl_comp->begin(pl_id);
}

///////////////////////////////////////////////////////////
// Function: f_EPTF_int2int_HashMap_Next
//
// Purpose:
//   External function call to get the next element of hashmap.
//   In TTCN we can't use iterators, so we get back a key element's next key element.
//
// Parameters:
//   pl_id - *in* *integer* - the ID of the hashmap
//   pl_iter - *inout* *charstring* - in = the iterator of the hashmap (iterator = key)
//                                    out = the next key in hashmap
//
// Return Value:
//   boolean - false if no next key in hashmap
//////////////////////////////////////////////////////////

template<class T, class H, class F> BOOLEAN f_EPTF_HashMap_Next(
		HashMapTypeCT<T,H,F>* pl_comp,
  const INTEGER& pl_id,
  INTEGER& pl_iter)
{
  return pl_comp->next(pl_id, pl_iter);
}

}
#endif //EPTF_CLL_HashMap_ExternalFunctions_hh
