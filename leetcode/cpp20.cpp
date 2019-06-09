#include <stack>

using namespace std;

class Solution {
public:
    bool isValid(string s) {
        
        stack<char> st;
        
        string::iterator it = s.begin();
        for (; it < s.end(); it++) {
            char c = *it;
            if (('(' == c) || ('[' == c) || ('{' == c)) {
                st.push(c);
            } else {
                if (st.empty()) {
                    return false;
                }
                char top = st.top();
                if ((')' == c) && ('(' != top)) {
                    return false;
                }
                if ((']' == c) && ('[' != top)) {
                    return false;
                }
                if (('}' == c) && ('{' != top)) {
                    return false;
                }
                st.pop();
            }
        }
        return st.size() == 0 ? true : false;
    }
};
