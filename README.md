# Exercise: Equation Evaluator

This is a simple C++ app that allows the user to input an equation with literals and operators and the app will output the evaluated result of that equation. The app will parse the equation string and builds it into a binary tree with respect to PEMDAS order of operations, then evaluates the tree recursively.

The app supports equations containing:
  - Parenthesis grouping symbols
  - Exponents
  - Multiplication and Division
  - Addition and Subtraction
  - Negate operator (only at the beginning of the equation or within grouping symbols)

## Development

This was just a fun little exercise that I thought about doing while on holiday vacation visiting family. I wanted to figure out how I could implement a simple app that can take an equation, break it down into a binary tree, then solve it.

The implementation is broken down into 3 parts (and these 3 parts can be repeated with recursion to support grouping symbols):
   - Parse the string and convert it to a list containing operands and operators in the order that they are in the string
   - Construct the binary tree from the operands and operators in the list
   - Recursively solve the equation binary tree

I have also added a number of test cases that served me well in identifying bugs. These test cases can be run by entering "test" when prompted to input an equation.
