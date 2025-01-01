#include <iostream>
#include <string>
#include <vector>
#include <stack>
#include <variant>
#include <unordered_map>
#include <cassert>
#include <cmath>

using namespace std;

#define ASSERT(condition, msg) \
    if (!(condition)) { \
        cout << "Error: " << msg << endl; \
        output.clear(); \
        return output; \
    } \

typedef variant<double, char> Token;

struct {
    string nums = "0123456789";
    unordered_map<char, int> ops = {
        { '+', 1 },
        { '-', 1 },
        { '*', 2 },
        { '/', 2 },
        { '^', 3 }
    };

    double Parse(const string& expression) {
        auto rpn = ToRPN(expression);

        if (rpn.empty()) {
            return NULL;
        }

        return EvaluateRPN(rpn);
    }

    vector<Token> ToRPN(const string& expression) {
        vector<Token> output;
        stack<char> operators;
        
        bool expect_operand = true;
        bool in_decimal = false;
        string current_num = "";

        auto add_num = [&output, &current_num, &expect_operand, &in_decimal]() -> void {
            if (!current_num.empty()) {
                output.push_back({ stod(current_num) });
                
                current_num = "";
                in_decimal = false;
                expect_operand = false;
            }
        };

        for (char token : expression) {
            if (nums.find(token) != string::npos) {
                current_num += string(1, token);
                
                expect_operand = false;
            }
            
            else if (token == '.') {
                in_decimal = true;
                current_num += string(1, token);
            }
            
            else {
                add_num();

                if (ops[token]) {
                    ASSERT(!expect_operand, "Unexpected operator");

                    while (
                        !operators.empty() && operators.top() != '(' &&
                        ops[operators.top()] >= ops[token]
                    ) {
                        output.push_back({ operators.top() });
                        operators.pop();
                    }

                    operators.push(token);
                    expect_operand = true;
                }

                else if (token == '(') {
                    operators.push(token);
                    expect_operand = false;
                }

                else if (token == ')') {
                    while (!operators.empty() && operators.top() != '(') {
                        output.push_back({ operators.top() });
                        operators.pop();
                        
                        ASSERT(!operators.empty(), "Mismatched parentheses");
                    }

                    operators.pop();
                }

                else {
                    ASSERT(token == ' ', "Invalid token");
                }
            }
        }

        add_num();

        ASSERT(!expect_operand, "Unexpected operator");

        while (!operators.empty()) {
            ASSERT(operators.top() != '(', "Mismatched parentheses");

            output.push_back({ operators.top() });

            operators.pop();
        }

        return output;
    }

    double EvaluateRPN(const vector<Token>& rpn) {
        stack<double> s;

        for (Token token : rpn) {
            if (holds_alternative<double>(token)) {
                s.push(get<double>(token));
            }

            else if (holds_alternative<char>(token)) {
                double b = s.top();
                s.pop();

                double a = s.top();
                s.pop();

                double res;

                switch (get<char>(token)) {
                    case '+':
                        res = a + b;
                        break;
                    case '-':
                        res = a - b;
                        break;
                    case '*':
                        res = a * b;
                        break;
                    case '/':
                        res = a / b;
                        break;
                    case '^':
                        res = pow(a, b);
                        break;
                }  

                s.push(res); 
            }
        }

        return s.top();
    }
} Parser;

int main() {
    while (1) {
        string exp;

        cout << "Expression: ";
        getline(cin, exp);
        
        double res = Parser.Parse(exp);

        if (res != NULL)
            cout << "Result: " << res << endl;
    }
}