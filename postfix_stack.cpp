#include <iostream>
#include <stack>
#include <vector>
#include <string>
#include <cctype>

using namespace std;

int precedence(char op) {
    if (op == '+' || op == '-') return 1;
    if (op == '*' || op == '/' || op == '%') return 2;
    return 0;
}

vector<string> infixToPostfix(const string& expr) {
    vector<string> output;
    stack<char> ops;

    for (int i = 0; i < (int)expr.size(); i++) {
        char c = expr[i];

        if (isspace(c)) continue;

        if (isdigit(c)) {
            string num;
            while (i < (int)expr.size() && isdigit(expr[i])) {
                num.push_back(expr[i]);
                i++;
            }
            i--;
            output.push_back(num);
        }
        else if (c == '(') {
            ops.push(c);
        }
        else if (c == ')') {
            while (!ops.empty() && ops.top() != '(') {
                output.push_back(string(1, ops.top()));
                ops.pop();
            }
            ops.pop();
        }
        else {
            while (!ops.empty() && precedence(ops.top()) >= precedence(c)) {
                output.push_back(string(1, ops.top()));
                ops.pop();
            }
            ops.push(c);
        }
    }

    while (!ops.empty()) {
        output.push_back(string(1, ops.top()));
        ops.pop();
    }

    return output;
}

int evalPostfix(const vector<string>& postfix) {
    stack<int> st;

    for (auto& token : postfix) {
        if (isdigit(token[0])) {
            st.push(stoi(token));
        } else {
            int b = st.top(); st.pop();
            int a = st.top(); st.pop();

            if (token == "+") st.push(a + b);
            else if (token == "-") st.push(a - b);
            else if (token == "*") st.push(a * b);
            else if (token == "/") st.push(a / b);
            else if (token == "%") st.push(a % b);
        }
    }

    return st.top();
}

int main() {
    string expr;
    cout << "Expression: ";
    getline(cin, expr);

    auto postfix = infixToPostfix(expr);

    cout << "Postfix: ";
    for (auto& t : postfix) cout << t << " ";
    cout << endl;

    cout << "Result = " << evalPostfix(postfix) << endl;
}