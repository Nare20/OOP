// algorithm for postfix
/*for each token:
    if number:
        push
    else operator:
        b = pop
        a = pop
        push(a op b)
        */
string infixToPostfix(string s)
{
    stack<char> st;
    string postfix;

    for(char c : s)
    {
        if(isdigit(c))
            postfix += c;

        else if(c == '(')
            st.push(c);

        else if(c == ')')
        {
            while(st.top() != '(')
            {
                postfix += st.top();
                st.pop();
            }
            st.pop();
        }

        else
        {
            while(!st.empty() && precedence(st.top()) >= precedence(c))
            {
                postfix += st.top();
                st.pop();
            }

            st.push(c);
        }
    }

    while(!st.empty())
    {
        postfix += st.top();
        st.pop();
    }

    return postfix;
}