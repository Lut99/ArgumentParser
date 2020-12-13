/* LOGIC EXPRESSION.hpp
 *   by Lut99
 *
 * Created:
 *   13/12/2020, 15:39:57
 * Last edited:
 *   13/12/2020, 16:32:58
 * Auto updated?
 *   Yes
 *
 * Description:
 *   This file contains the code to parse and then evaluate a
 *   LogicExpression for the Preprocessor.
**/

#ifndef LOGIC_EXPRESSION_HPP
#define LOGIC_EXPRESSION_HPP

namespace ArgumentParser {
    /* The LogicNode class, which forms the baseclass for both LogicValues and LogicOperators. */
    class LogicNode {
    public:
        /* Default constructor for the LogicNode, which doesn't do a lot. */
        LogicNode();

        /* Evaluates the tree from this node on, returning the correct boolean value. */
        virtual bool evaluate() const = 0;

        /* Allows the LogicNodes to copy themselves polymorphically. */
        virtual LogicNode* copy() const = 0;

    };

    /* The LogicValue class, which is used to represent a 'constant' boolean. */
    class LogicValue: public LogicNode {
    public:
        /* The value that this LogicValue wraps. */
        bool value;

        /* Constructor for the LogicValue, which optionally takes the value it wraps. */
        LogicValue(bool value = true);

        /* Evaluates the value, i.e., returns the internal value. */
        virtual bool evaluate() const;

        /* Allows the LogicValue to copy itself polymorphically. */
        virtual LogicValue* copy() const;
        
    };

    /* The LogicOperator class, which is used to represent either a '&&' or a '||'. */
    class LogicOperator: public LogicNode {
    public:
        /* The lefthand side value of the operator. */
        LogicNode* left;
        /* The righthand side value of the operator. */
        LogicNode* right;


        /* Constructor for the LogicOperator, which optionally takes the left hand side to operate on and the right hand side. */
        LogicOperator(LogicNode* left = nullptr, LogicNode* right = nullptr);
        /* Copy constructor for the LogicOperator class. */
        LogicOperator(const LogicOperator& other);
        /* Move constructor for the LogicOperator class. */
        LogicOperator(LogicOperator&& other);
        /* Destructor for the LogicOperator. */
        virtual ~LogicOperator();

        /* Swap operator for the LogicOperator. */
        friend void ArgumentParser::swap(LogicOperator& lo1, LogicOperator& lo2);

    };

    /* The LogicAnd class, which implements the logical-and operation. */
    class LogicAnd: public LogicOperator {
    public:
        /* Constructor for the LogicAnd class, which optionally takes the left hand side to operate on and the right hand side. */
        LogicAnd(LogicNode* left = nullptr, LogicNode* right = nullptr);
        /* Copy constructor for the LogicAnd class. */
        LogicAnd(const LogicAnd& other);
        /* Move constructor for the LogicAnd class. */
        LogicAnd(LogicAnd&& other);
        /* Destructor for the LogicAnd. */
        virtual ~LogicAnd();
        
        /* Evaluates the lefthand side and the righthand side, and then ands those values. */
        virtual bool evaluate() const;

        /* Allows the LogicAnd to copy itself polymorphically. */
        virtual LogicAnd* copy() const;
        
        /* Copy assignment operator for the LogicAnd. */
        inline LogicAnd& operator=(const LogicAnd& other) { return *this = LogicAnd(other); }
        /* Move assignment operator for the LogicAnd. */
        LogicAnd& operator=(LogicAnd&& other);
        /* Swap operator for the LogicAnd. */
        friend void swap(LogicAnd& lo1, LogicAnd& lo2);
    };



    /* The LogicExpression class, which uses LogicOperators and LogicValues to parse the meaning of a logical statement involving &&'s and ||'s. */
    class LogicExpression {
    private:
        /* Root node of the tree. */
        LogicNode* root;

    public:
        /* Default constructor for the LogicExpression class. */
        LogicExpression();
        /* Copy constructor for the LogicExpression class. */
        LogicExpression(const LogicExpression& other);
        /* Move constructor for the LogicExpression class. */
        LogicExpression(LogicExpression&& other);
        /* Destructor for the LogicExpression class. */
        ~LogicExpression();

        /* Adds a new boolean value to the tree. */
        void add_value(const LogicValue& val);
        /* Adds a new operator to the tree. */
        void add_operator(const LogicOperator& op);
        /* Evaluates the tree, returning the correct boolean value. */
        bool evaluate() const;

        /* Copy assignment operator for the LogicExpression. */
        inline LogicExpression& operator=(const LogicExpression& other) { return *this = LogicExpression(other); }
        /* Move assignment operator for the LogicExpression. */
        LogicExpression& operator=(LogicExpression&& other);
        /* Swap operator for the LogicExpression. */
        friend void swap(LogicExpression& le1, LogicExpression& le2);

    };
}

#endif
