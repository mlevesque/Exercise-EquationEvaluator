//
//  evaluator.h
//  Excercise: Equation Evaluator
//
//  Created by Michael Levesque on 12/20/19.
//  Copyright © 2019 Michael Levesque. All rights reserved.
//

#ifndef evaluator_h
#define evaluator_h

#include <string>

/// Exception error types that can be thrown during equation parsing.
enum ErrorType {
    InvalidOperand,
    InvalidOperator,
    EmptyGroup,
    MissingClosingSymbol,
    UnknownError
};

/// Type indicator for operator nodes.
enum OperatorType {
    Addition = '+',
    Subtraction = '-',
    Multiplication = '*',
    Division = '/',
    Power = '^',
    Negate = '~'
};

/// Node representing an element of an equation tree.
struct Node {
    Node();
    
    union {
        char operatorSymbol;
        double liternal;
    };
    bool isLiteral;
    
    Node* left;
    Node* right;
};

Node* buildEquationTree(std::string equation) noexcept(false);
double evaluateEquationTree(const Node* tree) noexcept(false);
double evaluateEquation(std::string equation) noexcept(false);

#endif /* evaluator_h */
