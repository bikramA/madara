/* -*- C++ -*- */
#ifndef _NEGATE_NODE_CPP_
#define _NEGATE_NODE_CPP_

#ifndef _MADARA_NO_KARL_

#include <iostream>

#include "madara/expression_tree/Component_Node.h"
#include "madara/expression_tree/Composite_Unary_Node.h"
#include "madara/expression_tree/Visitor.h"
#include "madara/expression_tree/Composite_Negate_Node.h"
#include "madara/expression_tree/Leaf_Node.h"

Madara::Expression_Tree::Composite_Negate_Node::Composite_Negate_Node (
  Logger::Logger & logger, Component_Node *right)
  : Composite_Unary_Node (logger, right)
{    
}

// Dtor
Madara::Expression_Tree::Composite_Negate_Node::~Composite_Negate_Node (void)
{
}

Madara::Knowledge_Record
Madara::Expression_Tree::Composite_Negate_Node::item (void) const
{
  return "-";
}

/// Prune the tree of unnecessary nodes. 
/// Returns evaluation of the node and sets can_change appropriately.
/// if this node can be changed, that means it shouldn't be pruned.
Madara::Knowledge_Record
Madara::Expression_Tree::Composite_Negate_Node::prune (bool & can_change)
{
  bool right_child_can_change = false;
  Madara::Knowledge_Record right_value;

  if (this->right_)
  {
    right_value = this->right_->prune (right_child_can_change);
    if (!right_child_can_change && dynamic_cast <Leaf_Node *> (right_) == 0)
    {
      delete this->right_;
      this->right_ = new Leaf_Node (*(this->logger_), right_value);
    }
  }
  else
  {
    logger_->log (Logger::LOG_EMERGENCY,
      "KARL COMPILE ERROR: "
      "Negate has no right expression\n");

    exit (-1); 
  }

  can_change = right_child_can_change;

  return -right_value;
}

/// Evaluates the node and its children. This does not prune any of
/// the expression tree, and is much faster than the prune function
Madara::Knowledge_Record 
Madara::Expression_Tree::Composite_Negate_Node::evaluate (
  const Madara::Knowledge_Engine::Knowledge_Update_Settings & settings)
{
  Madara::Knowledge_Record value = right_->evaluate (settings);

  logger_->log (Logger::LOG_DETAILED,
    "KARL COMPILE ERROR: "
    "Negating %s.\n", value.to_string ().c_str ());

  return -value;
}

// accept a visitor
void 
Madara::Expression_Tree::Composite_Negate_Node::accept (Visitor &visitor) const
{
  visitor.visit (*this);
}

#endif // _MADARA_NO_KARL_

#endif /* _NEGATE_NODE_CPP_ */
