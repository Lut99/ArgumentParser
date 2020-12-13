/* LOGIC EXPRESSION.cpp
 *   by Lut99
 *
 * Created:
 *   13/12/2020, 15:39:42
 * Last edited:
 *   13/12/2020, 16:32:45
 * Auto updated?
 *   Yes
 *
 * Description:
 *   This file contains the code to parse and then evaluate a
 *   LogicExpression for the Preprocessor.
**/

#include <utility>

#include "LogicExpression.hpp"

using namespace std;
using namespace ArgumentParser;


/***** LOGICNODE CLASS *****/

/* Default constructor for the LogicNode, which doesn't do a lot. */
LogicNode::LogicNode() {}





/***** LOGICVALUE CLASS *****/

/* Constructor for the LogicValue, which optionally takes the value it wraps. */
LogicValue::LogicValue(bool value) :
    value(value)
{}



/* Evaluates the value, i.e., returns the internal value. */
bool LogicValue::evaluate() const { return this->value; }



/* Allows the LogicValue to copy itself polymorphically. */
LogicValue* LogicValue::copy() const { return new LogicValue(*this); }





/***** LOGICOPERATOR CLASS *****/

/* Constructor for the LogicOperator, which optionally takes the left hand side to operate on and the right hand side. */
LogicOperator::LogicOperator(LogicNode* left, LogicNode* right) :
    left(left),
    right(right)
{}

/* Copy constructor for the LogicOperator class. */
LogicOperator::LogicOperator(const LogicOperator& other) :
    left(other.left->copy()),
    right(other.right->copy())
{}

/* Move constructor for the LogicOperator class. */
LogicOperator::LogicOperator(LogicOperator&& other) :
    left(other.left),
    right(other.right)
{
    // Set the other's operators to nullptrs to void deallocation
    other.left = nullptr;
    other.right = nullptr;
}

/* Destructor for the LogicOperator. */
LogicOperator::~LogicOperator() {
    // Only deallocate the nodes if they're non-nullptrs
    if (this->left != nullptr) { delete this->left; }
    if (this->right != nullptr) { delete this->right; }
}



/* Swap operator for the LogicOperator. */
void ArgumentParser::swap(LogicOperator& lo1, LogicOperator& lo2) {
    using std::swap;

    swap(lo1.left, lo2.left);
    swap(lo1.right, lo2.right);
}





/***** LOGICAND CLASS *****/

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
void ArgumentParser::swap(LogicAnd& la1, LogicAnd& la2) {
    using std::swap;

    swap((LogicOperator&) la1, (LogicOperator& la2));
}





/***** LOGICEXPRESSION CLASS *****/

/* Default constructor for the LogicExpression class. */
LogicExpression::LogicExpression() {}

/* Copy constructor for the LogicExpression class. */
LogicExpression::LogicExpression(const LogicExpression& other) :
    root(other.root->copy())
{}

/* Move constructor for the LogicExpression class. */
LogicExpression::LogicExpression(LogicExpression&& other) :
    root(other.root)
{
    // Set the other's field to nullptr to avoid deallocation
    other.root = nullptr;
}

/* Destructor for the LogicExpression class. */
LogicExpression::~LogicExpression() {
    // Only deallocate if not nullptr
    if (this->root != nullptr) { delete this->root; }
}



/* Adds a new boolean value to the tree. */
void LogicExpression::add_value(const LogicValue& val) {
    
}

/* Adds a new operator to the tree. */
void LogicExpression::add_operator(const LogicOperator& op) {
    
}



/* Move assignment operator for the LogicExpression. */
LogicExpression& LogicExpression::operator=(LogicExpression&& other) {
    if (this != &other) { swap(*this, other); }
    return *this;
}

/* Swap operator for the LogicExpression. */
void ArgumentParser::swap(LogicExpression& le1, LogicExpression& le2) {
    using std::swap;

    swap(le1.root, le1.root);
}
