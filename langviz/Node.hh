/******************************************************************************
 * Copyright (c) 2000-2019 Ericsson Telecom AB
 * All rights reserved. This program and the accompanying materials
 * are made available under the terms of the Eclipse Public License v2.0
 * which accompanies this distribution, and is available at
 * https://www.eclipse.org/org/documents/epl-2.0/EPL-2.0.html
 *
 * Contributors:
 *   Balasko, Jeno
 *   Forstner, Matyas
 *
 ******************************************************************************/
#ifndef _langviz_Node_HH
#define _langviz_Node_HH

/**
 * Base class for AST-classes.
 */
class Node {
private:
  /** To detect missing/duplicated invoking of destructor. */
  static int counter;
#ifdef MEMORY_DEBUG
  /** Linked list for tracking undeleted nodes. */
  Node *prev_node, *next_node;
#endif
protected:
  /** Default constructor. */
  Node();
  /** The copy constructor. */
  Node(const Node& p);
public:
  /**
   * "Virtual constructor".
   */
  virtual Node* clone() const =0;
  /** The destructor. */
  virtual ~Node();
  /** Gives an error message if counter is not zero. It can be
   * invoked before the program exits (like check_mem_leak()) to
   * verify that all Nodes are destructed. */
  static void chk_counter();
};

#endif // _langviz_Node_HH
