/******************************************************************************
 * Copyright (c) 2000-2020 Ericsson Telecom AB
 * All rights reserved. This program and the accompanying materials
 * are made available under the terms of the Eclipse Public License v2.0
 * which accompanies this distribution, and is available at
 * https://www.eclipse.org/org/documents/epl-2.0/EPL-2.0.html
 ******************************************************************************/
//  File:     EPTF_CLL_RBT_ExternalFunctions.cc
//  Rev:      <RnXnn>
//  Prodnr:   CNL 113 512
//  Updated:  2011-09-26
//  Contact:  http://ttcn.ericsson.se
///////////////////////////////////////////////////////////////////////////////

#include <math.h> // isgreater, isless

#include "TTCN3.hh"
#include "EPTF_CLL_Common_Definitions.hh"
#include "EPTF_CLL_RBT_Definitions.hh"
#include "EPTF_CLL_RBT_Functions.hh"

// EPTF_RBT_MINITEMGROWBY and EPTF_RBT_GROWBYSHIFT may be specified in makefile.
// The number of new items preallocated will be (assuming itemAllocCount is the old allocation count):
//  exp2(ceil(log2(itemAllocCount>>EPTF_RBT_GROWBYSHIFT))) i.e.
//    (itemAllocCount>>EPTF_RBT_GROWBYSHIFT) round up to the nearest base-2 exponent if
//    greater than EPTF_RBT_MINITEMGROWBY, otherwise EPTF_RBT_MINITEMGROWBY
#ifndef EPTF_RBT_MINITEMGROWBY
#define EPTF_RBT_MINITEMGROWBY 32
#endif

#ifndef EPTF_RBT_GROWBYSHIFT
#define EPTF_RBT_GROWBYSHIFT 7
#endif

#if EPTF_RBT_MINITEMGROWBY < 1
#error "EPTF_RBT_MINITEMGROWBY must be a positive integer number"
#endif
#if EPTF_RBT_GROWBYSHIFT < 1
#error "EPTF_RBT_GROWBYSHIFT must be a positive integer number"
#endif

#define FLAG_RED 1
#define FLAG_NODE 2 // node or (non-head) cluster element
#define FLAG_FREE 4 // item is in the free chain
#define FLAG_INVALID 8 // item is invalid, i.e. neither in the tree, nor in the free chain

#if (defined(EPTF_DEBUG) && !defined(EPTF_RBT_DEBUG))
#define EPTF_RBT_DEBUG
#endif

using namespace EPTF__CLL__Common__Definitions;
using namespace EPTF__CLL__RBT__Definitions;

namespace EPTF__CLL__RBT__Functions {

typedef enum {K_double=0, K_int=1, K_charstring=2} EPTF_RBT_KeyType; // currently supported key types
const char *EPTF_RBT_KeyTypeNames[] = { "float", "integer", "charstring" };

class EPTF_RBT_Base
{
protected:
  EPTF_RBT_KeyType keyType;

  char *name;
  int itemCount; // number of used allocated items (tree node, chain item and free chain together)
  int itemAllocCount; // number of allocated items
#ifdef EPTF_RBT_DEBUG
  int maxItemCount;
  double increaseRate;
#endif
  int root; // <0 -> empty
  int freeHead; // free chain works like a stack
  int freeCount; // number of free items
  int smallestKeyIndex; // <0 -> invalid

  EPTF_RBT_Base(EPTF_RBT_KeyType p_keyType, const char *pl_name);
public:
  virtual ~EPTF_RBT_Base();
  inline EPTF_RBT_KeyType getKeyType() const { return keyType; }
  inline const char *getName() const { return name; }

  virtual void removeItemFromTree(int itemIdx) = 0;
  virtual void putItemInFreeChain(int itemIdx) = 0;

  virtual int getSmallest() = 0;
  inline int getRoot() const { return root; }
  virtual int getLeft(int itemIdx) const = 0;
  virtual int getRight(int itemIdx) const = 0;
  virtual int getParent(int itemIdx) const = 0;
  virtual int getNextInChain(int itemIdx) const = 0;
  virtual int getPrevInChain(int itemIdx) const = 0;
  virtual int iterateIncremental(int itemIdx) const = 0;
  virtual void sortIncremental(EPTF__RBT__ItemIdxList &items) = 0;

  inline int getItemCount() const { return itemCount; } // number of nodes + chain items + free items
  inline int getFreeCount() const { return freeCount; }
  inline int getTreeItemCount() const { return itemCount - freeCount; } // number of nodes + chain items

  virtual bool isTreeValid() = 0;

  virtual void dumpToPng(const char *name) = 0;

  virtual bool isItemNode(int itemIdx) const = 0;
  virtual bool isItemRed(int itemIdx) const = 0;
  virtual bool isItemFree(int itemIdx) const = 0;
  virtual bool isItemInvalid(int itemIdx) const = 0;
};

template <class TKey, class TKeyHelper>
class EPTF_RBT
 : public EPTF_RBT_Base
{
private:
  // items are stored in a linear "array", so each item has a unique id, its index
  // three types of items are stored in the linear "array": a node, a non-head cluster element or a free item
  // a node can be the head of a cluster
  // if the head of a cluster is removed, the next item has to become the node, so the parent node must point to that
  // the free chain contains only cluster items, bwd of head and fwd of tail are -1
  struct Item {
    int flags;
    TKey key;
    union {
      struct { // valid if flags&FLAG_NODE
        int left;
        int right;
        int parent;
        int fwd; // -1 if single element
        int endOfCluster; // self if single element
      } node;
      struct { // valid if !(flags&FLAG_NODE) (i.e. if in cluster but not the head, which is also the node in the tree)
        int startOfCluster; // Note: this is only valid if fwd < 0, i.e. if the item is the end of the chain! It is not updated for intermediate items!
        int fwd; // -1 if last
        int bwd;
      } clusterItem;
      struct { // valid if flags&FLAG_FREE
        int next; // free chain works like a stack
      } freeItem;
    };
  };

  Item *items;

  int findInsertionPoint(TKey key, int &insertionPoint);
  void insertItemHelper(int itemIdx, TKey key);
  void rotateRight(int aIdx);
  void rotateLeft(int aIdx);
  void fixRed(int nodeIdx);
  inline void swapIdx(int &i1, int &i2);
  void updateParent(int itemIdx, int newChildIdx);
  void swapNodes(int itemIdx, int successorIdx);
  void fixRemove(int itemIdx);
  void removeNodeWithOneOrNoChild(int itemIdx);
  void removeNodeWithTwoChildren(int itemIdx);
  inline int findSmallestFrom(int idx) const;
  inline int getNodeWithNextBiggerKey(int itemIdx) const;

  int black_depth;
  bool checkNode(int itemIdx, int parentIdx, int depth);
  int countChainLength(int nodeIdx);

  int dumpNode(FILE *fp, int nodeIdx);

  int allocItem();
  
public:
  EPTF_RBT(EPTF_RBT_KeyType p_keyType, const char *pl_name);
  EPTF_RBT(const EPTF_RBT &other);
  ~EPTF_RBT();

  int insertItem(TKey key);

  int insertItem(TKey key, bool isRed, int parentItemIdx); // only for testing!!!

  void removeItemFromTree(int itemIdx); // does not put it in the free chain, also use putItemInFreeChain!
  void putItemInFreeChain(int itemIdx);

  int getSmallest();
  TKey getKey(int itemIdx);
  int findByKey(TKey key);
  int getLeft(int itemIdx) const;
  int getRight(int itemIdx) const;
  int getParent(int itemIdx) const;
  int getNextInChain(int itemIdx) const;
  int getPrevInChain(int itemIdx) const;

  int iterateIncremental(int itemIdx) const;
  void sortIncremental(EPTF__RBT__ItemIdxList &items);

  bool isTreeValid();
  void dumpToPng(const char *name);

  bool isItemNode(int itemIdx) const;
  bool isItemRed(int itemIdx) const;
  bool isItemFree(int itemIdx) const;
  bool isItemInvalid(int itemIdx) const;
};

#include <stdio.h>
#include <stdarg.h>
void f_EPTF_RBT_error(const char *msg, ...)
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

///////////////////////////////////////////////////////////////////////////////
// EPTF_RBT_Base
///////////////////////////////////////////////////////////////////////////////

EPTF_RBT_Base::EPTF_RBT_Base(EPTF_RBT_KeyType p_keyType, const char *pl_name)
  : keyType(p_keyType),
    name(NULL),
    itemCount(0),
    itemAllocCount(0),
    root(-1),
    freeHead(-1),
    freeCount(0),
    smallestKeyIndex(-1)
#ifdef EPTF_RBT_DEBUG
    ,maxItemCount( (int)EPTF__CLL__Common__Definitions::tsp__CLL__debug__acceptableMaxSizeOfGrowingVariables )
    ,increaseRate( (double)EPTF__CLL__Common__Definitions::tsp__CLL__debug__increasePercentage4AcceptableMaxSize )
#endif
{
  if(pl_name) {
    name = new char[strlen(pl_name) + 1];
    strcpy(name, pl_name);
  }
#ifdef EPTF_RBT_DEBUG //corrigate the parameters if the growth is less than 1
  if( maxItemCount>=0 && increaseRate>=0 && ((double)maxItemCount)*increaseRate<1.0 )
  {
    if(increaseRate == 0)
    {
      increaseRate=0.5;
    }
    maxItemCount = (int)(1.0/increaseRate + 1.0);
  }
#endif  
}

EPTF_RBT_Base::~EPTF_RBT_Base()
{
  if(name) delete []name;
}

///////////////////////////////////////////////////////////////////////////////
// EPTF_RBT
///////////////////////////////////////////////////////////////////////////////

template <class TKey, class TKeyHelper>
EPTF_RBT<TKey,TKeyHelper>::EPTF_RBT(
  EPTF_RBT_KeyType p_keyType,
  const char *pl_name)
  : EPTF_RBT_Base(p_keyType, pl_name),
    items(NULL)
{
}

template <class TKey, class TKeyHelper>
EPTF_RBT<TKey,TKeyHelper>::EPTF_RBT(const EPTF_RBT &other)
  : EPTF_RBT_Base(other.keyType, other.name)
{
  itemCount = other.itemCount;
  itemAllocCount = other.itemAllocCount;
  root = other.root;
  freeHead = other.freeHead;
  freeCount = other.freeCount;
  smallestKeyIndex =   other.smallestKeyIndex;
  items = (Item*) Malloc(other.itemAllocCount * sizeof(Item));
  memcpy(items, other.items, other.itemCount * sizeof(Item));
}

template <class TKey, class TKeyHelper>
EPTF_RBT<TKey,TKeyHelper>::~EPTF_RBT()
{
  for(int i=0;i<itemCount;i++) {
    TKeyHelper::dealloc(items[i].key);
  }
  if(items) Free(items);
  items = NULL;
}

template <class TKey, class TKeyHelper>
int EPTF_RBT<TKey,TKeyHelper>::findInsertionPoint(TKey key, int &insertionPoint)
{
  insertionPoint=root;
  // TODO: assert: root must be >= 0
  int cmp_res;
  for(;;) {
    cmp_res = TKeyHelper::compare(key, items[insertionPoint].key);
    if(cmp_res > 0) {
      if(items[insertionPoint].node.right >= 0) {
        insertionPoint = items[insertionPoint].node.right;
      } else break;
    } else if(cmp_res <0) {
      if(items[insertionPoint].node.left >= 0) {
        insertionPoint = items[insertionPoint].node.left;
      } else break;
    } else {
      break;
    }
  }
  return cmp_res;
}

template <class TKey, class TKeyHelper>
void EPTF_RBT<TKey,TKeyHelper>::insertItemHelper(int itemIdx, TKey key)
{
  Item *item = items + itemIdx;
//  item->key = key;
  item->key = TKeyHelper::clone(key);
  if(root < 0) { // empty tree
    root = itemIdx;
    item->flags = FLAG_NODE; // root node is black
    item->node.left = -1;
    item->node.right = -1;
    item->node.parent = -1;
    item->node.fwd = -1;
    item->node.endOfCluster = itemIdx;
  } else {
    int insertionPoint;
    int cmp_res = findInsertionPoint(key, insertionPoint);

    // note: in this context (compared to the wikipedia) node is the parent node when inserting item as the new node,
    //  node.parent is the grandparent of the new item, node's sibling is item's uncle
    Item *node = items + insertionPoint;
    if(cmp_res > 0) { // key > node->key
      // add as right leaf
      item->flags = FLAG_NODE | FLAG_RED;
      item->node.left = -1;
      item->node.right = -1;
      item->node.parent = insertionPoint;
      item->node.fwd = -1;
      item->node.endOfCluster = itemIdx;
      node->node.right = itemIdx;

      if(node->flags & FLAG_RED) { // fix red violation (item and its parent, 'node' are both red)
        fixRed(itemIdx);
      }
//      } else if(TKeyHelper::isLower(key, node->key)) {
    } else if(cmp_res < 0) { // key < node->key
      // add as left leaf
      item->flags = FLAG_NODE | FLAG_RED;
      item->node.left = -1;
      item->node.right = -1;
      item->node.parent = insertionPoint;
      item->node.fwd = -1;
      item->node.endOfCluster = itemIdx;
      node->node.left = itemIdx;

      if(node->flags & FLAG_RED) { // fix red violation (item and its parent, 'node' are both red)
        fixRed(itemIdx);
      }
    } else { // key == node.key
      // add as cluster item
      item->flags = 0;
      item->clusterItem.startOfCluster = insertionPoint;
      item->clusterItem.fwd = -1;
      if(node->node.fwd < 0) { // empty chain, append to node (chain head)
        node->node.fwd = itemIdx;
        item->clusterItem.bwd = insertionPoint;
      } else { // non-empty chain, append to end of chain
        items[node->node.endOfCluster].clusterItem.fwd = itemIdx;
        item->clusterItem.bwd = node->node.endOfCluster;
      }
      node->node.endOfCluster = itemIdx;
    }
  }
}

/*  |             |
    a             b
   / \           / \
  b   x1   ->   x2  a
 / \               / \
x2 x3             x3 x1   */
template <class TKey, class TKeyHelper>
void EPTF_RBT<TKey,TKeyHelper>::rotateRight(int aIdx)
{
  Item *a = items + aIdx;
  int bIdx = a->node.left;
  if(bIdx < 0) f_EPTF_RBT_error("EPTF_RBT::rotateRight: node has no left child");
  Item *b = items + bIdx;
  if(aIdx == root) { // FIXME: use updateParent?
    root = bIdx;
  } else {
    Item *r = items + a->node.parent;
    if(r->node.left == aIdx) {
      r->node.left = bIdx;
    } else {
      r->node.right = bIdx;
    }
  }
  b->node.parent = a->node.parent;
  a->node.parent = bIdx;
  a->node.left = b->node.right;
  if(b->node.right >= 0) items[b->node.right].node.parent = aIdx;
  b->node.right = aIdx;
}

/*  |              |
    a              b
   / \            / \
  x1  b    ->    a  x3
     / \        / \
    x2 x3      x1 x2      */
template <class TKey, class TKeyHelper>
void EPTF_RBT<TKey,TKeyHelper>::rotateLeft(int aIdx)
{
  Item *a = items + aIdx;
  int bIdx = a->node.right;
  if(bIdx < 0) f_EPTF_RBT_error("EPTF_RBT::rotateLeft: node has no right child");
  Item *b = items + bIdx;
  if(aIdx == root) { // FIXME: use updateParent?
    root = bIdx;
  } else {
    Item *r = items + a->node.parent;
    if(r->node.left == aIdx) {
      r->node.left = bIdx;
    } else {
      r->node.right = bIdx;
    }
  }
  b->node.parent = a->node.parent;
  a->node.parent = bIdx;
  a->node.right = b->node.left;
  if(b->node.left >= 0) items[b->node.left].node.parent = aIdx;
  b->node.left = aIdx;
}

template <class TKey, class TKeyHelper>
void EPTF_RBT<TKey,TKeyHelper>::fixRed(int nodeIdx)
{
  for(;;) {
    Item *node = items + nodeIdx;
    int parentIdx = node->node.parent;
    if(parentIdx < 0) { // nodeIdx is root
      node->flags &= ~FLAG_RED;
      break;
    }
    Item *parent = items + parentIdx;

    if((parent->flags&FLAG_RED) == 0) break; // parent is black node

    int grandparentIdx = parent->node.parent;
    if(grandparentIdx < 0) { // parent is root
      parent->flags &= ~FLAG_RED;
      break;
    }
    Item *grandparent = items + grandparentIdx;

    bool leftparent = false;
    int uncleIdx = grandparent->node.left;
    if(uncleIdx == parentIdx) {
      uncleIdx = grandparent->node.right;
      leftparent = true;
    }

    if(uncleIdx >= 0) {
         // note: parent is red at this point (there's a return/break earlier if parent is black)
      if(//(parent->flags&FLAG_RED) &&
          (items[uncleIdx].flags&FLAG_RED)) {
        grandparent->flags |= FLAG_RED;
        parent->flags &= ~FLAG_RED;
        items[uncleIdx].flags &= ~FLAG_RED;

        nodeIdx = grandparentIdx;
        continue;

      }
    }

    // note: at this point both node and parent are red, but uncle is black
    if(leftparent) {
      if(parent->node.left == nodeIdx) { // node is left child of parent - A
        // rotate parent-grandparent
        rotateRight(grandparentIdx);
        parent->flags &= ~FLAG_RED;
        grandparent->flags |= FLAG_RED;
      } else { // node is right child of parent - B
        // note: the two rotation are equivalent to the first one (rotate node-parent) then calling fixRed for parent
        rotateLeft(parentIdx);
        rotateRight(grandparentIdx);
        node->flags &= ~FLAG_RED;
        grandparent->flags |= FLAG_RED;
      }
    } else {
      if(parent->node.left == nodeIdx) { // node is left child of parent - mirror of B
        rotateRight(parentIdx);
        rotateLeft(grandparentIdx);
        node->flags &= ~FLAG_RED;
        grandparent->flags |= FLAG_RED;
      } else { // node is right child of parent - mirror of A
        // rotate parent-grandparent
        rotateLeft(grandparentIdx);
        parent->flags &= ~FLAG_RED;
        grandparent->flags |= FLAG_RED;
      }
    }

    break;

  }
}

template <class TKey, class TKeyHelper>
void EPTF_RBT<TKey,TKeyHelper>::swapIdx(int &i1, int &i2)
{
  int tmp = i1;
  i1 = i2;
  i2 = tmp;
}

template <class TKey, class TKeyHelper>
void EPTF_RBT<TKey,TKeyHelper>::updateParent(int itemIdx, int newChildIdx)
{
  int parentIdx = items[itemIdx].node.parent;
  if(parentIdx >= 0) {
    if(items[parentIdx].node.left == itemIdx) {
      items[parentIdx].node.left = newChildIdx;
    } else {
      items[parentIdx].node.right = newChildIdx;
    }
  } else { // no parent - root node
#ifdef EPTF_DEBUG
    if(itemIdx != root) {
      f_EPTF_RBT_error("EPTF_RBT::updateParent: parent index of item is -1 but it is not the root node.");
    }
#endif
    root = newChildIdx;
  }
}

template <class TKey, class TKeyHelper>
void EPTF_RBT<TKey,TKeyHelper>::swapNodes(int itemIdx, int successorIdx)
{
  Item *item = items + itemIdx;
  Item *successor = items + successorIdx;

  // update parents
  updateParent(itemIdx, successorIdx); // NOTE: this DOES NOT update successor->node.parent!
  updateParent(successorIdx, itemIdx);

  // update nodes that are swapped
  swapIdx(item->node.parent, successor->node.parent);
  swapIdx(item->node.left, successor->node.left);
  swapIdx(item->node.right, successor->node.right);

  // swap the RED flag of the nodes
  int tmp = (item->flags & FLAG_RED) ^ (successor->flags & FLAG_RED);
  item->flags ^= tmp;
  successor->flags ^= tmp;
//  if(itemIdx == tree.root) tree.root = successorIdx; // done by updateParent
  // note: don't swap the chains! (item.node.fwd, successor.node.fwd)

  // update parent pointer of children of the nodes being swapped
  // note: left/right pointers of item and successor are already swapped
  if(item->node.left >= 0) items[item->node.left].node.parent = itemIdx;
  if(item->node.right >= 0) items[item->node.right].node.parent = itemIdx;
  if(successor->node.left >= 0) items[successor->node.left].node.parent = successorIdx;
  if(successor->node.right >= 0) items[successor->node.right].node.parent = successorIdx;
}

template <class TKey, class TKeyHelper>
void EPTF_RBT<TKey,TKeyHelper>::fixRemove(int itemIdx)
{
  for(;;) {
    Item *item = items + itemIdx;
    if(item->node.parent < 0) { // root node (case 1)
      break;
    } else {
      int parentIdx = item->node.parent;
      Item *parent = items + parentIdx;
      int siblingIdx = parent->node.left;
      bool siblingIsLeft = true;
      if(siblingIdx == itemIdx) {
        siblingIdx = parent->node.right;
        siblingIsLeft = false;
      }
      Item *sibling = items + siblingIdx;
      if(sibling->flags & FLAG_RED) { // (case 2)
        // swap the RED flag of the nodes
        int tmp = (sibling->flags & FLAG_RED) ^ (parent->flags & FLAG_RED);
        // FLAG_RED in tmp will be 1 if FLAG_RED differs in sibling and parent
        sibling->flags ^= tmp; // invert FLAG_RED if differs
        parent->flags ^= tmp; // invert FLAG_RED if differs
        // rotate sibling-parent
        if(siblingIsLeft) {
          rotateRight(parentIdx);
        } else {
          rotateLeft(parentIdx);
        }
        continue;
      } else { // note: sibling is already BLACK here
        // siblings left child exists and is red
        bool redSL = (sibling->node.left >= 0) && (items[sibling->node.left].flags & FLAG_RED);
        // siblings right child exists and is red
        bool redSR = (sibling->node.right >= 0) && (items[sibling->node.right].flags & FLAG_RED);
        if(!redSL && !redSR) { // both children of sibling is black (or nonexistent)
          if((parent->flags & FLAG_RED) == 0) {
            // parent, sibling and siblings both children are black -> repaint sibling red (case 3)
            sibling->flags |= FLAG_RED;
            itemIdx = parentIdx; // goto case 1 with parent
            continue;
          } else {
            // sibling and siblings both children are black but parent is red (case 4)
            // swap color of sibling and parent
            sibling->flags |= FLAG_RED;
            parent->flags &= ~FLAG_RED;
            break;
          }
        } else { // at least one of the siblings child is red

          if(siblingIsLeft) {
             if(redSR) { // case 5
              sibling->flags |= FLAG_RED;
              items[sibling->node.right].flags &= ~FLAG_RED;
              int tmp = sibling->node.right;
              rotateLeft(siblingIdx);
              // continue with case 6
              siblingIdx = tmp;
              sibling = items + siblingIdx;
              redSL = true;//(sibling->node.left >= 0) && (tree.items[sibling->node.left].flags & FLAG_RED);
            }
            if(redSL) { // case 6
              // sibling takes the color of parent, parent becomes red, siblings child becomes black, rotate parent
              int tmp = parent->flags & FLAG_RED;
              sibling->flags &= ~FLAG_RED;
              sibling->flags |= tmp;
              parent->flags &= ~FLAG_RED;
              items[sibling->node.left].flags &= ~FLAG_RED;
              rotateRight(parentIdx);
            }
          } else {
            if(redSL) { // case 5
              sibling->flags |= FLAG_RED;
              items[sibling->node.left].flags &= ~FLAG_RED;
              int tmp = sibling->node.left;
              rotateRight(siblingIdx);
              // continue with case 6
              siblingIdx = tmp;
              sibling = items + siblingIdx;
              redSR = true;//(sibling->node.right >= 0) && (items[sibling->node.right].flags & FLAG_RED);
            }
            if(redSR) { // case 6
              // sibling takes the color of parent, parent becomes red, siblings child becomes black, rotate parent
              int tmp = parent->flags & FLAG_RED;
              sibling->flags &= ~FLAG_RED;
              sibling->flags |= tmp;
              parent->flags &= ~FLAG_RED;
              items[sibling->node.right].flags &= ~FLAG_RED;
              rotateLeft(parentIdx);
            }
          }
          
          break;

        }
      }
    }
  }
}

template <class TKey, class TKeyHelper>
void EPTF_RBT<TKey,TKeyHelper>::removeNodeWithOneOrNoChild(int itemIdx)
{
  Item *item = items + itemIdx;
  int childIdx = item->node.left;
  if(childIdx < 0) childIdx = item->node.right; // can also be -1
  if(childIdx >= 0) { // has child
    Item *child = items + childIdx;
    if(item->flags & FLAG_RED || child->flags & FLAG_RED) {
      // replace item with its child and paint it black
      updateParent(itemIdx, childIdx);
//      updateParent(childIdx, -1); // parent of child is the node to be removed, so modifying it is not necessary
//      child->node.left = item->node.left; // NOTE: this is -1
//      child->node.right = item->node.right; // NOTE: this is -1
      child->node.parent = item->node.parent;
      child->flags &= ~FLAG_RED;
    } else { // both are black nodes - invalid tree
      f_EPTF_RBT_error("EPTF_RBT::removeNodeWithOneOrNoChild: removing black (non-leaf) node with *single* black (non-leaf) child - tree was invalid!");
    }
  } else { // no child
    if(item->flags & FLAG_RED) {
      // simply remove the red node
      updateParent(itemIdx, -1);
    } else { // black node
      fixRemove(itemIdx);
      updateParent(itemIdx, -1); // unlink the node from its parent
    }
  }
}

template <class TKey, class TKeyHelper>
void EPTF_RBT<TKey,TKeyHelper>::removeNodeWithTwoChildren(int itemIdx)
{
  static int lr = 0;
  lr ^= 1;
  int successorIdx;
  if(lr) { // choose left/right alternating each call
    successorIdx = items[itemIdx].node.left;
    while(items[successorIdx].node.right >= 0) successorIdx = items[successorIdx].node.right;
  } else {
    successorIdx = items[itemIdx].node.right;
    while(items[successorIdx].node.left >= 0) successorIdx = items[successorIdx].node.left;
  }
  // swap the two node, then delete the item (at its new position in tree - but same index!) with removeNodeWithOneOrNoChild
  swapNodes(itemIdx, successorIdx);
  removeNodeWithOneOrNoChild(itemIdx);
}

template <class TKey, class TKeyHelper>
int EPTF_RBT<TKey,TKeyHelper>::findSmallestFrom(int idx) const
{
  while(items[idx].node.left >= 0) {
    idx = items[idx].node.left;
  }
  return idx;
}

template <class TKey, class TKeyHelper>
int EPTF_RBT<TKey,TKeyHelper>::getNodeWithNextBiggerKey(int itemIdx) const
{
  if(items[itemIdx].node.right >= 0) { // has right child -> find smallest from right child
    return findSmallestFrom(items[itemIdx].node.right);
  } else {
    int p = items[itemIdx].node.parent;
    for(;;) { // find next ancestor on the right, i.e. that has bigger key (if any)
      if(p < 0) { return -1; }
      if(items[p].node.left == itemIdx) return p;
      itemIdx = p;
      p = items[p].node.parent;
    }
  }
}

template <class TKey, class TKeyHelper>
bool EPTF_RBT<TKey,TKeyHelper>::checkNode(int itemIdx, int parentIdx, int depth)
{
  if(itemIdx < 0) { // leaf
    if(black_depth < 0) {
      black_depth = depth;
      return true;
    } else if(depth != black_depth) {
      TTCN_Logger::log(TTCN_DEBUG, "f_EPTF_RBT_isTreeValid: depth of black nodes is unbalanced.");
      return false;
    }
    return true;
  }
  if((items[itemIdx].flags & FLAG_NODE) == 0) {
    TTCN_Logger::log(TTCN_DEBUG, "f_EPTF_RBT_isTreeValid: item is not node.");
    return false;
  }
  if(items[itemIdx].node.parent < 0 && root != itemIdx) {
    TTCN_Logger::log(TTCN_DEBUG, "f_EPTF_RBT_isTreeValid: parent of item %d is "
      "less than 0 (%d), but another item (%d) is root.",
      itemIdx, items[itemIdx].node.parent, root);
    return false;
  }
  if(itemIdx == root && items[itemIdx].node.parent >= 0) {
    TTCN_Logger::log(TTCN_DEBUG, "f_EPTF_RBT_isTreeValid: parent of root node (%d) is greater than 0 (%d)",
      itemIdx, items[itemIdx].node.parent);
    return false;
  }
  if(items[itemIdx].node.parent != parentIdx) {
    TTCN_Logger::log(TTCN_DEBUG, "f_EPTF_RBT_isTreeValid: parent reference of item %d is %d, shoud be %d.",
      itemIdx, items[itemIdx].node.parent, parentIdx);
    return false;
  }
  if(items[itemIdx].node.left >= 0 &&
//      (items[items[itemIdx].node.left].key > items[itemIdx].key) ){
      TKeyHelper::isGreater(items[items[itemIdx].node.left].key, items[itemIdx].key) ){
    TTCN_Logger::log(TTCN_DEBUG, "f_EPTF_RBT_isTreeValid: node's left child has greater key.");
    return false;
  }
  if(items[itemIdx].node.right >= 0 &&
//      (items[items[itemIdx].node.right].key < items[itemIdx].key) ){
      TKeyHelper::isLower(items[items[itemIdx].node.right].key, items[itemIdx].key) ){
    TTCN_Logger::log(TTCN_DEBUG, "f_EPTF_RBT_isTreeValid: node's right child has smaller key.");
    return false;
  }
  if(items[itemIdx].flags & FLAG_RED) { // red node, parent must be black (both children of every red node are black)
    if(items[items[itemIdx].node.parent].flags & FLAG_RED) {
      TTCN_Logger::log(TTCN_DEBUG, "f_EPTF_RBT_isTreeValid: red node has red parent.");
      return false;
    }
  } else { // black node -> increment depth
    depth++;
  }
  if(!checkNode(items[itemIdx].node.left, itemIdx, depth)) return false;
  if(!checkNode(items[itemIdx].node.right, itemIdx, depth)) return false;
  return true;
}

template <class TKey, class TKeyHelper>
int EPTF_RBT<TKey,TKeyHelper>::countChainLength(int nodeIdx)
{
  int len = 0;
  Item *item = items + nodeIdx;
  for(;;) {
    if(item->flags & FLAG_NODE) {
      if(item->node.fwd >= 0) {
        len++;
        item = items + item->node.fwd;
      } else {
        break;
      }
    } else {
      if(item->clusterItem.fwd >= 0) {
        len++;
        item = items + item->clusterItem.fwd;
      } else {
        break;
      }
    }
  }
  return len;
}


template <class TKey, class TKeyHelper>
int EPTF_RBT<TKey,TKeyHelper>::dumpNode(FILE *fp, int nodeIdx)
{
  if((items[nodeIdx].flags & FLAG_NODE) == 0) return -1;
  if(items[nodeIdx].node.parent >= 0) {
    fprintf(fp, "node%d->node%d\n", nodeIdx, items[nodeIdx].node.parent);
  }
  if(items[nodeIdx].flags & FLAG_RED) {
    fprintf(fp, "node%d [label=\"%d (", nodeIdx, nodeIdx);
    TKeyHelper::dumpKey(fp, items[nodeIdx].key);
    fprintf(fp, ")\", shape=box, color=red, style=filled, fontcolor=black]\n");
  } else {
    fprintf(fp, "node%d [label=\"%d (", nodeIdx, nodeIdx);
    TKeyHelper::dumpKey(fp, items[nodeIdx].key);
    fprintf(fp, ")\", shape=box, color=black, style=filled, fontcolor=white]\n");
  }
  if(items[nodeIdx].node.left >= 0) {
    if(items[nodeIdx].node.left == nodeIdx) {
      TTCN_Logger::log(TTCN_ERROR, "dump_tree_node: circular reference in node %d", nodeIdx);
      fprintf(fp, "node%d->node%d [color=green]\n", nodeIdx, nodeIdx);
    } else {
      int thatNode = dumpNode(fp, items[nodeIdx].node.left);
      if(thatNode < 0) {
        fprintf(fp, "linvalid%d [label=\"invalid\", shape=box, color=purple, style=filled, fontcolor=black]\n", nodeIdx);
        fprintf(fp, "node%d->linvalid%d  [color=green]\n", nodeIdx, nodeIdx);
      } else {
        fprintf(fp, "node%d->node%d [color=green]\n", nodeIdx, thatNode);
      }
    }
  } else {
    fprintf(fp, "lnull%d [label=\"null\", shape = box, fontsize=6, color=gray, fontcolor=gray, height=0.1, width=0.1]\n", nodeIdx);
    fprintf(fp, "node%d->lnull%d [color=green]\n", nodeIdx, nodeIdx);
  }
  int clusterLen = countChainLength(nodeIdx);
  if(clusterLen > 0) {
    fprintf(fp, "cluster%d [label=\"cluster (%d)\", shape=box, fontsize=6, color=blue, fontcolor=blue, height=0.1, width=0.1]\n",
      nodeIdx, clusterLen);
    fprintf(fp, "node%d->cluster%d\n", nodeIdx, nodeIdx);
  }
  if(items[nodeIdx].node.right >= 0) {
    if(items[nodeIdx].node.right == nodeIdx) {
      TTCN_Logger::log(TTCN_ERROR, "dump_tree_node: circular reference in node %d", nodeIdx);
      fprintf(fp, "node%d->node%d [color=blue]\n", nodeIdx, nodeIdx);
    } else {
      int thatNode = dumpNode(fp, items[nodeIdx].node.right);
      if(thatNode < 0) {
        fprintf(fp, "rinvalid%d [label=\"invalid\", shape=box, color=purple, style=filled, fontcolor=black]\n", nodeIdx);
        fprintf(fp, "node%d->rinvalid%d [color=blue]\n", nodeIdx, nodeIdx);
      } else {
        fprintf(fp, "node%d->node%d [color=blue]\n", nodeIdx, thatNode);
      }
    }
  } else {
    fprintf(fp, "rnull%d [label=\"null\", shape = box, fontsize=6, color=gray, fontcolor=gray, height=0.1, width=0.1]\n", nodeIdx);
    fprintf(fp, "node%d->rnull%d [color=blue]\n", nodeIdx, nodeIdx);
  }
  return nodeIdx;
}

template <class TKey, class TKeyHelper>
int EPTF_RBT<TKey,TKeyHelper>::allocItem()
{
  int itemIdx;
  if(itemAllocCount > itemCount) {
    // if there are any unused allocated items, use them first
    itemIdx = itemCount;
    itemCount++;
  } else if(freeHead >= 0) {
    // otherwise use the head of the free chain
    itemIdx = freeHead;
    items[itemIdx].flags &= ~FLAG_FREE;
    freeHead = items[itemIdx].freeItem.next; // can be -1 -> chain becomes empty
    freeCount--;
  } else {
    // if the free chain is empty, allocate more items

    int growby = EPTF_RBT_MINITEMGROWBY;
    int bits = 0;
    int tmp = itemAllocCount >> EPTF_RBT_GROWBYSHIFT;
    while(tmp) {
      bits++;
      tmp>>=1;
    }
    tmp = 1<<bits;
    if(tmp > growby) growby = tmp;
//    printf("itemAllocCount = %d\n",itemAllocCount);
//    printf("growby = %d\n", growby);
    itemAllocCount += growby;

    items = (Item*) Realloc(items, itemAllocCount*sizeof(Item));
    itemIdx = itemCount;
    itemCount++;
  }
  return itemIdx;
}

template <class TKey, class TKeyHelper>
int EPTF_RBT<TKey,TKeyHelper>::insertItem(TKey key)
{
  int itemIdx = allocItem();
#ifdef EPTF_RBT_DEBUG
  if(maxItemCount >= 0 && increaseRate >= 0 && itemCount > maxItemCount)
  {
    TTCN_warning("RBT: Tree: %s has exceeded the maximum item count: %d! New maxItemCount is %d.", getName(), maxItemCount, (int) (maxItemCount*(1+increaseRate)) );
    maxItemCount = (int) maxItemCount*(1+increaseRate);
  }
#endif
  // after this, start a tree insert, if that finds a node with the same key, chain it at the end of that chain,
  // otherwise insert as node and update the tree (set FLAG_NODE!)
  bool treeWasEmpty = root < 0;
  insertItemHelper(itemIdx, key);

  if( treeWasEmpty ||
     ((smallestKeyIndex >= 0) &&
      (items[itemIdx].flags&FLAG_NODE) &&
//      (items[smallestKeyIndex].key > key))) {
      TKeyHelper::isGreater(items[smallestKeyIndex].key, key))) {
    smallestKeyIndex = itemIdx;
  }

  return itemIdx;
}

template <class TKey, class TKeyHelper>
int EPTF_RBT<TKey,TKeyHelper>::insertItem(TKey key, bool isRed, int parentItemIdx) // only for testing!!!
{
  int itemIdx = allocItem();
  Item *item = items + itemIdx;
  item->flags = FLAG_NODE;
  item->key = TKeyHelper::clone(key);
  item->node.left = -1;
  item->node.right = -1;
  item->node.parent = parentItemIdx;
  item->node.fwd = -1;
  item->node.endOfCluster = itemIdx;
  if(parentItemIdx < 0) {
    root = itemIdx;
    smallestKeyIndex = itemIdx;
    if(isRed) {
      TTCN_Logger::log(TTCN_WARNING, "insertItem: root node cannot be red, iserting as black");
    }
  } else {
    if(isRed) {
      item->flags |= FLAG_RED;
    }
    Item *parent = items + parentItemIdx;
    int cmp_res = TKeyHelper::compare(item->key, parent->key);
    if(cmp_res < 0) {
      parent->node.left = itemIdx;
    } else if(cmp_res > 0) {
      parent->node.right = itemIdx;
    } else {
      f_EPTF_RBT_error("insertItem: key of child node must not equal key of node");
    }
    if(TKeyHelper::isLower(item->key, items[smallestKeyIndex].key)) {
      smallestKeyIndex = itemIdx;
    }
  }
  return itemIdx;
}

template <class TKey, class TKeyHelper>
void EPTF_RBT<TKey,TKeyHelper>::removeItemFromTree(int itemIdx)
{
  Item *item = items + itemIdx;
  if(item->flags & FLAG_NODE) {
    if(item->node.fwd >= 0) {

      // update end of cluster's startOfCluster to point at the new node, i.e. node.fwd
      Item *end = items + item->node.endOfCluster;
      end->clusterItem.startOfCluster = item->node.fwd;

      if(itemIdx == smallestKeyIndex) {
        smallestKeyIndex = item->node.fwd;
      }
      // set parent to point to next chain item
      updateParent(itemIdx, item->node.fwd);
      // change the next item to a node
      Item *next = items + item->node.fwd;
      int tmp = next->clusterItem.fwd;
      next->flags = item->flags;
      next->node.left = item->node.left;
      next->node.right = item->node.right;
      next->node.parent = item->node.parent;
      next->node.fwd = tmp;

#ifdef EPTF_RBT_DEBUG
      if(item->node.endOfCluster == itemIdx) { // this cannot happen, as this would mean empty chain, but item.node.fwd >= 0
        f_EPTF_RBT_error("EPTF_RBT::removeItemFromTree: item->node.endOfCluster == itemIdx, but item->node.fwd >= 0");
      } else {
#endif
        next->node.endOfCluster = item->node.endOfCluster;
#ifdef EPTF_RBT_DEBUG
      }
#endif

      if(next->node.left>=0) {
        items[next->node.left].node.parent = item->node.fwd;
      }
      if(next->node.right>=0) {
        items[next->node.right].node.parent = item->node.fwd;
      }
    } else {
      // otherwise remove node from tree and update tree
      if(itemIdx == smallestKeyIndex) {
        // note: smallestKeyIndex must be updated before removing item, because of tree rotations!
        if(item->node.left >= 0) {
          f_EPTF_RBT_error("EPTF_RBT::removeItemFromTree: item %d is smallest but has left child", itemIdx);
        }
        smallestKeyIndex = getNodeWithNextBiggerKey(smallestKeyIndex);
      }
      if(item->node.left == -1 || item->node.right == -1) {
        removeNodeWithOneOrNoChild(itemIdx);
      } else {
        removeNodeWithTwoChildren(itemIdx);
      }
    }
  } else {
    // unchain item
#ifdef EPTF_RBT_DEBUG
    if(itemIdx == smallestKeyIndex) { // this cannot happen, the smallest index should always point to a node
      f_EPTF_RBT_error("EPTF_RBT::removeItemFromTree: smallestKeyIndex points to cluster element");
    }
#endif
    // bwd should never be <0, because that would be the chain head, i.e. the node item itself
    Item *prev = items + item->clusterItem.bwd;
    if(item->clusterItem.fwd < 0) { // item is end of chain
      Item *node = items + item->clusterItem.startOfCluster; // startOfCluster is valid (may not be the case for intermediate items !!!!)
      node->node.endOfCluster = item->clusterItem.bwd;
    }
    if(prev->flags & FLAG_NODE) {
      prev->node.fwd = item->clusterItem.fwd;
    } else {
      if(item->clusterItem.fwd < 0) { // item is end of chain -> update prev's startOfCluster to point to the node
        prev->clusterItem.startOfCluster = item->clusterItem.startOfCluster;
      }
      prev->clusterItem.fwd = item->clusterItem.fwd;
    }
    if(item->clusterItem.fwd >= 0) {
      Item *next = items + item->clusterItem.fwd;
      next->clusterItem.bwd = item->clusterItem.bwd;
    }
  }
  item->flags = FLAG_INVALID;
}

template <class TKey, class TKeyHelper>
void EPTF_RBT<TKey,TKeyHelper>::putItemInFreeChain(int itemIdx)
{
  // put item in free chain - item MUST be removed from the tree previously!
  Item *item = items + itemIdx;
  TKeyHelper::dealloc(item->key);
  item->flags = FLAG_FREE;
  item->freeItem.next = freeHead;
  freeHead = itemIdx;
  freeCount++;
}


template <class TKey, class TKeyHelper>
int EPTF_RBT<TKey,TKeyHelper>::getSmallest()
{
/*  if(smallestKeyIndex < 0) {
    if(root < 0) { // empty tree
      return -1;
    }
    // update smallestKeyIndex (actually, smallestKeyIndex is maintained in a way that this should never be performed)
    smallestKeyIndex = findSmallestFrom(root);
  }*/ // Note: if smallestKeyIndex < 0 the tree is empty
  return smallestKeyIndex;
}

template <class TKey, class TKeyHelper>
TKey EPTF_RBT<TKey,TKeyHelper>::getKey(int itemIdx)
{
  return items[itemIdx].key;
}

template <class TKey, class TKeyHelper>
int EPTF_RBT<TKey,TKeyHelper>::findByKey(TKey key)
{
  for(int i = root; i >= 0; ) {
    int cmp_res = TKeyHelper::compare(key, items[i].key);
//    if(key < items[i].key) {
//    if(TKeyHelper::isLower(key, items[i].key)) {
    if(cmp_res < 0) { // key < items[i].key
      i = items[i].node.left;
//    } else if(key > items[i].key) {
//    } else if(TKeyHelper::isGreater(key, items[i].key)) {
    } else if(cmp_res > 0) { // key > items[i].key
      i = items[i].node.right;
    } else {
      return i;
    }
  }
  return -1;
}

template <class TKey, class TKeyHelper>
int EPTF_RBT<TKey,TKeyHelper>::getLeft(int itemIdx) const
{
  if(items[itemIdx].flags & FLAG_NODE) { return items[itemIdx].node.left; }
  else { return -1; }
}

template <class TKey, class TKeyHelper>
int EPTF_RBT<TKey,TKeyHelper>::getRight(int itemIdx) const
{
  if(items[itemIdx].flags & FLAG_NODE) { return items[itemIdx].node.right; }
  else { return -1; }
}

template <class TKey, class TKeyHelper>
int EPTF_RBT<TKey,TKeyHelper>::getParent(int itemIdx) const
{
  if(items[itemIdx].flags & FLAG_NODE) { return items[itemIdx].node.parent; }
  else { return -1; }
}

template <class TKey, class TKeyHelper>
int EPTF_RBT<TKey,TKeyHelper>::getNextInChain(int itemIdx) const
{
  if(items[itemIdx].flags & FLAG_NODE) { return items[itemIdx].node.fwd; }
  else { return items[itemIdx].clusterItem.fwd; }
}

template <class TKey, class TKeyHelper>
int EPTF_RBT<TKey,TKeyHelper>::getPrevInChain(int itemIdx) const
{
  if(items[itemIdx].flags & FLAG_NODE) { return -1; }
  else { return items[itemIdx].clusterItem.bwd; }
}

template <class TKey, class TKeyHelper>
int EPTF_RBT<TKey,TKeyHelper>::iterateIncremental(int itemIdx) const
{
  if(items[itemIdx].flags & FLAG_NODE) {
    if(items[itemIdx].node.fwd >= 0) return items[itemIdx].node.fwd;
    return getNodeWithNextBiggerKey(itemIdx);
  } else {
    if(items[itemIdx].clusterItem.fwd >= 0) return items[itemIdx].clusterItem.fwd;
    return getNodeWithNextBiggerKey(items[itemIdx].clusterItem.startOfCluster);
  }
}

template <class TKey, class TKeyHelper>
void EPTF_RBT<TKey,TKeyHelper>::sortIncremental(EPTF__RBT__ItemIdxList &items)
{
  int count = getTreeItemCount();
  int idx = getSmallest();
  items.set_size(count);
  for(int i=0; i<count; i++) {
    if(idx < 0) {
      TTCN_Logger::begin_event(TTCN_ERROR);
      TTCN_Logger::log_event("ERROR: sortIncremental: item count = %d, sorted items = ", count);
      items.log();
      TTCN_Logger::end_event();
      f_EPTF_RBT_error("f_EPTF_RBT_sortIncremental: found less items than expected.");
    }
    items[i] = idx;
    idx = iterateIncremental(idx);
  }
  if(idx >= 0) {
//    dumpToPng("faulty_tree");
    TTCN_Logger::begin_event(TTCN_ERROR);
    TTCN_Logger::log_event("ERROR: sortIncremental: item count = %d, extra item found at index %d, sorted items = ", count, idx);
    items.log();
    TTCN_Logger::end_event();
    f_EPTF_RBT_error("f_EPTF_RBT_sortIncremental: found more items than expected.");
  }
}


template <class TKey, class TKeyHelper>
bool EPTF_RBT<TKey,TKeyHelper>::isTreeValid()
{
  if(root < 0) { return true; } // empty tree
  if(items[root].flags & FLAG_RED) {
    TTCN_Logger::log(TTCN_DEBUG, "EPTF_RBT::isTreeValid: root node is red.");
    return false;
  }
  black_depth = -1;
  bool ret = checkNode(root, -1, 0);
//  if(ret) TTCN_Logger::log(TTCN_DEBUG, "EPTF_RBT::isTreeValid: tree is valid.");
  if(root < 0) {
    if(smallestKeyIndex >= 0) {
      TTCN_Logger::log(TTCN_DEBUG, "EPTF_RBT::isTreeValid: smallest item index is set but the tree is empty.");
      return false;
    }
  } else {
    if(smallestKeyIndex != findSmallestFrom(root)) {
      TTCN_Logger::log(TTCN_DEBUG, "EPTF_RBT::isTreeValid: smallest item index cache is invalid. Cached: %d, actual smallest item: %d.",
        smallestKeyIndex, findSmallestFrom(root));
      return false;
    }
    // check if it's realy a binary search tree
    EPTF__RBT__ItemIdxList order;
    sortIncremental(order);
    TKey key = items[(int)order[0]].key;
    for(int i=1; i<order.lengthof(); i++) {
      TKey nextKey = items[(int)order[i]].key;
      if(TKeyHelper::isLower(nextKey, key)) {
        TTCN_Logger::log(TTCN_DEBUG, "EPTF_RBT::isTreeValid: incorrect order of sorted items, tree is not a binary search tree");
        return false;
      }
      key = nextKey;
    }
  }
  return ret;
}


template <class TKey, class TKeyHelper>
void EPTF_RBT<TKey,TKeyHelper>::dumpToPng(const char *name)
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
  fprintf(fp, "rankdir=TB;\n"); // top to buttom layout, other options: BT, LR, RL
  if(items == NULL || root < 0) {
    fprintf(fp, "smallest [shape=box, label=\"smallest: %d\"]\n", smallestKeyIndex);
    fprintf(fp, "empty [shape=box]\n");
  } else {
    fprintf(fp, "smallest [shape=box, label=\"smallest: %d (", smallestKeyIndex);
    TKeyHelper::dumpKey(fp, items[smallestKeyIndex].key);
    fprintf(fp, ")\"]\n");
    dumpNode(fp, root);
  }
  fprintf(fp, "}\n");
  fclose(fp);
  const char cmd[]="dot -Tpng %s > %s";
  char *tmp = new char[strlen(cmd) + strlen(fn_dot) + strlen(fn_png) + 1];
  sprintf(tmp, cmd, fn_dot, fn_png);
  if(-1 == system(tmp)) {
    TTCN_warning("EPTF_RBT::dumpToPng: system call `%s` failed.", tmp);
  }
  
  delete []fn_dot;
  delete []fn_png;
  delete []tmp;
}


template <class TKey, class TKeyHelper>
bool EPTF_RBT<TKey,TKeyHelper>::isItemNode(int itemIdx) const
{
  return items[itemIdx].flags & FLAG_NODE;
}

template <class TKey, class TKeyHelper>
bool EPTF_RBT<TKey,TKeyHelper>::isItemRed(int itemIdx) const
{
  return items[itemIdx].flags & FLAG_RED;
}

template <class TKey, class TKeyHelper>
bool EPTF_RBT<TKey,TKeyHelper>::isItemFree(int itemIdx) const
{
  return items[itemIdx].flags & FLAG_FREE;
}

template <class TKey, class TKeyHelper>
bool EPTF_RBT<TKey,TKeyHelper>::isItemInvalid(int itemIdx) const
{
  return items[itemIdx].flags & FLAG_INVALID;
}


///////////////////////////////////////////////////////////////////////////////
// implementation of TTCN-3 external functions
///////////////////////////////////////////////////////////////////////////////

static EPTF_RBT_Base **rbtree_list;
static int rbtree_count;

// Note: rbtree_deletedList works like a stack, i.e. the last deleted item will be reused the next time a tree is created
static int *rbtree_deletedList = NULL;
static int rbtree_deletedCount = 0;
static int rbtree_deletedAllocCount = 0;

void f_EPTF_RBT_pushDeletedTree(int queueId)
{
  if(rbtree_deletedList == NULL) {
    rbtree_deletedAllocCount = EPTF_RBT_MINITEMGROWBY;
    rbtree_deletedList = (int*) Malloc(sizeof(int) * rbtree_deletedAllocCount);
  } else if(rbtree_deletedCount+1 >= rbtree_deletedAllocCount) {
    rbtree_deletedAllocCount += EPTF_RBT_MINITEMGROWBY;
    rbtree_deletedList = (int*) Realloc(rbtree_deletedList,
      sizeof(int) * rbtree_deletedAllocCount);
  }
  rbtree_deletedList[rbtree_deletedCount] = queueId;
  rbtree_deletedCount++;
}

int f_EPTF_RBT_popDeletedTree() // returns -1 if none
{
  int ret_val = -1;
  if(rbtree_deletedCount > 0) {
    rbtree_deletedCount--;
    ret_val = rbtree_deletedList[rbtree_deletedCount];
  }
  return ret_val;
}

inline void f_EPTF_RBT_checkTreeId(const char *fn, int treeId)
{
  if(treeId<0 || treeId>=rbtree_count) {
    f_EPTF_RBT_error("%s: invalid tree %d, number of trees: %d", fn, treeId, rbtree_count);
  }
  if(rbtree_list[treeId] == NULL) {
    f_EPTF_RBT_error("%s: invalid tree %d, tree has been deleted.", fn, treeId);
  }
}

inline void f_EPTF_RBT_checkTreeType(const char *fn, int treeId, EPTF_RBT_KeyType keyType)
{
  if(rbtree_list[treeId]->getKeyType() != keyType) {
    if(rbtree_list[treeId]->getName() != NULL) {
      f_EPTF_RBT_error("%s: invalid tree type, tree \"%s\" is %s tree", fn, rbtree_list[treeId]->getName(), EPTF_RBT_KeyTypeNames[rbtree_list[treeId]->getKeyType()]);
    } else {
      f_EPTF_RBT_error("%s: invalid tree type, tree is %s tree", fn, EPTF_RBT_KeyTypeNames[rbtree_list[treeId]->getKeyType()]);
    }
  }
}

inline void f_EPTF_RBT_checkItemIdx(const char *fn, int treeId, int itemIdx, bool checkFree=true, bool checkInvalid=true)
{
  if(itemIdx<0 ||
     itemIdx>=rbtree_list[treeId]->getItemCount()) {
    if(rbtree_list[treeId]->getName() != NULL) {
      f_EPTF_RBT_error("%s: invalid item %d, number of items in tree \"%s\": %d",
        fn, itemIdx, rbtree_list[treeId]->getName(), rbtree_list[treeId]->getItemCount());
    } else {
      f_EPTF_RBT_error("%s: invalid item %d, number of items in tree: %d",
        fn, itemIdx, rbtree_list[treeId]->getItemCount());
    }
  }
  if(checkFree && rbtree_list[treeId]->isItemFree(itemIdx)) {
    if(rbtree_list[treeId]->getName() != NULL) {
      f_EPTF_RBT_error("%s: item %d in tree \"%s\" is in the free chain", fn, itemIdx, rbtree_list[treeId]->getName());
    } else {
      f_EPTF_RBT_error("%s: item %d is in the free chain", fn, itemIdx);
    }
  }
  if(checkInvalid && rbtree_list[treeId]->isItemInvalid(itemIdx)) {
    if(rbtree_list[treeId]->getName() != NULL) {
      f_EPTF_RBT_error("%s: item %d in tree \"%s\" is invalid", fn, itemIdx, rbtree_list[treeId]->getName());
    } else {
      f_EPTF_RBT_error("%s: item %d is invalid", fn, itemIdx);
    }
  }
}


void f__EPTF__RBT__init()
{
  rbtree_list = NULL;
  rbtree_count = 0;
  rbtree_deletedList = NULL;
  rbtree_deletedCount = 0;
  rbtree_deletedAllocCount = 0;
}

void f__EPTF__RBT__cleanup()
{
  if(rbtree_list) {
    for(int i=0; i<rbtree_count; i++) {
      if(rbtree_list[i]) {
        delete rbtree_list[i];
      }
    }
    Free(rbtree_list);
  }
  rbtree_list = NULL;
  rbtree_count = 0;
  if(rbtree_deletedList) {
    Free(rbtree_deletedList);
  }
  rbtree_deletedList = NULL;
  rbtree_deletedCount = 0;
  rbtree_deletedAllocCount = 0;
}

class IntKeyHelper
{
public:
  static void dumpKey(FILE *fp, int k) { fprintf(fp, "%d", k); }
  static bool isLower(int k1, int k2) { return k1 < k2; }
  static bool isGreater(int k1, int k2) { return k1 > k2; }
  inline static int compare(int k1, int k2) { // <0 -> k1<k2, ==0 -> k1==k2, >0 -> k1>k2
    return k1 - k2;
  }
  static int clone(int src) { return src; }
  static void dealloc(int &pKey) { }
};

class FloatKeyHelper
{
public:
  static void dumpKey(FILE *fp, double k) { fprintf(fp, "%.6f", k); }
//  static bool isLower(double k1, double k2) { return k1 < k2; }
//  static bool isGreater(double k1, double k2) { return k1 > k2; }
  static bool isLower(double k1, double k2) { return isless(k1, k2); }
  static bool isGreater(double k1, double k2) { return isgreater(k1, k2); }
  inline static int compare(double k1, double k2) { // <0 -> k1<k2, ==0 -> k1==k2, >0 -> k1>k2
    if(isgreater(k1,k2))return 1;
    if(isless(k1,k2))return -1;
    return 0;
  }
  static double clone(double src) { return src; }
  static void dealloc(double &pKey) { }
};

// Note: this mess is needed to keep the float version simple while still supporting charstring keys
class CharstringKeyHelper
{
public:
#ifdef EPTF_RBT_DEBUG
  static unsigned long m_new;
  static unsigned long m_del;
  ~CharstringKeyHelper() {
    if(m_new != m_del) {
      printf("CharstringKeyHelper: memory leak detected.\nNew calls: %lu, delete calls: %lu\n", m_new, m_del);
    }
  }
#endif
  static void dumpKey(FILE *fp, const char *k) { fprintf(fp, "%s", k); }
  static bool isLower(const char * k1, const char * k2) { return strcmp(k1, k2) < 0; }
  static bool isGreater(const char * k1, const char * k2) { return strcmp(k1, k2) > 0; }
  inline static int compare(const char * k1, const char * k2) { // <0 -> k1<k2, ==0 -> k1==k2, >0 -> k1>k2
    return strcmp(k1, k2);
  }
  static const char* clone(const char* src) {
#ifdef EPTF_RBT_DEBUG
    m_new++;
#endif
    size_t len = strlen(src);
    char *ret = new char[len+1];
    memcpy(ret, src, len);
    ret[len] = 0;
    return ret;
  }
  static void dealloc(const char* &pKey) {
    if(pKey) {
#ifdef EPTF_RBT_DEBUG
      m_del++;
#endif
      delete []pKey;
      pKey = NULL;
    }
  }
};

#ifdef EPTF_RBT_DEBUG
  unsigned long CharstringKeyHelper::m_new = 0;
  unsigned long CharstringKeyHelper::m_del = 0;
  CharstringKeyHelper cs_rbt_checkmemleak;
#endif

int f_EPTF_RBT_create()
{
  int treeId = f_EPTF_RBT_popDeletedTree();
  if(treeId < 0) {
    treeId = rbtree_count;
    rbtree_count++;
    if(rbtree_list == NULL) rbtree_list = (EPTF_RBT_Base**) Malloc(sizeof(EPTF_RBT_Base*));
    else rbtree_list = (EPTF_RBT_Base**) Realloc(rbtree_list, rbtree_count * sizeof(EPTF_RBT_Base*));
  }
  return treeId;
}

EPTF__RBT__TreeId f__EPTF__RBT__createIntTree(const CHARSTRING &pl__name)
{
  int treeId = f_EPTF_RBT_create();
  rbtree_list[treeId] = new EPTF_RBT<int,IntKeyHelper>(K_int, pl__name);
  return EPTF__RBT__TreeId(treeId);
}

EPTF__RBT__TreeId f__EPTF__RBT__createFloatTree(const CHARSTRING &pl__name)
{
  int treeId = f_EPTF_RBT_create();
  rbtree_list[treeId] = new EPTF_RBT<double,FloatKeyHelper>(K_double, pl__name);
  return EPTF__RBT__TreeId(treeId);
}

EPTF__RBT__TreeId f__EPTF__RBT__createCharstringTree(const CHARSTRING &pl__name)
{
  int treeId = f_EPTF_RBT_create();
  rbtree_list[treeId] = new EPTF_RBT<const char *,CharstringKeyHelper>(K_charstring, pl__name);
  return EPTF__RBT__TreeId(treeId);
}

void insertItem(int treeId, int parentItemIdx, const EPTF__RBT__TreeInitNode &node)
{
  switch(node.key().get_selection()) {
  case EPTF__RBT__Key::ALT_intKey: {
    f_EPTF_RBT_checkTreeType("f_EPTF_RBT_createAndInitTree", treeId, K_int);
    EPTF_RBT<int,IntKeyHelper> *tree = (EPTF_RBT<int,IntKeyHelper> *)rbtree_list[treeId];
    int itemIdx = tree->insertItem((int)node.key().intKey(), (bool)((boolean)node.isRed()),parentItemIdx);
    if(node.leftChild().ispresent()) {
      if(node.leftChild()().key().intKey() >= node.key().intKey()) {
        f_EPTF_RBT_error("insertItem: key of left child node must be less than key of node");
      }
      insertItem(treeId, itemIdx, node.leftChild()());
    }
    if(node.rightChild().ispresent()) {
      if(node.rightChild()().key().intKey() <= node.key().intKey()) {
        f_EPTF_RBT_error("insertItem: key of right child node must be greater than key of node");
      }
      insertItem(treeId, itemIdx, node.rightChild()());
    }
    break;
  }
  case EPTF__RBT__Key::ALT_floatKey: {
    f_EPTF_RBT_checkTreeType("f_EPTF_RBT_createAndInitTree", treeId, K_double);
    EPTF_RBT<double,FloatKeyHelper> *tree = (EPTF_RBT<double,FloatKeyHelper> *)rbtree_list[treeId];
    int itemIdx = tree->insertItem((double)node.key().floatKey(), (bool)((boolean)node.isRed()),parentItemIdx);
    if(node.leftChild().ispresent()) {
      if(node.leftChild()().key().floatKey() >= node.key().floatKey()) {
        f_EPTF_RBT_error("insertItem: key of left child node must be less than key of node");
      }
      insertItem(treeId, itemIdx, node.leftChild()());
    }
    if(node.rightChild().ispresent()) {
      if(node.rightChild()().key().floatKey() <= node.key().floatKey()) {
        f_EPTF_RBT_error("insertItem: key of right child node must be greater than key of node");
      }
      insertItem(treeId, itemIdx, node.rightChild()());
    }
    break;
  }
  case EPTF__RBT__Key::ALT_stringKey: {
    f_EPTF_RBT_checkTreeType("f_EPTF_RBT_createAndInitTree", treeId, K_charstring);
    EPTF_RBT<const char *,CharstringKeyHelper> *tree = (EPTF_RBT<const char *,CharstringKeyHelper> *)rbtree_list[treeId];
    int itemIdx = tree->insertItem((const char*)node.key().stringKey(), (bool)((boolean)node.isRed()),parentItemIdx);
    if(node.leftChild().ispresent()) {
      if(strcmp((const char*)node.leftChild()().key().stringKey(), (const char*)node.key().stringKey()) >= 0) {
        f_EPTF_RBT_error("insertItem: key of left child node must be less than key of node");
      }
      insertItem(treeId, itemIdx, node.leftChild()());
    }
    if(node.rightChild().ispresent()) {
      if(strcmp((const char*)node.rightChild()().key().stringKey(), (const char*)node.key().stringKey()) <= 0) {
        f_EPTF_RBT_error("insertItem: key of right child node must be greater than key of node");
      }
      insertItem(treeId, itemIdx, node.rightChild()());
    }
    break;
  }
  default :
    f_EPTF_RBT_error("insertItem: unsupported key type");
    break;
  }
}

// for testing
EPTF__RBT__TreeId f__EPTF__RBT__createAndInitTree(const CHARSTRING &pl__name, const EPTF__RBT__TreeInitNode &pl__rootNode)
{
  int treeId = -1;
  switch(pl__rootNode.key().get_selection()) {
  case EPTF__RBT__Key::ALT_intKey:
    treeId = f_EPTF_RBT_create();
    rbtree_list[treeId] = new EPTF_RBT<int,IntKeyHelper>(K_int, pl__name);
    break;
  case EPTF__RBT__Key::ALT_floatKey:
    treeId = f_EPTF_RBT_create();
    rbtree_list[treeId] = new EPTF_RBT<double,FloatKeyHelper>(K_double, pl__name);
    break;
  case EPTF__RBT__Key::ALT_stringKey:
    treeId = f_EPTF_RBT_create();
    rbtree_list[treeId] = new EPTF_RBT<const char *,CharstringKeyHelper>(K_charstring, pl__name);
    break;
  default :
    return EPTF__RBT__TreeId(-1);
    break;
  }

  insertItem(treeId, -1, pl__rootNode);
  if(! rbtree_list[treeId]->isTreeValid()) {
    TTCN_Logger::begin_event(TTCN_WARNING);
    TTCN_Logger::log_event_str("f_EPTF_RBT_createAndInitTree: invalid tree: ");
    pl__rootNode.log();
    TTCN_Logger::end_event();
    TTCN_Logger::log(TTCN_WARNING, "f_EPTF_RBT_createAndInitTree: dumping tree as f_EPTF_RBT_createAndInitTree.png");
    rbtree_list[treeId]->dumpToPng("f_EPTF_RBT_createAndInitTree");
    delete rbtree_list[treeId];
    rbtree_list[treeId] = NULL; // prevent cleanup from deleting it again;
    f_EPTF_RBT_pushDeletedTree(treeId);
  }

  return EPTF__RBT__TreeId(treeId);
}

void f__EPTF__RBT__deleteTree(const EPTF__RBT__TreeId &pl__tree)
{
  int treeId = (int)pl__tree;
  f_EPTF_RBT_checkTreeId("f_EPTF_RBT_deleteTree", treeId);
  delete rbtree_list[treeId];
  rbtree_list[treeId] = NULL; // prevent cleanup from deleting it again;
  f_EPTF_RBT_pushDeletedTree(treeId);
}

CHARSTRING f__EPTF__RBT__getName(const EPTF__RBT__TreeId &pl__tree)
{
  int treeId = (int)pl__tree;
  f_EPTF_RBT_checkTreeId("f_EPTF_RBT_getName", treeId);
  return rbtree_list[treeId]->getName();
}

INTEGER f__EPTF__RBT__getItemCount(const EPTF__RBT__TreeId &pl__tree)
{
  int treeId = (int)pl__tree;
  f_EPTF_RBT_checkTreeId("f_EPTF_RBT_getItemCount", treeId);
  return rbtree_list[treeId]->getTreeItemCount();
}

EPTF__RBT__ItemIdx f__EPTF__RBT__insertIntItem(
  const EPTF__RBT__TreeId &pl__tree,
  const INTEGER &pl__key)
{
  int treeId = pl__tree;
  f_EPTF_RBT_checkTreeId("f_EPTF_RBT_insertIntItem", treeId);
  f_EPTF_RBT_checkTreeType("f_EPTF_RBT_insertIntItem", treeId, K_int);
  EPTF_RBT<int,IntKeyHelper> *tree = (EPTF_RBT<int,IntKeyHelper> *)rbtree_list[treeId];
  return EPTF__RBT__ItemIdx(tree->insertItem((int)pl__key));
}

EPTF__RBT__ItemIdx f__EPTF__RBT__insertFloatItem(
  const EPTF__RBT__TreeId &pl__tree,
  const FLOAT &pl__key)
{
  int treeId = pl__tree;
  f_EPTF_RBT_checkTreeId("f_EPTF_RBT_insertFloatItem", treeId);
  f_EPTF_RBT_checkTreeType("f_EPTF_RBT_insertFloatItem", treeId, K_double);
  EPTF_RBT<double,FloatKeyHelper> *tree = (EPTF_RBT<double,FloatKeyHelper> *)rbtree_list[treeId];
  return EPTF__RBT__ItemIdx(tree->insertItem((double)pl__key));
}

EPTF__RBT__ItemIdx f__EPTF__RBT__insertCharstringItem(
  const EPTF__RBT__TreeId &pl__tree,
  const CHARSTRING &pl__key)
{
  int treeId = pl__tree;
  f_EPTF_RBT_checkTreeId("f_EPTF_RBT_insertCharstringtItem", treeId);
  f_EPTF_RBT_checkTreeType("f_EPTF_RBT_insertCharstringItem", treeId, K_charstring);
  EPTF_RBT<const char *,CharstringKeyHelper> *tree = (EPTF_RBT<const char *,CharstringKeyHelper> *)rbtree_list[treeId];
  return EPTF__RBT__ItemIdx(tree->insertItem((const char *)pl__key));
}

void f__EPTF__RBT__removeItem(
  const EPTF__RBT__TreeId &pl__tree,
  const EPTF__RBT__ItemIdx &pl__item)
{
  int treeId = pl__tree;
  f_EPTF_RBT_checkTreeId("f_EPTF_RBT_removeItem", treeId);
  int itemIdx = pl__item;
  f_EPTF_RBT_checkItemIdx("f_EPTF_RBT_removeItem", treeId, itemIdx);
  rbtree_list[treeId]->removeItemFromTree(itemIdx);
  rbtree_list[treeId]->putItemInFreeChain(itemIdx);
}

void f__EPTF__RBT__removeItemWithoutFree(
  const EPTF__RBT__TreeId &pl__tree,
  const EPTF__RBT__ItemIdx &pl__item)
{
  int treeId = pl__tree;
  f_EPTF_RBT_checkTreeId("f_EPTF_RBT_removeItemWithoutFree", treeId);
  int itemIdx = pl__item;
  f_EPTF_RBT_checkItemIdx("f_EPTF_RBT_removeItemWithoutFree", treeId, itemIdx);
  rbtree_list[treeId]->removeItemFromTree(itemIdx);
}

void f__EPTF__RBT__freeInvalidItem(
  const EPTF__RBT__TreeId &pl__tree,
  const EPTF__RBT__ItemIdx &pl__item)
{
  int treeId = pl__tree;
  f_EPTF_RBT_checkTreeId("f_EPTF_RBT_freeInvalidItem", treeId);
  int itemIdx = pl__item;
  f_EPTF_RBT_checkItemIdx("f_EPTF_RBT_freeInvalidItem", treeId, itemIdx, true, false);
  if(rbtree_list[treeId]->isItemInvalid(itemIdx)) {
    rbtree_list[treeId]->putItemInFreeChain(itemIdx);
  } else {
    if(rbtree_list[treeId]->getName() != NULL) {
      f_EPTF_RBT_error("f_EPTF_RBT_freeInvalidItem: item must be removed from tree \"%s\" first using f_EPTF_RBT_removeItemWithoutFree!", rbtree_list[treeId]->getName());
    } else {
      f_EPTF_RBT_error("f_EPTF_RBT_freeInvalidItem: item must be removed from tree first using f_EPTF_RBT_removeItemWithoutFree!");
    }
  }
}

EPTF__RBT__ItemIdx f__EPTF__RBT__getItemWithSmallestKey(
  const EPTF__RBT__TreeId &pl__tree)
{
  int treeId = pl__tree;
  f_EPTF_RBT_checkTreeId("f_EPTF_RBT_getItemWithSmallestKey", treeId);
  return EPTF__RBT__ItemIdx(rbtree_list[treeId]->getSmallest());
}

INTEGER f__EPTF__RBT__getKeyOfIntItem(
  const EPTF__RBT__TreeId &pl__tree,
  const EPTF__RBT__ItemIdx &pl__item)
{
  int treeId = pl__tree;
  f_EPTF_RBT_checkTreeId("f_EPTF_RBT_getKeyOfIntItem", treeId);
  f_EPTF_RBT_checkTreeType("f_EPTF_RBT_getKeyOfIntItem", treeId, K_int);
  int itemIdx = pl__item;
  f_EPTF_RBT_checkItemIdx("f_EPTF_RBT_getKeyOfIntItem", treeId, itemIdx);
  EPTF_RBT<int,IntKeyHelper> *tree = (EPTF_RBT<int,IntKeyHelper> *)rbtree_list[treeId];
  return INTEGER(tree->getKey(itemIdx));
}

FLOAT f__EPTF__RBT__getKeyOfFloatItem(
  const EPTF__RBT__TreeId &pl__tree,
  const EPTF__RBT__ItemIdx &pl__item)
{
  int treeId = pl__tree;
  f_EPTF_RBT_checkTreeId("f_EPTF_RBT_getKeyOfFloatItem", treeId);
  f_EPTF_RBT_checkTreeType("f_EPTF_RBT_getKeyOfFloatItem", treeId, K_double);
  int itemIdx = pl__item;
  f_EPTF_RBT_checkItemIdx("f_EPTF_RBT_getKeyOfFloatItem", treeId, itemIdx);
  EPTF_RBT<double,FloatKeyHelper> *tree = (EPTF_RBT<double,FloatKeyHelper> *)rbtree_list[treeId];
  return FLOAT(tree->getKey(itemIdx));
}

CHARSTRING f__EPTF__RBT__getKeyOfCharstringItem(
  const EPTF__RBT__TreeId &pl__tree,
  const EPTF__RBT__ItemIdx &pl__item)
{
  int treeId = pl__tree;
  f_EPTF_RBT_checkTreeId("f_EPTF_RBT_getKeyOfCharstringItem", treeId);
  f_EPTF_RBT_checkTreeType("f_EPTF_RBT_getKeyOfCharstringItem", treeId, K_charstring);
  int itemIdx = pl__item;
  f_EPTF_RBT_checkItemIdx("f_EPTF_RBT_getKeyOfCharstringItem", treeId, itemIdx);
  EPTF_RBT<const char *,CharstringKeyHelper> *tree = (EPTF_RBT<const char *,CharstringKeyHelper> *)rbtree_list[treeId];
  return CHARSTRING(tree->getKey(itemIdx));
}

EPTF__RBT__ItemIdx f__EPTF__RBT__findFirstItemByIntKey(
  const EPTF__RBT__TreeId &pl__tree,
  const INTEGER &pl__key)
{
  int treeId = pl__tree;
  f_EPTF_RBT_checkTreeId("f_EPTF_RBT_findFirstItemByIntKey", treeId);
  f_EPTF_RBT_checkTreeType("f_EPTF_RBT_findFirstItemByIntKey", treeId, K_int);
  EPTF_RBT<int,IntKeyHelper> *tree = (EPTF_RBT<int,IntKeyHelper> *)rbtree_list[treeId];
  return INTEGER(tree->findByKey((int)pl__key));
}

EPTF__RBT__ItemIdx f__EPTF__RBT__findFirstItemByFloatKey(
  const EPTF__RBT__TreeId &pl__tree,
  const FLOAT &pl__key)
{
  int treeId = pl__tree;
  f_EPTF_RBT_checkTreeId("f_EPTF_RBT_findFirstItemByFloatKey", treeId);
  f_EPTF_RBT_checkTreeType("f_EPTF_RBT_findFirstItemByFloatKey", treeId, K_double);
  EPTF_RBT<double,FloatKeyHelper> *tree = (EPTF_RBT<double,FloatKeyHelper> *)rbtree_list[treeId];
  return INTEGER(tree->findByKey((double)pl__key));
}

EPTF__RBT__ItemIdx f__EPTF__RBT__findFirstItemByCharstringKey(
  const EPTF__RBT__TreeId &pl__tree,
  const CHARSTRING &pl__key)
{
  int treeId = pl__tree;
  f_EPTF_RBT_checkTreeId("f_EPTF_RBT_findFirstItemByCharstringKey", treeId);
  f_EPTF_RBT_checkTreeType("f_EPTF_RBT_findFirstItemByCharstringKey", treeId, K_charstring);
  EPTF_RBT<const char *,CharstringKeyHelper> *tree = (EPTF_RBT<const char *,CharstringKeyHelper> *)rbtree_list[treeId];
  return INTEGER(tree->findByKey((const char *)pl__key));
}

EPTF__RBT__ItemIdx f__EPTF__RBT__getRoot(
  const EPTF__RBT__TreeId &pl__tree)
{
  int treeId = pl__tree;
  f_EPTF_RBT_checkTreeId("f_EPTF_RBT_getRoot", treeId);
  return  EPTF__RBT__ItemIdx(rbtree_list[treeId]->getRoot());
}

EPTF__RBT__ItemIdx f__EPTF__RBT__getLeft(
  const EPTF__RBT__TreeId &pl__tree,
  const EPTF__RBT__ItemIdx &pl__item)
{
  int treeId = pl__tree;
  f_EPTF_RBT_checkTreeId("f_EPTF_RBT_getLeft", treeId);
  int itemIdx = pl__item;
  f_EPTF_RBT_checkItemIdx("f_EPTF_RBT_getLeft", treeId, itemIdx);
  return  EPTF__RBT__ItemIdx(rbtree_list[treeId]->getLeft(itemIdx));
}

EPTF__RBT__ItemIdx f__EPTF__RBT__getRight(
  const EPTF__RBT__TreeId &pl__tree,
  const EPTF__RBT__ItemIdx &pl__item)
{
  int treeId = pl__tree;
  f_EPTF_RBT_checkTreeId("f_EPTF_RBT_getRight", treeId);
  int itemIdx = pl__item;
  f_EPTF_RBT_checkItemIdx("f_EPTF_RBT_getRight", treeId, itemIdx);
  return  EPTF__RBT__ItemIdx(rbtree_list[treeId]->getRight(itemIdx));
}

EPTF__RBT__ItemIdx f__EPTF__RBT__getParent(
  const EPTF__RBT__TreeId &pl__tree,
  const EPTF__RBT__ItemIdx &pl__item)
{
  int treeId = pl__tree;
  f_EPTF_RBT_checkTreeId("f_EPTF_RBT_getParent", treeId);
  int itemIdx = pl__item;
  f_EPTF_RBT_checkItemIdx("f_EPTF_RBT_getParent", treeId, itemIdx);
  return  EPTF__RBT__ItemIdx(rbtree_list[treeId]->getParent(itemIdx));
}

EPTF__RBT__ItemIdx f__EPTF__RBT__getNextInChain(
  const EPTF__RBT__TreeId &pl__tree,
  const EPTF__RBT__ItemIdx &pl__item)
{
  int treeId = pl__tree;
  f_EPTF_RBT_checkTreeId("f_EPTF_RBT_getNextInChain", treeId);
  int itemIdx = pl__item;
  f_EPTF_RBT_checkItemIdx("f_EPTF_RBT_getNextInChain", treeId, itemIdx);
  return  EPTF__RBT__ItemIdx(rbtree_list[treeId]->getNextInChain(itemIdx));
}

EPTF__RBT__ItemIdx f__EPTF__RBT__getPrevInChain(
  const EPTF__RBT__TreeId &pl__tree,
  const EPTF__RBT__ItemIdx &pl__item)
{
  int treeId = pl__tree;
  f_EPTF_RBT_checkTreeId("f_EPTF_RBT_getPrevInChain", treeId);
  int itemIdx = pl__item;
  f_EPTF_RBT_checkItemIdx("f_EPTF_RBT_getPrevInChain", treeId, itemIdx);
  return  EPTF__RBT__ItemIdx(rbtree_list[treeId]->getPrevInChain(itemIdx));
}

// gets either the next element in chain or (if none) the first element with the next bigger key
EPTF__RBT__ItemIdx f__EPTF__RBT__iterateIncremental(
  const EPTF__RBT__TreeId &pl__tree,
  const EPTF__RBT__ItemIdx &pl__item)
{
  int treeId = pl__tree;
  f_EPTF_RBT_checkTreeId("f_EPTF_RBT_iterateIncremental", treeId);
  int itemIdx = pl__item;
  f_EPTF_RBT_checkItemIdx("f_EPTF_RBT_iterateIncremental", treeId, itemIdx);
  return  EPTF__RBT__ItemIdx(rbtree_list[treeId]->iterateIncremental(itemIdx));

}

void f__EPTF__RBT__sortIncremental(
  const EPTF__RBT__TreeId &pl__tree,
  EPTF__RBT__ItemIdxList &pl__itemsSorted)
{
  int treeId = pl__tree;
  f_EPTF_RBT_checkTreeId("f_EPTF_RBT_sortIcremental", treeId);
  rbtree_list[treeId]->sortIncremental(pl__itemsSorted);
}

BOOLEAN f__EPTF__RBT__isItemValid(
  const EPTF__RBT__TreeId &pl__tree,
  const EPTF__RBT__ItemIdx &pl__item)
{
  int treeId = pl__tree;
  int itemIdx = pl__item;
  f_EPTF_RBT_checkTreeId("f_EPTF_RBT_isItemValid", treeId);
  if(itemIdx<0 ||
     itemIdx>=rbtree_list[treeId]->getItemCount()) {
    return BOOLEAN(FALSE);
  }
  if(rbtree_list[treeId]->isItemFree(itemIdx)) {
    return BOOLEAN(FALSE);
  }
  if(rbtree_list[treeId]->isItemInvalid(itemIdx)) {
    return BOOLEAN(FALSE);
  }
  return BOOLEAN(TRUE);
}

BOOLEAN f__EPTF__RBT__isTreeValid(const INTEGER &pl__tree)
{
  int treeId = pl__tree;
  f_EPTF_RBT_checkTreeId("f_EPTF_RBT_isTreeValid", treeId);
  return BOOLEAN(rbtree_list[treeId]->isTreeValid());
}

EPTF__RBT__ItemType f__EPTF__RBT__getItemType(const EPTF__RBT__TreeId &pl__tree, const EPTF__RBT__ItemIdx &pl__item)
{
  int treeId = pl__tree;
  int itemIdx = pl__item;
  f_EPTF_RBT_checkTreeId("f_EPTF_RBT_getItemType", treeId);
  f_EPTF_RBT_checkItemIdx("f_EPTF_RBT_getItemType", treeId, itemIdx, false, true);
  if(rbtree_list[treeId]->isItemFree(itemIdx)) {
    return EPTF__RBT__ItemType(EPTF__RBT__ItemType::FreeItem);
  } else if(rbtree_list[treeId]->isItemNode(itemIdx)) {
    if(rbtree_list[treeId]->isItemRed(itemIdx)) {
      return EPTF__RBT__ItemType(EPTF__RBT__ItemType::RedNode);
    } else {
      return EPTF__RBT__ItemType(EPTF__RBT__ItemType::BlackNode);
    }
  } else {
    return EPTF__RBT__ItemType(EPTF__RBT__ItemType::ChainItem);
  }
}

void f__EPTF__RBT__cloneTree(const INTEGER &pl__src, const INTEGER &pl__dst)
{
  int src = (int)pl__src;
  int dst = (int)pl__dst;
  f_EPTF_RBT_checkTreeId("f_EPTF_RBT_cloneTree", src);
  f_EPTF_RBT_checkTreeId("f_EPTF_RBT_cloneTree", dst);
  delete rbtree_list[dst];
  switch(rbtree_list[src]->getKeyType()) {
  case K_int:
    rbtree_list[dst] = new EPTF_RBT<int,IntKeyHelper>(*((EPTF_RBT<int,IntKeyHelper>*)rbtree_list[src]));
    break;
  case K_double:
    rbtree_list[dst] = new EPTF_RBT<double,FloatKeyHelper>(*((EPTF_RBT<double,FloatKeyHelper>*)rbtree_list[src]));
    break;
  case K_charstring:
    rbtree_list[dst] = new EPTF_RBT<const char *,CharstringKeyHelper>(*((EPTF_RBT<const char *,CharstringKeyHelper>*)rbtree_list[src]));
    break;
  default:
    f_EPTF_RBT_error("f_EPTF_RBT_cloneTree: unhandled key type");
  }
}

void f__EPTF__RBT__dumpToPng(
  const EPTF__RBT__TreeId &pl__tree,
  const CHARSTRING &pl__name)
{
  int treeId = pl__tree;
  f_EPTF_RBT_checkTreeId("f_EPTF_RBT_dumpToPng", treeId);
  rbtree_list[treeId]->dumpToPng(pl__name);
}

} // namespace EPTF__CLL__RBT__Functions
