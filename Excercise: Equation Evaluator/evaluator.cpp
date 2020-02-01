//
//  evaluator.cpp
//  Excercise: Equation Evaluator
//
//  Created by Michael Levesque on 12/20/19.
//  Copyright © 2019 Michael Levesque. All rights reserved.
//

#include "evaluator.h"
#include <regex>
#include <list>
#include <cmath>
#include <algorithm>

typedef std::string::iterator StrIter;
typedef std::list<Node*>::iterator ListIter;

// Regular expressions to help parse through equation string
const std::regex openGroupReg("^[(].*");
const std::regex closeGroupReg("^[)].*");
const std::regex validOperandLiteralReg("^\\d+[.]*\\d*");
const std::regex validOperatorReg("^([\\+\\-\\*\\/\\^]{1})");
const std::regex negateReg("^[-]{1}");

Node::Node()
: isLiteral(false)
, left(nullptr)
, right(nullptr)
{}

Node* buildEquationTree(std::string equation, const bool isSubGroup);

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// STRING PARSING

/// Removes a negate symbol at the beginning of the given remaining equation string and returns it as a Node
/// representing the negate operation. This will return null if there is no negate symbol.
/// @param equation A reference to the equation string to pop from. The string will be modified.
Node* popNegateFromEquation(std::string& equation) {
    std::smatch sm;
    if (std::regex_search(equation, sm, negateReg)) {
        // update remaining equation with suffix of the negate
        equation = sm.suffix();
        
        // build negate node
        Node* node = new Node();
        node->operatorSymbol = Negate;
        node->isLiteral = false;
        return node;
    }
    else {
        return nullptr;
    }
}

/// Removes a group from the beginning of the given equation string and returns a tree representing the contents of
/// the equation within the group. This can throw an exception if there is not a valid balance of open and closing
/// grouping symbols.
/// @param equation A reference to the equation string to pop from. The string will be modified.
Node* popGroupFromEquation(std::string& equation) noexcept(false) {
    StrIter startPos = equation.begin();
    StrIter traversePos = equation.begin();
    int level = 0;
    do {
        // increment level if open group symbol
        if (std::regex_match(traversePos, traversePos + 1, openGroupReg)) {
            level++;
        }
        // decrement level if closed group symbol
        else if (std::regex_match(traversePos, traversePos + 1, closeGroupReg)) {
            level--;
        }
        // go to next character
        ++traversePos;
    } while (level > 0 && traversePos != equation.end());
    
    // if level is not zero, then we must have reached the end of the equation and found that there weren't enough
    // closing parentheses
    if (level > 0) {
        throw MissingClosingSymbol;
    }
    
    // create group substring, then update remaining equation with suffix of the group
    std::string groupStr(startPos + 1, traversePos - 1);
    equation = std::string(traversePos, equation.end());
    
    // recurse with substring
    return buildEquationTree(groupStr, true);
}

/// Removes a literal operand from the front of the given equation string and returns the literal as a node. This will
/// throw an exception if there is not a valid number literal at the front of the equation.
/// @param equation A reference to the equation string to pop from. The string will be modified.
Node* popLiteralFromEquation(std::string& equation) noexcept(false) {
    std::smatch sm;
    if (std::regex_search(equation, sm, validOperandLiteralReg)) {
        // convert number string to double
        double value = strtod(sm.str().c_str(), nullptr);
        
        // update remaining equation with suffix of the literal
        equation = sm.suffix();
        
        // build node
        Node* node = new Node();
        node->liternal = value;
        node->isLiteral = true;
        return node;
    }
    else {
        throw InvalidOperand;
    }
}

/// Removes the operand from the front of the given equation and returns it as a single node (if operand is a number)
/// or a tree (if the operand is a group designated by an open grouping symbol.
/// @param equation A reference to the equation string to pop from. The string will be modified.
Node* popOperandFromEquation(std::string& equation) noexcept(false) {
    // Determine if we have a group operand, or a literal
    if (std::regex_match(equation, openGroupReg)) {
        return popGroupFromEquation(equation);
    }
    else {
        return popLiteralFromEquation(equation);
    }
}

/// Removes the operator from the front of the given equation and returns it as a node. This will throw an exception
/// if the operator is not valid.
/// @param equation A reference to the equation string to pop from. The string will be modified.
Node* popOperatorFromEquation(std::string& equation) noexcept(false) {
    // Grab the operator symbol
    std::smatch sm;
    if (std::regex_search(equation, sm, validOperatorReg)) {
        // build node
        Node* node = new Node();
        node->operatorSymbol = sm[1].str()[0];
        
        // update remaining equation with suffix of the literal
        equation = sm.suffix();
        
        return node;
    }
    else {
        throw InvalidOperator;
    }
}


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// TREE BUILDING

/// If a negate node exists at the front of the given equation node list, then this will remove the neighboring operand
/// from the list and attach it to the negate node.
/// @param nodeList The list of nodes representing the operands and operators in the order that they appear in the
///     original equation.
void processNegationNode(std::list<Node*>& nodeList) noexcept(false) {
    Node* firstNode = *nodeList.begin();
    // Note: the node right leaf check is to prevent an issue that can happen if the equation has a negate operator
    // within a group. If a leaf node exists, then this negate node has already been processed and should not be
    // processed again
    if (!firstNode->isLiteral && firstNode->operatorSymbol == Negate && firstNode->right == nullptr) {
        ListIter secondIter = std::next(nodeList.begin());
        firstNode->right = *secondIter;
        nodeList.erase(secondIter);
    }
}

/// Loops through the given equation node list and finds all operators that match one of the two given operators. For
/// each one that it finds, it removes the neighboring operands from the list and attaches them to the operator node.
/// @param nodeList The list of nodes representing the operands and operators in the order that they appear in the
///     original equation.
/// @param op1 First operator to find.
/// @param op2 Second operator to find. There are two operators to check for because Addition/Subtraction and
///     Multiplication/Division are processed together in one run.
void processOperatorNodes(std::list<Node*>& nodeList, OperatorType op1, OperatorType op2) {
    ListIter previousIter = nodeList.begin();
    std::list<ListIter> elementsToRemove;
    for (auto i = std::next(previousIter); i != nodeList.end(); std::advance(i, 2)) {
        Node* node = *i;
        
        // if we found an operator that we are searching for, then we will take the left and right operands
        // in the previous and next elements in the list and combine those into a tree with the operator as the root
        if (node->operatorSymbol == op1 || node->operatorSymbol == op2) {
            auto nextIter = std::next(i);
            node->left = *previousIter;
            node->right = *nextIter;
            
            // remove the two operands from the list
            elementsToRemove.push_front(previousIter);
            elementsToRemove.push_front(nextIter);
            
            // our next previous iterator will point to our newly-formed tree
            previousIter = i;
        }
        
        // if no matching operator, then we update our previous iterator to the position before the next jump of i
        // i will jump by 2 places, so our previous iterator will be i+1
        else {
            previousIter = std::next(i);
        }
    }
    
    // remove all operands from the list that were attached to operator nodes from our above pass
    for (auto i = elementsToRemove.begin(); i != elementsToRemove.end(); ++i) {
        nodeList.erase(*i);
    }
}

/// Builds a tree of nodes from the given equation node list and returns the root node of the tree. This will modify the
/// contents of the list. This will throw an exception if the node list represents an invalid equation.
/// @param nodeList The list of nodes representing the operands and operators in the order that they appear in the
///     original equation.
Node* buildTreeFromNodes(std::list<Node*>& nodeList) noexcept(false) {
    // perform order of operations and combine nodes together into an equation tree until only one node is left in the
    // node this. That node is the root of our equation tree
    processNegationNode(nodeList);
    processOperatorNodes(nodeList, Power, Power);
    processOperatorNodes(nodeList, Multiplication, Division);
    processOperatorNodes(nodeList, Addition, Subtraction);
    
    // if there isn't only one element in the list by this point, then something went wrong
    if (nodeList.size() != 1) {
        throw UnknownError;
    }
    
    // return the equation tree root node
    return *nodeList.begin();
}

/// Builds a binary tree representing the given equation.
/// @param equation The equation string to parse.
/// @param isSubGroup Whether or not the given equation is a subgroup of a larger equation.
Node* buildEquationTree(std::string equation, bool isSubGroup) noexcept(false) {
    // handle empty equation
    if (equation.empty()) {
        if (isSubGroup) {
            throw EmptyGroup;
        }
        else {
            return nullptr;
        }
    }
    
    // Parse through equation string and build list of nodes containing operands and operators.
    std::list<Node*> equationNodes;
    
    // handle possible negate operator at beginning of equation
    Node* negateNode = popNegateFromEquation(equation);
    if (negateNode != nullptr) {
        equationNodes.push_back(negateNode);
    }
    
    // Now pop all (operand)(operator)(operand) nodes until the equation is empty
    // The equation must be in this pattern, otherwise it is not a valid equation
    equationNodes.push_back(popOperandFromEquation(equation));
    while (!equation.empty()) {
        equationNodes.push_back(popOperatorFromEquation(equation));
        equationNodes.push_back(popOperandFromEquation(equation));
    }
    
    // Build the equation tree from these nodes and return it
    return buildTreeFromNodes(equationNodes);
}



////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// ENTRY FUNCTIONS

/// Builds a binary tree representing the given equation and returns the tree root node.
/// @param equation The equation string to parse.
Node* buildEquationTree(std::string equation) noexcept(false) {
    // remove all whitespace
    equation.erase(std::remove_if(equation.begin(), equation.end(), isspace), equation.end());
    return buildEquationTree(equation, false);
}

/// Evaluates the equation represented by the given equation tree and returns the number result.
/// @param tree A binary tree representing an equation.
double evaluateEquationTree(const Node* tree) {
    // base case, there is no tree
    if (tree == nullptr) {
        return 0;
    }
    
    // base case, if a literal then just return it
    else if (tree->isLiteral) {
        return tree->liternal;
    }
    
    // otherwise, we have an operator, so recurse left and right down the tree and perform the correct operation
    else {
        switch (tree->operatorSymbol) {
            case Addition:
                return evaluateEquationTree(tree->left) + evaluateEquationTree(tree->right);
            case Subtraction:
                return evaluateEquationTree(tree->left) - evaluateEquationTree(tree->right);
            case Multiplication:
                return evaluateEquationTree(tree->left) * evaluateEquationTree(tree->right);
            case Division:
                return evaluateEquationTree(tree->left) / evaluateEquationTree(tree->right);
            case Power:
                return pow(evaluateEquationTree(tree->left), evaluateEquationTree(tree->right));
            case Negate:
                return -evaluateEquationTree(tree->right);
            default:
                return 0;
        }
    }
}

/// Evaluates the given equation and returns the number result.
/// @param equation The equation to evaluate.
double evaluateEquation(std::string equation) noexcept(false) {
    Node* tree = buildEquationTree(equation);
    return evaluateEquationTree(tree);
}
