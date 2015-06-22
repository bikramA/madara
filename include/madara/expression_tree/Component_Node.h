/* -*- C++ -*- */
#ifndef _COMPONENT_NODE_H_
#define _COMPONENT_NODE_H_

#ifndef _MADARA_NO_KARL_

#include <string>
#include <deque>
#include <stdexcept>
#include "madara/knowledge_engine/Knowledge_Record.h"
#include "madara/knowledge_engine/Knowledge_Update_Settings.h"
#include "madara/utility/stdint.h"
#include "madara/logger/Logger.h"

namespace Madara
{
  namespace Knowledge_Engine
  {
    class Thread_Safe_Context;
  }

  namespace Expression_Tree
  {
    // Forward declaration.
    class Visitor;

    /**
     * @class Component_Node
     * @brief An abstract base class defines a simple abstract
     *        implementation of an expression tree node.
     *        
     * @see   See Composite_Unary_Node, Composite_Binary_Node, and Leaf Node
     *        for immediate subclasses of this class.
     */
    class Component_Node
    {
    public:
      /**
       * Constructor
       * @param  logger   the logger to use for printing
       **/
      Component_Node (Logger::Logger & logger);

      /**
       * Destructor
       **/
      virtual ~Component_Node (void) = 0;

      /**
       * Returns the value of the node
       * @return    value of the node
       **/
      virtual Madara::Knowledge_Record item (void) const;

      /** 
       * Prunes the expression tree of unnecessary nodes. 
       * @return    value of current contained expression tree
       **/
      virtual Madara::Knowledge_Record prune (bool & can_change) = 0;

      /** 
       * Evaluates the expression tree. 
       * @param     settings     settings for evaluating the node
       * @return    value of current contained expression tree
       **/
      virtual Madara::Knowledge_Record evaluate (
        const Madara::Knowledge_Engine::Knowledge_Update_Settings & settings)
          = 0;

      /** 
       * Returns the left expression. 
       * @return    a pointer to the left expression
       **/
      virtual Component_Node *left (void) const;

      /** 
       * Returns the right expression. 
       * @return    a pointer to the right expression
       **/
      virtual Component_Node *right (void) const;

      /** 
       * Accepts a visitor subclassed from the Visitor class
       * @param    visitor   visitor instance to use
       **/
      virtual void accept (Visitor &visitor) const;

      /**
       * Sets the logger for printing errors and debugging info
       * @param  logger the logger to use
       **/
      void set_logger (Logger::Logger & logger);

    protected:
      /// handle the context
      Logger::Logger * logger_;
    };

    /// a vector of Component Nodes
    typedef   std::deque <Component_Node *>   Component_Nodes;
  }
}

#endif // _MADARA_NO_KARL_

#endif /* _COMPONENT_NODE_H_ */
