//
//  test_cases.cpp
//  Excercise: Equation Evaluator
//
//  Series of test cases for testing the equation evaluator code.
//
//  Created by Michael Levesque on 1/10/20.
//  Copyright © 2020 Michael Levesque. All rights reserved.
//

#include "test_cases.h"
#include "evaluator.h"
#include <iostream>
#include <cmath>
#include <limits>

const double epsilon = 0.0000001;

void outputTestResult(std::string title, bool pass) {
    std::string result = pass ? "[PASS] " : "[FAIL] ";
    std::cout << result << title << std::endl;
}

bool doesEqual(double a, double b) {
    if ((isinf(a) && isinf(b)) || (isnan(a) && isnan(b))) {
        return true;
    }
    return fabs(a - b) < epsilon;
}

void testValidEquation(std::string equation, double expected, std::string title) {
    std::string extendedTitle = title;
    bool pass;
    try {
        double actual = evaluateEquation(equation);
        pass = doesEqual(expected, actual);
        if (!pass) {
            extendedTitle += " - Expected: " + std::to_string(expected) + " Actual: " + std::to_string(actual);
        }
    }
    catch (...) {
        pass = false;
        extendedTitle += " - Unexpected exception!";
    }
    outputTestResult(extendedTitle, pass);
}

void testInvalidEquation(std::string equation, ErrorType expected, std::string title) {
    std::string extendedTitle = title;
    bool pass;
    try {
        evaluateEquation(equation);
        extendedTitle += " - Did not throw expected exception!";
        pass = false;
    }
    catch (const ErrorType& e) {
        pass = e == expected;
        if (!pass) {
            extendedTitle += " - Expected Exception: " + std::to_string(expected) + " Actual: " + std::to_string(e);
        }
    }
    outputTestResult(extendedTitle, pass);
}

void testEquationEvaluator() {
    testValidEquation("", 0, "Empty Equation");
    testValidEquation("1", 1, "Single Number");
    testValidEquation("2+2", 4, "Simple Addition");
    testValidEquation("2-1", 1, "Simple Subtraction");
    testValidEquation("2*3", 6, "Simple Multiplication");
    testValidEquation("6/2", 3, "Simple Division");
    testValidEquation("2^3", 8, "Simple Power");
    testValidEquation("-2", -2, "Simple Negate");
    testValidEquation("(2)", 2, "Simple Group");
    testValidEquation("(-2)", -2, "Nested Negate");
    testValidEquation("(-2)+(-6)", -8, "Multiple Nested Negates");
    testValidEquation("2*(1+1)", 4, "PEMDAS Test 1");
    testValidEquation("2*1+1", 3, "PEMDAS Test 2");
    testValidEquation("2/(1+1)^3", 0.25, "PEMDAS Test 3");
    testValidEquation("3*4-3", 9, "PEMDAS Test 4");
    testValidEquation("3+4*3", 15, "PEMDAS Test 5");
    testValidEquation("-4^3/5*(2+1)", -38.4, "PEMDAS Test 5");
    testValidEquation("(1+3)*(4+5)", 36, "PEMDAS Test 6");
    testValidEquation("-(-(3))", 3, "Nested Groups 1");
    testValidEquation("4*(5-(1+3)/(5*6))*(4-1)", 58.4, "Nested Groups 2");
    testValidEquation("(-1)^(0.5)", std::numeric_limits<double>::quiet_NaN(), "NaN 1");
    testValidEquation("0/0", std::numeric_limits<double>::quiet_NaN(), "NaN 2");
    testValidEquation("1/0", std::numeric_limits<double>::infinity(), "Infinity");
    testValidEquation(" 2 + 7  *  ( 3 +1) ", 30, "Equation with Whitespace");

    testInvalidEquation("()", EmptyGroup, "Empty Group Exception 1");
    testInvalidEquation("3+()", EmptyGroup, "Empty Group Exception 2");
    testInvalidEquation("1--3", InvalidOperand, "Invalid Operand 1");
    testInvalidEquation("--3", InvalidOperand, "Invalid Operand 2");
    testInvalidEquation("n-3", InvalidOperand, "Invalid Operand 3");
    testInvalidEquation("4-", InvalidOperand, "Invalid Operand 4");
    testInvalidEquation("(4-3)*)", InvalidOperand, "Invalid Operand 5");
    testInvalidEquation("1&3", InvalidOperator, "Invalid Operator 1");
    testInvalidEquation("5-1&3", InvalidOperator, "Invalid Operator 2");
    testInvalidEquation("(4-3))", InvalidOperator, "Invalid Operator 3");
    testInvalidEquation("(4+3", MissingClosingSymbol, "Missing Closing Symbol 1");
    testInvalidEquation("((4+(5*2))-3", MissingClosingSymbol, "Missing Closing Symbol 2");
}
