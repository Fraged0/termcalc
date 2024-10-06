#include <iostream>
#include <fstream>
#include <stack>
#include <string>
#include <cctype>
#include <cmath>
#include <map>
#include <csignal>

bool ansicolors;
std::string history;

bool conv(std::string a) {
    if (a == "true") {
        return true;
    }
    else if (a == "false") {
        return false;
    }
    else {
        return false;
    }
}

std::map<std::string, std::string> readConfig(const std::string& filename) {
    std::map<std::string, std::string> config;
    std::ifstream configFile(filename);
    std::string line;

    if (configFile.is_open()) {
        while (getline(configFile, line)) {
            if (line.empty() || line[0] == '#') {
                continue;  // Skip comments and empty lines
            }

            size_t delimiterPos = line.find('=');
            std::string key = line.substr(0, delimiterPos);
            std::string value = line.substr(delimiterPos + 1);

            config[key] = value;
        }
        configFile.close();
    } else {
        std::cerr << "unable to open config file" << std::endl;
    }

    return config;
}

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
    if (ansicolors == true) {
        std::cout << "\033[1;31m> E - segmentation fault <\033[0m" << std::endl;
    }
    else {
        std::cout << "> E - segmentation fault <" << std::endl;
    }
    exit(1);
}

int main() {
    signal(SIGSEGV, handle_segfault);
    
    time_t now = time(0);
    tm *ltm = localtime(&now);
    std::string date = std::to_string(ltm->tm_mday) + "." + 
                   std::to_string(ltm->tm_mon + 1) + "." + 
                   std::to_string(1900 + ltm->tm_year);

    //std::cout << "reading config.ini" << std::endl;
    
    auto config = readConfig("config.ini");
    ansicolors = conv(config["ansicolors"]);
    
    std::ifstream in;
    in.open("history.txt");
    std::string line;
    in.close();
    
    bool noWriteDate = false;
    if (ansicolors == true) {
        std::cout << "welcome to\033[1;36m termcalc\033[0;0m!" << std::endl << std::endl;
    }
    else {
        std::cout << "welcome to termcalc!" << std::endl << std::endl;
    }
    
    std::string expression;
    do {
        try {
            std::cout << ">>> ";
            getline(std::cin, expression);
            if (expression != "") {
                double result = evaluate(expression);
                if (ansicolors == true) {
                    std::cout << "\033[1;32m> " << result << " <\033[0;0m" << std::endl;
                }
                else {
                    std::cout << "> " << result << " <" << std::endl;
                }
                std::ofstream out;
                out.open("history.txt", std::ios::app);
                if (noWriteDate == false) {
                    out << date << std::endl;
                }
                noWriteDate = true;
                out << "ex: " + expression + "    re: " + std::to_string(result) + "\n";
                out.close();
            }
        } catch (const std::exception& e) {
            if (ansicolors == true) {
                std::cout << "\033[1;31m> E - " << e.what() << " <\033[0m" << std::endl;
            } else {
                std::cout << "> E - " << e.what() << " <" << std::endl;
            }
        }
    } while (true);
    return 0;
}
