/******************************************************************************
 * Copyright (c) 2000-2021 Ericsson Telecom AB
 * All rights reserved. This program and the accompanying materials
 * are made available under the terms of the Eclipse Public License v2.0
 * which accompanies this distribution, and is available at
 * https://www.eclipse.org/org/documents/epl-2.0/EPL-2.0.html
 ******************************************************************************/
//  File:     EPTF_CLL_NQueue_ExternalFunctions.cc
//  Rev:      <RnXnn>
//  Prodnr:   CNL 113 512
//  Updated:  2012-06-20
//  Contact:  http://ttcn.ericsson.se
///////////////////////////////////////////////////////////////////////////////

#include "EPTF_CLL_Base_Functions.hh"
#include "EPTF_CLL_NQueue_Functions.hh"

// may be specified in makefile
#ifndef EPTF_NQUEUE_ITEMGROWBY
#define EPTF_NQUEUE_ITEMGROWBY 64
#endif

#if EPTF_NQUEUE_ITEMGROWBY < 1
#error "EPTF_NQUEUE_ITEMGROWBY must be a positive integer number"
#endif

#ifdef EPTF_DEBUG
#define EPTF_NQUEUE_DEBUG
#endif

#ifdef EPTF_NQUEUE_DEBUG
#include "EPTF_CLL_Common_Definitions.hh"
#endif

using namespace EPTF__CLL__NQueue__Definitions;

namespace EPTF__CLL__NQueue__Functions {

typedef struct {
  int next;
  int prev;
  int chain;
} EPTF_NQueue_Item;

typedef struct {
  int head;
  int tail;
  int itemCount;
  #ifdef EPTF_NQUEUE_DEBUG
  int maxItemCount;
  #endif    
} EPTF_NQueue_Chain;

typedef struct {
  EPTF_NQueue_Item *items;
  EPTF_NQueue_Chain *chains;
  int chainCount;
  int itemCount;
  int itemAllocCount;
  char *name;
  bool deleted;
  #ifdef EPTF_NQUEUE_DEBUG
  int maxItemCount;
  int maxChainCount;
  #endif    
} EPTF_NQueue_Queue;

static EPTF_NQueue_Queue *EPTF_NQueue_queues = NULL;
static int EPTF_NQueue_queueCount = 0;

static int *EPTF_NQueue_deletedQueues = NULL;
static int EPTF_NQueue_deletedQueuesCount = 0;
static int EPTF_NQueue_deletedQueuesAllocCount = 0;

#ifdef EPTF_NQUEUE_DEBUG
static int EPTF_NQueue_maxCount;
static double EPTF_NQueue_increaseRate;
#endif

#include <stdio.h>
#include <stdarg.h>
void f_EPTF_NQueue_error(const char *msg, ...)
{
  va_list ap;
  va_start(ap, msg);
  char err_str[4096];
  vsprintf(err_str, msg, ap);
  va_end(ap);
#ifdef EPTF_DEBUG
  EPTF__CLL__Base__Functions::f__EPTF__Base__assert(CHARSTRING(err_str), BOOLEAN(FALSE));
#else
  TTCN_Logger::log_str(TTCN_Logger::ERROR_UNQUALIFIED, err_str);
  EPTF__CLL__Base__Functions::f__EPTF__Base__stop(FAIL);
  //TTCN_error(err_str);
#endif
}

void f_EPTF_NQueue_addDeletedQueueToList(int queueId)
{
  if(EPTF_NQueue_deletedQueues == NULL) {
    EPTF_NQueue_deletedQueuesAllocCount = EPTF_NQUEUE_ITEMGROWBY;
    EPTF_NQueue_deletedQueues = (int*) Malloc(sizeof(int) * EPTF_NQueue_deletedQueuesAllocCount);
  } else if(EPTF_NQueue_deletedQueuesCount+1 >= EPTF_NQueue_deletedQueuesAllocCount) {
    EPTF_NQueue_deletedQueuesAllocCount += EPTF_NQUEUE_ITEMGROWBY;
    EPTF_NQueue_deletedQueues = (int*) Realloc(EPTF_NQueue_deletedQueues,
      sizeof(int) * EPTF_NQueue_deletedQueuesAllocCount);
  }
  EPTF_NQueue_deletedQueues[EPTF_NQueue_deletedQueuesCount] = queueId;
  EPTF_NQueue_deletedQueuesCount++;
}

int f_EPTF_NQueue_getDeletedQueueFromList() // returns -1 if none, also removes from list!
{
  int ret_val = -1;
  if(EPTF_NQueue_deletedQueuesCount > 0) {
    EPTF_NQueue_deletedQueuesCount--;
    ret_val = EPTF_NQueue_deletedQueues[EPTF_NQueue_deletedQueuesCount];
/*    if(EPTF_NQueue_deletedQueuesCount == 0) {
      Free(EPTF_NQueue_deletedQueues);
      EPTF_NQueue_deletedQueues = NULL;
      EPTF_NQueue_deletedQueuesAllocCount = 0;
    }*/
  }
  return ret_val;
}

void f_EPTF_NQueue_logQueue(int queueId);
void f_EPTF_NQueue_logChain(int queueId, int chainId);
void f_EPTF_NQueue_logChainFields(int queueId, int chainId);

inline void f_EPTF_NQueue_checkQueueId(const char *fn, int queueId)
{
//#ifdef EPTF_DEBUG
  if(queueId<0 || queueId>=EPTF_NQueue_queueCount) {
    f_EPTF_NQueue_error("%s: invalid queue %d, number of queues: %d", fn, queueId, EPTF_NQueue_queueCount);
  }
  if(EPTF_NQueue_queues[queueId].deleted) {
    f_EPTF_NQueue_error("%s: invalid queue %d, queue has been deleted.", fn, queueId);
  }
  //#endif
}

inline void f_EPTF_NQueue_checkChainId(const char *fn, int queueId, int chainId)
{
//#ifdef EPTF_DEBUG
  if(chainId<0 || chainId>=EPTF_NQueue_queues[queueId].chainCount) {
    if(EPTF_NQueue_queues[queueId].name) {
      f_EPTF_NQueue_error("%s: invalid chain %d, number of chains in queue \"%s\": %d",
        fn, chainId, EPTF_NQueue_queues[queueId].name, EPTF_NQueue_queues[queueId].chainCount);
    } else {
      f_EPTF_NQueue_error("%s: invalid chain %d, number of chains in queue: %d",
        fn, chainId, EPTF_NQueue_queues[queueId].chainCount);
    }
  }
  //#endif
}

inline void f_EPTF_NQueue_checkItemIdx(const char *fn, int queueId, int itemIdx)
{
//#ifdef EPTF_DEBUG
  if(itemIdx<0 || itemIdx>=EPTF_NQueue_queues[queueId].itemCount) {
    if(EPTF_NQueue_queues[queueId].name) {
      f_EPTF_NQueue_error("%s: invalid item %d, number of items in queue \"%s\": %d",
        fn, itemIdx, EPTF_NQueue_queues[queueId].name, EPTF_NQueue_queues[queueId].itemCount);
    } else {
      f_EPTF_NQueue_error("%s: invalid item %d, number of items in queue: %d",
        fn, itemIdx, EPTF_NQueue_queues[queueId].itemCount);
    }
  }
  //#endif
}

#ifdef EPTF_NQUEUE_DEBUG
inline void f_EPTF_NQueue_checkQueueSize(int queueId)
{
  if((EPTF_NQueue_maxCount != -1) && (EPTF_NQueue_queues[queueId].maxItemCount <  EPTF_NQueue_queues[queueId].itemCount))
  {
  	TTCN_warning("NQueue: queue with id: %d has exceeded the maximum item count! "
          "New maxItemCount of queue is being increased by a rate of %f on the basis of new item count, from %d to %d.",
          queueId, EPTF_NQueue_increaseRate, EPTF_NQueue_queues[queueId].maxItemCount,
          (int) (EPTF_NQueue_queues[queueId].itemCount * (1+EPTF_NQueue_increaseRate)));
  	EPTF_NQueue_queues[queueId].maxItemCount = (int) (EPTF_NQueue_queues[queueId].itemCount * (1+EPTF_NQueue_increaseRate));
  }
  
}

inline void f_EPTF_NQueue_checkChainSize(int queueId, int chainId)
{
  if((EPTF_NQueue_maxCount != -1) && (EPTF_NQueue_queues[queueId].chains[chainId].maxItemCount < EPTF_NQueue_queues[queueId].chains[chainId].itemCount))
  {
  	TTCN_warning("NQueue: chain with id: %d in queue: %d has exceeded the maximum item count! "
          "New maxItemCount of chain is being increased by a rate of %f on the basis of new item count, from %d to %d.",
          chainId, queueId, EPTF_NQueue_increaseRate, EPTF_NQueue_queues[queueId].chains[chainId].maxItemCount,
          (int) (EPTF_NQueue_queues[queueId].chains[chainId].itemCount * (1+EPTF_NQueue_increaseRate)));
  	EPTF_NQueue_queues[queueId].chains[chainId].maxItemCount = (int) (EPTF_NQueue_queues[queueId].chains[chainId].itemCount * (1+EPTF_NQueue_increaseRate));
  }
}

inline void f_EPTF_NQueue_checkChainCount(int queueId)
{
  if((EPTF_NQueue_maxCount != -1) && (EPTF_NQueue_queues[queueId].maxChainCount < EPTF_NQueue_queues[queueId].chainCount))
  {
  	TTCN_warning("NQueue: queue with id: %d has exceeded the maximum chain count! "
          "New maxChainCount of queue is being increased by a rate of %f, from %d to %d.",
          queueId, EPTF_NQueue_increaseRate, EPTF_NQueue_queues[queueId].maxChainCount,
          (int) (EPTF_NQueue_queues[queueId].maxChainCount * (1+EPTF_NQueue_increaseRate)));
  	EPTF_NQueue_queues[queueId].maxChainCount = (int) (EPTF_NQueue_queues[queueId].maxChainCount * (1+EPTF_NQueue_increaseRate));
  }
}
#endif

void f__EPTF__NQueue__init()
{
  EPTF_NQueue_queues = NULL;
  EPTF_NQueue_queueCount = 0;
  EPTF_NQueue_deletedQueues = NULL;
  EPTF_NQueue_deletedQueuesCount = 0;
  EPTF_NQueue_deletedQueuesAllocCount = 0;
    
  #ifdef EPTF_NQUEUE_DEBUG
  EPTF_NQueue_maxCount = (int)EPTF__CLL__Common__Definitions::tsp__CLL__debug__acceptableMaxSizeOfGrowingVariables;
  EPTF_NQueue_increaseRate = (double)EPTF__CLL__Common__Definitions::tsp__CLL__debug__increasePercentage4AcceptableMaxSize;
  if(EPTF_NQueue_maxCount == -1)
  {  	 	
  	return;
  }
  if(EPTF_NQueue_increaseRate <= 0)
  {
  	EPTF_NQueue_increaseRate = 0.1;
  }
  if(EPTF_NQueue_maxCount * EPTF_NQueue_increaseRate < 1.0)
  {
  	EPTF_NQueue_maxCount = ((int) 1 / EPTF_NQueue_increaseRate) +1;
  }  
  #endif
}

void f_EPTF_NQueue_cleanupQueue(int queueId)
{
  if(EPTF_NQueue_queues[queueId].deleted) { return; }
  if(EPTF_NQueue_queues[queueId].items) {
    Free(EPTF_NQueue_queues[queueId].items);
  }
  if(EPTF_NQueue_queues[queueId].chains) {
    Free(EPTF_NQueue_queues[queueId].chains);
  }
  EPTF_NQueue_queues[queueId].items = NULL;
  EPTF_NQueue_queues[queueId].chains = NULL;
  if(EPTF_NQueue_queues[queueId].name) {
    Free(EPTF_NQueue_queues[queueId].name);
  }
  EPTF_NQueue_queues[queueId].name = NULL;
  EPTF_NQueue_queues[queueId].deleted = true;
}

void f__EPTF__NQueue__cleanup()
{
  if(EPTF_NQueue_queues) {
    for(int queueId=0; queueId<EPTF_NQueue_queueCount; queueId++) {
      f_EPTF_NQueue_cleanupQueue(queueId);
    }
    Free(EPTF_NQueue_queues);
  }
  if(EPTF_NQueue_deletedQueues) {
    Free(EPTF_NQueue_deletedQueues);
  }
  EPTF_NQueue_queues = NULL;
  EPTF_NQueue_queueCount = 0;
  EPTF_NQueue_deletedQueues = NULL;
  EPTF_NQueue_deletedQueuesCount = 0;
  EPTF_NQueue_deletedQueuesAllocCount = 0;
}

EPTF__NQueue__QueueId f__EPTF__NQueue__createQueue(const CHARSTRING& pl__name)
{
  int queueId = f_EPTF_NQueue_getDeletedQueueFromList();
  if(queueId < 0) {
    queueId = EPTF_NQueue_queueCount;
    EPTF_NQueue_queueCount++;
    if(EPTF_NQueue_queues == NULL) EPTF_NQueue_queues = (EPTF_NQueue_Queue*) Malloc(sizeof(EPTF_NQueue_Queue));
    else EPTF_NQueue_queues = (EPTF_NQueue_Queue*) Realloc(EPTF_NQueue_queues, EPTF_NQueue_queueCount * sizeof(EPTF_NQueue_Queue));
  }
  EPTF_NQueue_queues[queueId].items = NULL;
  EPTF_NQueue_queues[queueId].chains = NULL;
  EPTF_NQueue_queues[queueId].chainCount = 0;
  EPTF_NQueue_queues[queueId].itemCount = 0;
  EPTF_NQueue_queues[queueId].itemAllocCount = 0;
  #ifdef EPTF_NQUEUE_DEBUG
  EPTF_NQueue_queues[queueId].maxItemCount = EPTF_NQueue_maxCount;
  EPTF_NQueue_queues[queueId].maxChainCount = EPTF_NQueue_maxCount; 
  #endif  
  EPTF_NQueue_queues[queueId].name = NULL;
  if(pl__name.lengthof() != 0) {
    EPTF_NQueue_queues[queueId].name = (char*)Malloc(pl__name.lengthof() + 1);
    memcpy(EPTF_NQueue_queues[queueId].name, (const char*)pl__name, pl__name.lengthof());
    EPTF_NQueue_queues[queueId].name[pl__name.lengthof()] = 0;
  }
  EPTF_NQueue_queues[queueId].deleted = false;
  return EPTF__NQueue__QueueId(queueId);
}

void f__EPTF__NQueue__deleteQueue(const EPTF__NQueue__QueueId &pl__queue)
{
  int queueId = (int)pl__queue;
  f_EPTF_NQueue_checkQueueId("f_EPTF_NQueue_deleteQueue", queueId);
  f_EPTF_NQueue_cleanupQueue(queueId);
  f_EPTF_NQueue_addDeletedQueueToList(queueId);
}

CHARSTRING f__EPTF__NQueue__getNameOfQueue(const EPTF__NQueue__QueueId &pl__queue)
{
  int queueId = (int)pl__queue;
  f_EPTF_NQueue_checkQueueId("f_EPTF_NQueue_getNameOfQueue", queueId);
  return CHARSTRING(EPTF_NQueue_queues[queueId].name);
}

EPTF__NQueue__ChainId f__EPTF__NQueue__createChain(
  const EPTF__NQueue__QueueId &pl__queue)
{
  int queueId = (int)pl__queue, chainId;
  f_EPTF_NQueue_checkQueueId("f_EPTF_NQueue_createChain", queueId);
  if(EPTF_NQueue_queues[queueId].chains == NULL) {
    EPTF_NQueue_queues[queueId].chains = (EPTF_NQueue_Chain*) Malloc(sizeof(EPTF_NQueue_Chain));
    EPTF_NQueue_queues[queueId].chainCount = 1;
    chainId = 0;
  } else {
    chainId = EPTF_NQueue_queues[queueId].chainCount;
    EPTF_NQueue_queues[queueId].chainCount++;
    EPTF_NQueue_queues[queueId].chains = (EPTF_NQueue_Chain*) Realloc(EPTF_NQueue_queues[queueId].chains, EPTF_NQueue_queues[queueId].chainCount * sizeof(EPTF_NQueue_Chain));
  }
  EPTF_NQueue_queues[queueId].chains[chainId].head = -1;
  EPTF_NQueue_queues[queueId].chains[chainId].tail = -1;
  EPTF_NQueue_queues[queueId].chains[chainId].itemCount = 0;
  #ifdef EPTF_NQUEUE_DEBUG
  EPTF_NQueue_queues[queueId].chains[chainId].maxItemCount = EPTF_NQueue_maxCount;
  f_EPTF_NQueue_checkChainCount(queueId);
  #endif
  return EPTF__NQueue__ChainId(chainId);
}

int f_EPTF_NQueue_createItem(int queueId)
{
  f_EPTF_NQueue_checkQueueId("f_EPTF_NQueue_createItem", queueId);
  int itemIdx;
  if(EPTF_NQueue_queues[queueId].items == NULL) {
    EPTF_NQueue_queues[queueId].items = (EPTF_NQueue_Item*) Malloc(EPTF_NQUEUE_ITEMGROWBY * sizeof(EPTF_NQueue_Item));
    EPTF_NQueue_queues[queueId].itemCount = 1;
    EPTF_NQueue_queues[queueId].itemAllocCount = EPTF_NQUEUE_ITEMGROWBY;
    itemIdx = 0;
  } else {
    itemIdx = EPTF_NQueue_queues[queueId].itemCount;
    EPTF_NQueue_queues[queueId].itemCount++;
    if(itemIdx >= EPTF_NQueue_queues[queueId].itemAllocCount) {
      EPTF_NQueue_queues[queueId].itemAllocCount += EPTF_NQUEUE_ITEMGROWBY;
      EPTF_NQueue_queues[queueId].items = (EPTF_NQueue_Item*) Realloc(EPTF_NQueue_queues[queueId].items, EPTF_NQueue_queues[queueId].itemAllocCount * sizeof(EPTF_NQueue_Item));
    }
  }
  #ifdef EPTF_NQUEUE_DEBUG
  f_EPTF_NQueue_checkQueueSize(queueId);
  #endif
  return itemIdx;
}

void f_EPTF_NQueue_createItems(int queueId, int count)
{
  f_EPTF_NQueue_checkQueueId("f_EPTF_NQueue_createItems", queueId);
  // round up to nearest EPTF_NQUEUE_ITEMGROWBY granularity:
  int allocCount = ((count + EPTF_NQueue_queues[queueId].itemCount + EPTF_NQUEUE_ITEMGROWBY - 1) / EPTF_NQUEUE_ITEMGROWBY) * EPTF_NQUEUE_ITEMGROWBY;
  //TTCN_Logger::log(TTCN_USER, "f_EPTF_NQueue_createItems: count: %d, itemCount: %d, current alloc: %d, allocCount = %d", count, EPTF_NQueue_queues[queueId].itemCount, EPTF_NQueue_queues[queueId].itemAllocCount, allocCount);

  if(EPTF_NQueue_queues[queueId].items == NULL) {
    EPTF_NQueue_queues[queueId].items = (EPTF_NQueue_Item*) Malloc(allocCount * sizeof(EPTF_NQueue_Item));
    EPTF_NQueue_queues[queueId].itemCount = count;
    EPTF_NQueue_queues[queueId].itemAllocCount = allocCount;
  } else {
    EPTF_NQueue_queues[queueId].itemCount += count;
    if(allocCount >= EPTF_NQueue_queues[queueId].itemAllocCount) {
      EPTF_NQueue_queues[queueId].itemAllocCount = allocCount;
      EPTF_NQueue_queues[queueId].items = (EPTF_NQueue_Item*) Realloc(EPTF_NQueue_queues[queueId].items, EPTF_NQueue_queues[queueId].itemAllocCount * sizeof(EPTF_NQueue_Item));
    }
  }
  #ifdef EPTF_NQUEUE_DEBUG
  f_EPTF_NQueue_checkQueueSize(queueId);
  #endif
//  TTCN_Logger::log(TTCN_USER, "f_EPTF_NQueue_createItems: itemCount = %d", EPTF_NQueue_queues[queueId].itemCount);
//  TTCN_Logger::log(TTCN_USER, "f_EPTF_NQueue_createItems: itemAllocCount = %d", EPTF_NQueue_queues[queueId].itemAllocCount);
}

void f_EPTF_NQueue_chainItemsFwd(int queueId, int chainId, int itemIdx, int itemCount) // itemCount MUST be >0
{
  int firstIdx = itemIdx;
  int lastIdx = itemIdx + itemCount - 1;

  while(itemCount--) {
    EPTF_NQueue_Item &item = EPTF_NQueue_queues[queueId].items[itemIdx];
    item.next = itemIdx + 1;
    item.prev = itemIdx - 1;
    item.chain = chainId;
    itemIdx++;
  }

  EPTF_NQueue_queues[queueId].items[firstIdx].prev = firstIdx; // first item's prev points to self
  EPTF_NQueue_queues[queueId].items[lastIdx].next = lastIdx; // last item's next points to self
}

void f_EPTF_NQueue_chainItemsBwd(int queueId, int chainId, int itemIdx, int itemCount) // itemCount MUST be >0
{
  int firstIdx = itemIdx;
  int lastIdx = itemIdx + itemCount - 1;

  while(itemCount--) {
    EPTF_NQueue_Item &item = EPTF_NQueue_queues[queueId].items[itemIdx];
    item.next = itemIdx - 1;
    item.prev = itemIdx + 1;
    item.chain = chainId;
    itemIdx++;
  }

  EPTF_NQueue_queues[queueId].items[firstIdx].next = firstIdx; // first item's next points to self
  EPTF_NQueue_queues[queueId].items[lastIdx].prev = lastIdx; // last item's prev points to self
}

EPTF__NQueue__ItemIdx f__EPTF__NQueue__createItemAtChainHead(
  const EPTF__NQueue__QueueId &pl__queue,
  const EPTF__NQueue__ChainId &pl__chain)
{
  int queueId = (int)pl__queue;
  int chainId = (int)pl__chain;
  f_EPTF_NQueue_checkQueueId("f_EPTF_NQueue_createItemAtChainHead", queueId);
  int itemIdx = f_EPTF_NQueue_createItem(queueId);
  f_EPTF_NQueue_checkChainId("f_EPTF_NQueue_createItemAtChainHead", queueId, chainId);
  EPTF_NQueue_Item &item = EPTF_NQueue_queues[queueId].items[itemIdx];
  EPTF_NQueue_Chain &chain = EPTF_NQueue_queues[queueId].chains[chainId];
  if(chain.head < 0) { chain.head = chain.tail = itemIdx; }
  item.next = chain.head;
  item.prev = itemIdx;
  item.chain = chainId;
  EPTF_NQueue_queues[queueId].items[chain.head].prev = itemIdx;
  chain.head = itemIdx;
  chain.itemCount++;
  #ifdef EPTF_NQUEUE_DEBUG  
  f_EPTF_NQueue_checkChainSize(queueId, chainId);
  #endif
  return EPTF__NQueue__ItemIdx(itemIdx);
}

EPTF__NQueue__ItemIdx f__EPTF__NQueue__createItemAtChainTail(
  const EPTF__NQueue__QueueId &pl__queue,
  const EPTF__NQueue__ChainId &pl__chain)
{
  int queueId = (int)pl__queue;
  int chainId = (int)pl__chain;
  f_EPTF_NQueue_checkQueueId("f_EPTF_NQueue_createItemAtChainTail", queueId);
  int itemIdx = f_EPTF_NQueue_createItem(queueId);
  f_EPTF_NQueue_checkChainId("f_EPTF_NQueue_createItemAtChainTail", queueId, chainId);
  EPTF_NQueue_Item &item = EPTF_NQueue_queues[queueId].items[itemIdx];
  EPTF_NQueue_Chain &chain = EPTF_NQueue_queues[queueId].chains[chainId];
  if(chain.head < 0) { chain.head = chain.tail = itemIdx; }
  item.next = itemIdx;
  item.prev = chain.tail;
  item.chain = chainId;
  EPTF_NQueue_queues[queueId].items[chain.tail].next = itemIdx;
  chain.tail = itemIdx;
  chain.itemCount++;
  #ifdef EPTF_NQUEUE_DEBUG  
  f_EPTF_NQueue_checkChainSize(queueId, chainId);
  #endif
  return EPTF__NQueue__ItemIdx(itemIdx);
}

EPTF__NQueue__ItemIdx f__EPTF__NQueue__createItemAfter(
  const EPTF__NQueue__QueueId &pl__queue,
  const EPTF__NQueue__ItemIdx &pl__afterIdx)
{
  int queueId = (int)pl__queue;
  int afterIdx = (int)pl__afterIdx;
  f_EPTF_NQueue_checkQueueId("f_EPTF_NQueue_createItemAfter", queueId);
  f_EPTF_NQueue_checkItemIdx("f_EPTF_NQueue_createItemAfter", queueId, afterIdx);
  int itemIdx = f_EPTF_NQueue_createItem(queueId);

  EPTF_NQueue_Item &item = EPTF_NQueue_queues[queueId].items[itemIdx];
  EPTF_NQueue_Item &after = EPTF_NQueue_queues[queueId].items[afterIdx];
  int chainId = after.chain;
  item.chain = chainId;
  EPTF_NQueue_Chain &chain = EPTF_NQueue_queues[queueId].chains[chainId];
  if(afterIdx == chain.tail) {
    chain.tail = itemIdx;
    item.next = itemIdx;
  } else {
    item.next = after.next;
    EPTF_NQueue_queues[queueId].items[after.next].prev = itemIdx;
  }
  item.prev = afterIdx;
  chain.itemCount++;
  after.next = itemIdx;
  #ifdef EPTF_NQUEUE_DEBUG  
  f_EPTF_NQueue_checkChainSize(queueId, chainId);
  #endif
  return EPTF__NQueue__ItemIdx(itemIdx);
}

EPTF__NQueue__ItemIdx f__EPTF__NQueue__createItemBefore(
  const EPTF__NQueue__QueueId &pl__queue,
  const EPTF__NQueue__ItemIdx &pl__beforeIdx)
{
  int queueId = (int)pl__queue;
  int beforeIdx = (int)pl__beforeIdx;
  f_EPTF_NQueue_checkQueueId("f_EPTF_NQueue_createItemBefore", queueId);
  f_EPTF_NQueue_checkItemIdx("f_EPTF_NQueue_createItemBefore", queueId, beforeIdx);
  int itemIdx = f_EPTF_NQueue_createItem(queueId);

  EPTF_NQueue_Item &item = EPTF_NQueue_queues[queueId].items[itemIdx];
  EPTF_NQueue_Item &before = EPTF_NQueue_queues[queueId].items[beforeIdx];
  int chainId = before.chain;
  item.chain = chainId;
  EPTF_NQueue_Chain &chain = EPTF_NQueue_queues[queueId].chains[chainId];
  if(beforeIdx == chain.head) {
    chain.head = itemIdx;
    item.prev = itemIdx;
  } else {
    item.prev = before.prev;
    EPTF_NQueue_queues[queueId].items[before.prev].next = itemIdx;
  }
  item.next = beforeIdx;
  chain.itemCount++;
  before.prev = itemIdx;
  #ifdef EPTF_NQUEUE_DEBUG  
  f_EPTF_NQueue_checkChainSize(queueId, chainId);
  #endif
  return EPTF__NQueue__ItemIdx(itemIdx);
}

void f__EPTF__NQueue__createItemsAtChainHead(
  const EPTF__NQueue__QueueId &pl__queue,
  const EPTF__NQueue__ChainId &pl__chain,
  const INTEGER &pl__count)
{
  int queueId = (int)pl__queue;
  int chainId = (int)pl__chain;
  int count = (int)pl__count;
  if(count < 1) return;
  f_EPTF_NQueue_checkQueueId("f_EPTF_NQueue_createItemsAtChainHead", queueId);
  f_EPTF_NQueue_checkChainId("f_EPTF_NQueue_createItemsAtChainHead", queueId, chainId);
  int newItem = EPTF_NQueue_queues[queueId].itemCount;
  f_EPTF_NQueue_createItems(queueId, count);
  f_EPTF_NQueue_chainItemsBwd(queueId, chainId, newItem, count);
  EPTF_NQueue_Chain &chain = EPTF_NQueue_queues[queueId].chains[chainId];
  if(chain.head < 0) {
    chain.head = newItem + count - 1;
    chain.tail = newItem;
  } else {
    EPTF_NQueue_queues[queueId].items[newItem].next = chain.head; // first new item's next points to old head
    EPTF_NQueue_queues[queueId].items[chain.head].prev = newItem; // previous head's prev points to first new item
    chain.head = newItem + count - 1; // last new item becomes head
  }
  // EPTF_NQueue_queues[queueId].items[chain.head].prev = chain.head; // chain head's prev points to self - note: already done by chainItemsBwd
  chain.itemCount += count;
  #ifdef EPTF_NQUEUE_DEBUG  
  f_EPTF_NQueue_checkChainSize(queueId, chainId);
  #endif
//  f_EPTF_NQueue_logChainFields(queueId, chainId);
}

void f__EPTF__NQueue__createItemsAtChainTail(
  const EPTF__NQueue__QueueId &pl__queue,
  const EPTF__NQueue__ChainId &pl__chain,
  const INTEGER &pl__count)
{
  int queueId = (int)pl__queue;
  int chainId = (int)pl__chain;
  int count = (int)pl__count;
  if(count < 1) return;
  f_EPTF_NQueue_checkQueueId("f_EPTF_NQueue_createItemsAtChainTail", queueId);
  f_EPTF_NQueue_checkChainId("f_EPTF_NQueue_createItemsAtChainTail", queueId, chainId);
  int newItem = EPTF_NQueue_queues[queueId].itemCount;
  f_EPTF_NQueue_createItems(queueId, count);
  f_EPTF_NQueue_chainItemsFwd(queueId, chainId, newItem, count);
  EPTF_NQueue_Chain &chain = EPTF_NQueue_queues[queueId].chains[chainId];
  if(chain.head < 0) {
    chain.head = newItem;
    chain.tail = newItem + count - 1;
  } else {
    EPTF_NQueue_queues[queueId].items[newItem].prev = chain.tail; // first new item's prev points to old tail
    EPTF_NQueue_queues[queueId].items[chain.tail].next = newItem; // previous tail's next points to first new item
    chain.tail = newItem + count - 1; // last new item becomes tail
  }
  // EPTF_NQueue_queues[queueId].items[chain.tail].next = chain.tail; // chain tail's next points to self - note: already done by chainItemsFwd
  chain.itemCount += count;
  #ifdef EPTF_NQUEUE_DEBUG  
  f_EPTF_NQueue_checkChainSize(queueId, chainId);
  #endif
//  f_EPTF_NQueue_logChainFields(queueId, chainId);
}

void f_EPTF_NQueue_unchainItem(int queueId, int itemIdx)
{
  EPTF_NQueue_Item &item = EPTF_NQueue_queues[queueId].items[itemIdx];
  EPTF_NQueue_Chain &chain = EPTF_NQueue_queues[queueId].chains[item.chain];
  item.chain = -1;
  EPTF_NQueue_queues[queueId].items[item.next].prev = item.prev;
  EPTF_NQueue_queues[queueId].items[item.prev].next = item.next;
  chain.itemCount--;
  if(chain.itemCount) {
    if(itemIdx == chain.head) {
      chain.head = item.next;
      EPTF_NQueue_queues[queueId].items[chain.head].prev = chain.head;
    }
    if(itemIdx == chain.tail) {
      chain.tail = item.prev;
      EPTF_NQueue_queues[queueId].items[chain.tail].next = chain.tail;
    }
  } else {
    chain.head = -1;
    chain.tail = -1;
  }
}

void f__EPTF__NQueue__moveToHead(
  const EPTF__NQueue__QueueId &pl__queue,
  const EPTF__NQueue__ChainId &pl__toChain, // note: can be the same chain -> move to front
  const EPTF__NQueue__ItemIdx &pl__item)
{
  int queueId = (int)pl__queue;
  int chainId = (int)pl__toChain;
  int itemIdx = (int)pl__item;
  f_EPTF_NQueue_checkQueueId("f_EPTF_NQueue_moveToHead", queueId);
  f_EPTF_NQueue_checkChainId("f_EPTF_NQueue_moveToHead", queueId, chainId);
  f_EPTF_NQueue_checkItemIdx("f_EPTF_NQueue_moveToHead", queueId, itemIdx);
  EPTF_NQueue_Item &item = EPTF_NQueue_queues[queueId].items[itemIdx];
  f_EPTF_NQueue_unchainItem(queueId, itemIdx);
  EPTF_NQueue_Chain &chain = EPTF_NQueue_queues[queueId].chains[chainId];
  if(chain.head < 0) { chain.head = chain.tail = itemIdx; }
  else {
    EPTF_NQueue_queues[queueId].items[chain.head].prev = itemIdx;
  }
  item.next = chain.head;
  item.prev = itemIdx;
  item.chain = chainId;
  chain.head = itemIdx;
  chain.itemCount++;
  #ifdef EPTF_NQUEUE_DEBUG  
  f_EPTF_NQueue_checkChainSize(queueId, chainId);
  #endif
}

void f__EPTF__NQueue__moveToTail(
  const EPTF__NQueue__QueueId &pl__queue,
  const EPTF__NQueue__ChainId &pl__toChain, // note: can be the same chain -> move to back
  const EPTF__NQueue__ItemIdx &pl__item)
{
  int queueId = (int)pl__queue;
  int chainId = (int)pl__toChain;
  int itemIdx = (int)pl__item;
  f_EPTF_NQueue_checkQueueId("f_EPTF_NQueue_moveToTail", queueId);
  f_EPTF_NQueue_checkChainId("f_EPTF_NQueue_moveToTail", queueId, chainId);
  f_EPTF_NQueue_checkItemIdx("f_EPTF_NQueue_moveToTail", queueId, itemIdx);
  f_EPTF_NQueue_unchainItem(queueId, itemIdx);
  EPTF_NQueue_Item &item = EPTF_NQueue_queues[queueId].items[itemIdx];
  EPTF_NQueue_Chain &chain = EPTF_NQueue_queues[queueId].chains[chainId];
  if(chain.head < 0) { chain.head = chain.tail = itemIdx; }
  else {
    EPTF_NQueue_queues[queueId].items[chain.tail].next = itemIdx;
  }
  item.next = itemIdx;
  item.prev = chain.tail;
  item.chain = chainId;
  chain.tail = itemIdx;
  chain.itemCount++;
  #ifdef EPTF_NQUEUE_DEBUG  
  f_EPTF_NQueue_checkChainSize(queueId, chainId);
  #endif
}

// note: target chain is the chain of the item at pl__afterIdx
void f__EPTF__NQueue__moveAfter(
  const EPTF__NQueue__QueueId &pl__queue,
  const EPTF__NQueue__ItemIdx &pl__afterIdx, // index of the item in &pl__toChain after which to move item &pl__item
  const EPTF__NQueue__ItemIdx &pl__item)
{
  int queueId = (int)pl__queue;
  int afterIdx = (int)pl__afterIdx;
  int itemIdx = (int)pl__item;
  f_EPTF_NQueue_checkQueueId("f_EPTF_NQueue_moveAfter", queueId);
  f_EPTF_NQueue_checkItemIdx("f_EPTF_NQueue_moveAfter", queueId, itemIdx);
  f_EPTF_NQueue_checkItemIdx("f_EPTF_NQueue_moveAfter", queueId, afterIdx);
  if(itemIdx == afterIdx) { return; }
  EPTF_NQueue_Item &item = EPTF_NQueue_queues[queueId].items[itemIdx];
  EPTF_NQueue_Item &after = EPTF_NQueue_queues[queueId].items[afterIdx];
  f_EPTF_NQueue_unchainItem(queueId, itemIdx);

  int chainId = after.chain;
  item.chain = chainId;
  EPTF_NQueue_Chain &chain = EPTF_NQueue_queues[queueId].chains[chainId];

  if(afterIdx == chain.tail) {
    chain.tail = itemIdx;
    item.next = itemIdx;
  } else {
    item.next = after.next;
    EPTF_NQueue_queues[queueId].items[after.next].prev = itemIdx;
  }
  item.prev = afterIdx;
  chain.itemCount++;
  after.next = itemIdx;
  #ifdef EPTF_NQUEUE_DEBUG  
  f_EPTF_NQueue_checkChainSize(queueId, chainId);
  #endif
}

// note: target chain is the chain of the item at pl__beforeIdx
void f__EPTF__NQueue__moveBefore(
  const EPTF__NQueue__QueueId &pl__queue,
  const EPTF__NQueue__ItemIdx &pl__beforeIdx, // index of the item in &pl__toChain before which to move item &pl__item
  const EPTF__NQueue__ItemIdx &pl__item)
{
  int queueId = (int)pl__queue;
  int beforeIdx = (int)pl__beforeIdx;
  int itemIdx = (int)pl__item;
  f_EPTF_NQueue_checkQueueId("f_EPTF_NQueue_moveBefore", queueId);
  f_EPTF_NQueue_checkItemIdx("f_EPTF_NQueue_moveBefore", queueId, itemIdx);
  f_EPTF_NQueue_checkItemIdx("f_EPTF_NQueue_moveBefore", queueId, beforeIdx);
  if(itemIdx == beforeIdx) { return; }
  EPTF_NQueue_Item &item = EPTF_NQueue_queues[queueId].items[itemIdx];
  EPTF_NQueue_Item &before = EPTF_NQueue_queues[queueId].items[beforeIdx];
  f_EPTF_NQueue_unchainItem(queueId, itemIdx);

  int chainId = before.chain;
  item.chain = chainId;
  EPTF_NQueue_Chain &chain = EPTF_NQueue_queues[queueId].chains[chainId];

  if(beforeIdx == chain.head) {
    chain.head = itemIdx;
    item.prev = itemIdx;
  } else {
    item.prev = before.prev;
    EPTF_NQueue_queues[queueId].items[before.prev].next = itemIdx;
  }
  item.next = beforeIdx;
  chain.itemCount++;
  before.prev = itemIdx;
  #ifdef EPTF_NQUEUE_DEBUG  
  f_EPTF_NQueue_checkChainSize(queueId, chainId);
  #endif
}

EPTF__NQueue__ChainId f__EPTF__NQueue__getChainOfItem(
  const EPTF__NQueue__QueueId &pl__queue,
  const EPTF__NQueue__ItemIdx &pl__item)
{
  int queueId = (int)pl__queue;
  int itemIdx = (int)pl__item;
  f_EPTF_NQueue_checkQueueId("f_EPTF_NQueue_getChainOfItem", queueId);
  f_EPTF_NQueue_checkItemIdx("f_EPTF_NQueue_getChainOfItem", queueId, itemIdx);
  return EPTF__NQueue__ChainId(EPTF_NQueue_queues[queueId].items[itemIdx].chain);
}

INTEGER f__EPTF__NQueue__getLengthOfQueue(
  const EPTF__NQueue__QueueId &pl__queue)
{
  int queueId = (int)pl__queue;
  f_EPTF_NQueue_checkQueueId("f_EPTF_NQueue_getLengthOfQueue", queueId);
  return INTEGER(EPTF_NQueue_queues[queueId].itemCount);
}

INTEGER f__EPTF__NQueue__getLengthOfChain(
  const EPTF__NQueue__QueueId &pl__queue,
  const EPTF__NQueue__ChainId &pl__chain)
{
  int queueId = (int)pl__queue;
  int chainId = (int)pl__chain;
  f_EPTF_NQueue_checkQueueId("f_EPTF_NQueue_getLengthOfChain", queueId);
  f_EPTF_NQueue_checkChainId("f_EPTF_NQueue_getLengthOfChain", queueId, chainId);
  return INTEGER(EPTF_NQueue_queues[queueId].chains[chainId].itemCount);
}

EPTF__NQueue__ItemIdx f__EPTF__NQueue__getHeadOfChain(
  const EPTF__NQueue__QueueId &pl__queue,
  const EPTF__NQueue__ChainId &pl__chain)
{
  int queueId = (int)pl__queue;
  int chainId = (int)pl__chain;
  f_EPTF_NQueue_checkQueueId("f_EPTF_NQueue_getHeadOfChain", queueId);
  f_EPTF_NQueue_checkChainId("f_EPTF_NQueue_getHeadOfChain", queueId, chainId);
  return INTEGER(EPTF_NQueue_queues[queueId].chains[chainId].head);
}

EPTF__NQueue__ItemIdx f__EPTF__NQueue__getTailOfChain(
  const EPTF__NQueue__QueueId &pl__queue,
  const EPTF__NQueue__ChainId &pl__chain)
{
  int queueId = (int)pl__queue;
  int chainId = (int)pl__chain;
  f_EPTF_NQueue_checkQueueId("f_EPTF_NQueue_getTailOfChain", queueId);
  f_EPTF_NQueue_checkChainId("f_EPTF_NQueue_getTailOfChain", queueId, chainId);
  return INTEGER(EPTF_NQueue_queues[queueId].chains[chainId].tail);
}

BOOLEAN f__EPTF__NQueue__getNextItemIdx(
  const EPTF__NQueue__QueueId &pl__queue,
  EPTF__NQueue__ItemIdx &pl__item) // changed to next item index if any
{
  int queueId = (int)pl__queue;
  int itemIdx = (int)pl__item;
  f_EPTF_NQueue_checkQueueId("f_EPTF_NQueue_getNextItemIdx", queueId);
  f_EPTF_NQueue_checkItemIdx("f_EPTF_NQueue_getNextItemIdx", queueId, itemIdx);
  if(itemIdx == EPTF_NQueue_queues[queueId].items[itemIdx].next) return BOOLEAN(FALSE);
  pl__item = EPTF_NQueue_queues[queueId].items[itemIdx].next;
  return BOOLEAN(TRUE);
}

BOOLEAN f__EPTF__NQueue__getPrevItemIdx(
  const EPTF__NQueue__QueueId &pl__queue,
  EPTF__NQueue__ItemIdx &pl__item) // changed to previous item index if any
{
  int queueId = (int)pl__queue;
  int itemIdx = (int)pl__item;
  f_EPTF_NQueue_checkQueueId("f_EPTF_NQueue_getPrevItemIdx", queueId);
  f_EPTF_NQueue_checkItemIdx("f_EPTF_NQueue_getPrevItemIdx", queueId, itemIdx);
  if(itemIdx == EPTF_NQueue_queues[queueId].items[itemIdx].prev) return BOOLEAN(FALSE);
  pl__item = EPTF_NQueue_queues[queueId].items[itemIdx].prev;
  return BOOLEAN(TRUE);
}

void f_EPTF_NQueue_logQueue(int queueId)
{
//  TTCN_Logger::log(TTCN_USER, "f_EPTF_NQueue_logQueue: queue %d: { items = %p, chain = %p, chainCount = %d, itemCount = %d, itemAllocCount = %d, deleted = %s }",
  #ifdef EPTF_NQUEUE_DEBUG
  TTCN_Logger::log(TTCN_USER, "f_EPTF_NQueue_logQueue: queue %d: { items = %p, chain = %p, chainCount = %d, itemCount = %d, itemAllocCount = %d, maxItemCount = %d, maxChainCount = %d }",
    queueId,
    EPTF_NQueue_queues[queueId].items,
    EPTF_NQueue_queues[queueId].chains,
    EPTF_NQueue_queues[queueId].chainCount,
    EPTF_NQueue_queues[queueId].itemCount,
    EPTF_NQueue_queues[queueId].itemAllocCount,
    EPTF_NQueue_queues[queueId].maxItemCount,
    EPTF_NQueue_queues[queueId].maxChainCount/*,
    EPTF_NQueue_queues[queueId].deleted?"true":"false"*/);
  #else
  TTCN_Logger::log(TTCN_USER, "f_EPTF_NQueue_logQueue: queue %d: { items = %p, chain = %p, chainCount = %d, itemCount = %d, itemAllocCount = %d }",
    queueId,
    EPTF_NQueue_queues[queueId].items,
    EPTF_NQueue_queues[queueId].chains,
    EPTF_NQueue_queues[queueId].chainCount,
    EPTF_NQueue_queues[queueId].itemCount,
    EPTF_NQueue_queues[queueId].itemAllocCount/*,
    EPTF_NQueue_queues[queueId].deleted?"true":"false"*/);
  #endif  
  for(int i=0; i<EPTF_NQueue_queues[queueId].chainCount; i++) {
    f_EPTF_NQueue_logChain(queueId, i);
  }
}

void f__EPTF__NQueue__logQueue(
  const EPTF__NQueue__QueueId &pl__queue)
{
  int queueId = (int)pl__queue;
  f_EPTF_NQueue_checkQueueId("f_EPTF_NQueue_logQueue", queueId);
  
  f_EPTF_NQueue_logQueue(queueId);
}

void f_EPTF_NQueue_logChain(int queueId, int chainId)
{
  #ifdef EPTF_NQUEUE_DEBUG
  TTCN_Logger::log(TTCN_USER, "f_EPTF_NQueue_logChain: queue %d, chain %d: { head = %d, tail = %d, itemCount = %d, maxItemCount = %d }",
    queueId, chainId, EPTF_NQueue_queues[queueId].chains[chainId].head, EPTF_NQueue_queues[queueId].chains[chainId].tail,
    EPTF_NQueue_queues[queueId].chains[chainId].itemCount, EPTF_NQueue_queues[queueId].chains[chainId].maxItemCount );
  #else
  TTCN_Logger::log(TTCN_USER, "f_EPTF_NQueue_logChain: queue %d, chain %d: { head = %d, tail = %d, itemCount = %d }",
    queueId, chainId, EPTF_NQueue_queues[queueId].chains[chainId].head, EPTF_NQueue_queues[queueId].chains[chainId].tail,
    EPTF_NQueue_queues[queueId].chains[chainId].itemCount);
  #endif  
  if(EPTF_NQueue_queues[queueId].chains[chainId].itemCount > 0) {
    int itemIdx = EPTF_NQueue_queues[queueId].chains[chainId].head;
    f_EPTF_NQueue_checkItemIdx("f_EPTF_NQueue_logChain", queueId, itemIdx);
    int count = EPTF_NQueue_queues[queueId].chains[chainId].itemCount;
    while(count--) {
      TTCN_Logger::log(TTCN_USER, "f_EPTF_NQueue_logChain: item at index %d: { next = %d, prev = %d, chain = %d }",
        itemIdx,
        EPTF_NQueue_queues[queueId].items[itemIdx].next,
        EPTF_NQueue_queues[queueId].items[itemIdx].prev,
        EPTF_NQueue_queues[queueId].items[itemIdx].chain);
      if(EPTF_NQueue_queues[queueId].items[itemIdx].next == itemIdx) break;
      itemIdx = EPTF_NQueue_queues[queueId].items[itemIdx].next;
      f_EPTF_NQueue_checkItemIdx("f_EPTF_NQueue_logChain", queueId, itemIdx);
    }
  }
}

void f__EPTF__NQueue__logChain(
  const EPTF__NQueue__QueueId &pl__queue,
  const EPTF__NQueue__ChainId &pl__chain)
{
  int queueId = (int)pl__queue;
  int chainId = (int)pl__chain;
  f_EPTF_NQueue_checkQueueId("f_EPTF_NQueue_logChain", queueId);
  f_EPTF_NQueue_checkChainId("f_EPTF_NQueue_logChain", queueId, chainId);

  f_EPTF_NQueue_logChain(queueId, chainId);
}

void f_EPTF_NQueue_logChainFields(int queueId, int chainId)
{
  TTCN_Logger::log(TTCN_USER, "f_EPTF_NQueue_logChainFields: queue %d, chain %d: { head = %d, tail = %d, itemCount = %d }",
    queueId, chainId, EPTF_NQueue_queues[queueId].chains[chainId].head, EPTF_NQueue_queues[queueId].chains[chainId].tail,
    EPTF_NQueue_queues[queueId].chains[chainId].itemCount);
}

void f__EPTF__NQueue__logChainFields(
  const EPTF__NQueue__QueueId &pl__queue,
  const EPTF__NQueue__ChainId &pl__chain)
{
  int queueId = (int)pl__queue;
  int chainId = (int)pl__chain;
  f_EPTF_NQueue_checkQueueId("f_EPTF_NQueue_logChainFields", queueId);
  f_EPTF_NQueue_checkChainId("f_EPTF_NQueue_logChainFields", queueId, chainId);

  f_EPTF_NQueue_logChainFields(queueId, chainId);
}

void dump_item(FILE *fp, int queueId, int itemIdx)
{
  fprintf(fp, "node%d [label=\"%d\", shape=box]\n", itemIdx, itemIdx);
  if(EPTF_NQueue_queues[queueId].items[itemIdx].prev >= 0)
    fprintf(fp, "node%d->node%d [label=\"p\"]\n", itemIdx, EPTF_NQueue_queues[queueId].items[itemIdx].prev);
  if(EPTF_NQueue_queues[queueId].items[itemIdx].next >= 0)
    fprintf(fp, "node%d->node%d [label=\"n\"]\n", itemIdx, EPTF_NQueue_queues[queueId].items[itemIdx].next);
}

void dump_chain(FILE*fp, int queueId, int chainId)
{
  int iter = EPTF_NQueue_queues[queueId].chains[chainId].head;
  fprintf(fp, "chain%d [label=\"chain %d (len: %d)\", shape=box]\n",
    chainId, chainId, EPTF_NQueue_queues[queueId].chains[chainId].itemCount);
  if(EPTF_NQueue_queues[queueId].chains[chainId].itemCount) {
    fprintf(fp, "chain%d->node%d [style=dashed, label=\"h\"]\n", chainId, EPTF_NQueue_queues[queueId].chains[chainId].head);
    fprintf(fp, "chain%d->node%d [style=dashed, label=\"t\"]\n", chainId, EPTF_NQueue_queues[queueId].chains[chainId].tail);
    for(int i=0; i<EPTF_NQueue_queues[queueId].chains[chainId].itemCount; i++) {
      dump_item(fp, queueId, iter);
      iter = EPTF_NQueue_queues[queueId].items[iter].next;
    }
  }
}

void dump_queue_to_png(int queueId, const char *name) // name of the graph - ".dot" will be appended for filename
{
  char *fn_dot = new char[strlen(name) + 5];
  strcpy(fn_dot, name);
  strcat(fn_dot, ".dot");
  FILE *fp = fopen(fn_dot, "w");
  if(!fp) {
    TTCN_warning("could not open file %s for writing", fn_dot);
    delete []fn_dot;
    return;
  }
  char *fn_png = new char[strlen(name) + 5];
  strcpy(fn_png, name);
  strcat(fn_png, ".png");

  fprintf(fp, "digraph %s {\n", name);
  fprintf(fp, "rankdir=LR;\n"); // top to buttom layout, other options: BT, LR, RL
  if(EPTF_NQueue_queues[queueId].itemCount < 1) {
    fprintf(fp, "empty [shape=box]\n");
  } else {
/*    for(int i=0; i<EPTF_NQueue_queues[queueId].itemCount; i++) {
      dump_item(fp, queueId, i);
    }*/
    for(int i=0; i<EPTF_NQueue_queues[queueId].chainCount; i++) {
      dump_chain(fp, queueId, i);
    }
  }
  fprintf(fp, "}\n");
  fclose(fp);
  const char cmd[]="dot -Tpng %s > %s";
  char *tmp = new char[strlen(cmd) + strlen(fn_dot) + strlen(fn_png) + 1];
  sprintf(tmp, cmd, fn_dot, fn_png);
  system(tmp);
  
  delete []fn_dot;
  delete []fn_png;
  delete []tmp;
}

void f__EPTF__NQueue__dumpToPng(
  const EPTF__NQueue__QueueId &pl__queue,
  const CHARSTRING &pl__name)
{
  f_EPTF_NQueue_checkQueueId("f_EPTF_NQueue_dumpToPng", pl__queue);
  dump_queue_to_png(pl__queue, pl__name);
}

} // namespace EPTF____NQueue____Functions
