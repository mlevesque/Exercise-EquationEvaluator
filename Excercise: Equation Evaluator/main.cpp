//
//  main.cpp
//  Excercise: Equation Evaluator
//
//  Created by Michael Levesque on 12/20/19.
//  Copyright © 2019 Michael Levesque. All rights reserved.
//

#include "evaluator.h"
#include "test_cases.h"
#include <iostream>

int main(int argc, const char * argv[]) {
    // prompt for input
    std::string equation;
    double result;
    std::cout << "Enter equation: ";
    std::cin >> equation;

    // perform test if input was "test"
    if (equation == "test") {
        testEquationEvaluator();
    }

    // otherwise perform evaluation and output result
    else {
        try {
            result = evaluateEquation(equation);
            std::cout << "Result: " << result;
        }
        catch (...) {
            std::cout << "Error: Invalid Equation!";
        }
    }
    std::cout << std::endl;
    return 0;
}
