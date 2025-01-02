#include <iostream>
#include <string>
#include <vector>
#include <stack>
#include <variant>
#include <unordered_map>
#include <cassert>
#include <cmath>

using namespace std;

#define ASSERT(condition, msg) if (!(condition)) throw string(msg);

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
            throw string("");
        }

        return EvaluateRPN(rpn);
    }

    vector<Token> ToRPN(const string& expression) {
        vector<Token> output;
        stack<char> operators;
        
        bool expect_operand = true;
        bool in_decimal = false;
        bool in_negative = false;
        string current_num = "";

        auto add_num = [&output, &current_num, &expect_operand, &in_decimal, &in_negative]() -> void {
            if (!current_num.empty() && current_num.back() != '-') { 
                ASSERT(expect_operand, "Unexpected digit");

                output.push_back({ stod(current_num) });
                
                current_num = "";
                in_decimal = false;
                in_negative = false;
                expect_operand = false;
            }
        };

        for (char token : expression) {
            if (nums.find(token) != string::npos) {
                current_num += string(1, token);
            }
            
            else if (token == '.') {
                ASSERT(!in_decimal, "Unexpected floating point");

                in_decimal = true;
                current_num += string(1, token);
            }
            
            else {
                add_num();

                if (ops[token]) {
                    if (token == '-' && expect_operand) {
                        ASSERT(!in_negative, "Unexpected negative sign");

                        current_num += string(1, token);

                        in_negative = true;

                        continue;
                    }

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

                    expect_operand = true;
                }

                else if (token == ')') {
                    while (!operators.empty() && operators.top() != '(') {
                        output.push_back({ operators.top() });
                        operators.pop(); 
                    }
                    
                    ASSERT(!operators.empty(), "Mismatched parentheses");

                    operators.pop();
                }

                else {
                    ASSERT(token == ' ', "Invalid token");
                }
            }
        }

        add_num();

        ASSERT(!in_negative, "Unexpected operator");

        if (!output.empty())
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
        
        try {
            auto res = Parser.Parse(exp);

            cout << "Result: " << res << endl;
        }

        catch (const string& error) {
            if (error.size())
                cout << "Error: " << error << endl;
        }
    }
}