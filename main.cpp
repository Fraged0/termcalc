#include <iostream>
#include <stack>
#include <string>
#include <cctype>
#include <cmath>
#include <map>
#include <csignal>

int precedence(char op) {
    if (op == '+' || op == '-') return 1;
    if (op == '*' || op == '/') return 2;
    return 0;
}

double applyOp(double a, double b, char op) {
    switch (op) {
        case '+': return a + b;
        case '-': return a - b;
        case '*': return a * b;
        case '/': 
            if (b == 0) throw std::runtime_error("division by zero");
            return a / b;
    }
    return 0;
}

double evaluate(const std::string& expression) {
    std::stack<double> values;
    std::stack<char> ops;
    
    for (size_t i = 0; i < expression.length(); i++) {
        if (isspace(expression[i]))
            continue;

        if (isdigit(expression[i])) {
            double val = 0;
            int decimal_places = -1;
            bool is_decimal = false;

            while (i < expression.length() && (isdigit(expression[i]) || expression[i] == '.')) {
                if (expression[i] == '.') {
                    is_decimal = true;
                } else {
                    val = (val * 10) + (expression[i] - '0');
                    if (is_decimal) {
                        decimal_places *= 10;
                    }
                }
                i++;
            }
    
            if (is_decimal) {
                val /= -decimal_places;
            }
    
            values.push(val);
            i--;
        }
        else if (expression[i] == '(') {
            ops.push(expression[i]);
        }
        else if (expression[i] == ')') {
            while (!ops.empty() && ops.top() != '(') {
                double val2 = values.top(); values.pop();
                double val1 = values.top(); values.pop();
                char op = ops.top(); ops.pop();
                values.push(applyOp(val1, val2, op));
            }
        }
        else if (expression[i] == '+' || expression[i] == '-' || expression[i] == '*' || expression[i] == '/') {
            while (!ops.empty() && precedence(ops.top()) >= precedence(expression[i])) {
                double val2 = values.top(); values.pop();
                double val1 = values.top(); values.pop();
                char op = ops.top(); ops.pop();
                values.push(applyOp(val1, val2, op));
            }
            ops.push(expression[i]);
        } else {
            throw std::runtime_error("invalid char in expression");
        }
    }
    
    while (!ops.empty()) {
        double val2 = values.top(); values.pop();
        double val1 = values.top(); values.pop();
        char op = ops.top(); ops.pop();
        values.push(applyOp(val1, val2, op));
    }
    
    return values.top();
}

void handle_segfault(int sig) {
    std::cout << "\033[1;31m> E - segmentation fault <\033[0m" << std::endl;
    exit(1);
}

int main() {
    signal(SIGSEGV, handle_segfault);

    std::cout << "\033[37mwelcome to\033[1;36m termcalc\033[0;0m!" << std::endl << std::endl;
    
    std::string expression;
    do {
        try {
            std::cout << ">>> ";
            getline(std::cin, expression);
            if (expression != "") {
                double result = evaluate(expression);
                std::cout << "\033[1;32m> " << result << " <\033[0;0m" << std::endl;
            }
        } catch (const std::exception& e) {
            std::cout << "\033[1;31m> E - " << e.what() << " <\033[0m" << std::endl;
        }
    } while (true);
    return 0;
}
