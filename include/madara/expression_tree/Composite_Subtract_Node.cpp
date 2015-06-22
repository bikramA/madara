/* -*- C++ -*- */
#ifndef _SUBTRACT_NODE_CPP_
#define _SUBTRACT_NODE_CPP_

#ifndef _MADARA_NO_KARL_

#include "madara/expression_tree/Component_Node.h"
#include "madara/expression_tree/Visitor.h"
#include "madara/expression_tree/Composite_Binary_Node.h"
#include "madara/expression_tree/Composite_Subtract_Node.h"
#include "madara/expression_tree/Leaf_Node.h"

Madara::Expression_Tree::Composite_Subtract_Node::Composite_Subtract_Node (
  Logger::Logger & logger, Component_Node *left, Component_Node *right)
: Madara::Expression_Tree::Composite_Binary_Node (logger, left, right)
{    
}

// Dtor
Madara::Expression_Tree::Composite_Subtract_Node::~Composite_Subtract_Node (void)
{
}

Madara::Knowledge_Record
Madara::Expression_Tree::Composite_Subtract_Node::item (void) const
{
  return "-";
}

/// Prune the tree of unnecessary nodes. 
/// Returns evaluation of the node and sets can_change appropriately.
/// if this node can be changed, that means it shouldn't be pruned.
Madara::Knowledge_Record
Madara::Expression_Tree::Composite_Subtract_Node::prune (bool & can_change)
{
  bool left_child_can_change = false;
  bool right_child_can_change = false;
  Madara::Knowledge_Record left_value;
  Madara::Knowledge_Record right_value;

  if (this->left_)
  {
    left_value = this->left_->prune (left_child_can_change);
    if (!left_child_can_change && dynamic_cast <Leaf_Node *> (left_) == 0)
    {
      delete this->left_;
      this->left_ = new Leaf_Node (*(this->logger_), left_value);
    }
  }
  else
  {
    logger_->log (Logger::LOG_EMERGENCY,
      "KARL COMPILE ERROR: Subtraction has no left expression\n");

    exit (-1);
  }

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
      "KARL COMPILE ERROR: Subtraction has no right expression\n");

    exit (-1); 
  }

  can_change = left_child_can_change || right_child_can_change;

  return left_value - right_value;
}

/// Evaluates the node and its children. This does not prune any of
/// the expression tree, and is much faster than the prune function
Madara::Knowledge_Record 
Madara::Expression_Tree::Composite_Subtract_Node::evaluate (
  const Madara::Knowledge_Engine::Knowledge_Update_Settings & settings)
{
  // note we do not check if left or right are null. This should be checked
  // during prune or an earlier phase. Evaluate is all about speed.
  return left_->evaluate (settings) - right_->evaluate (settings);
}

// accept a visitor
void 
Madara::Expression_Tree::Composite_Subtract_Node::accept (Visitor &visitor) const
{
  visitor.visit (*this);
}

#endif // _MADARA_NO_KARL_

#endif /* _SUBTRACT_NODE_CPP_ */
