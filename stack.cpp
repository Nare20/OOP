#include<iostream>
#include<stack>
using namespace std;
int main(){

stack<int> st;
st.push(5);
st.push(10);
//cout << st.top() << endl; ;
//st.pop();
//cout << st.top() ;
if(st.empty())
    cout << "stack is empty" << endl;
else
    cout << "not empty" << endl;
cout << "stack size is " << st.size() << endl;
}