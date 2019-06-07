class Solution {
public:
    bool backspaceCompare(string S, string T) {
        char stack_s[201] = {'\0'};
        int head_s = 0;
        string::iterator it_s = S.begin();
        for (;it_s != S.end(); it_s++) {
            char c_s = *it_s;
            if (c_s != '#') {
                head_s += 1;
                stack_s[head_s] = c_s;
            } else {
                if (head_s == 0) {
                    continue;
                }
                stack_s[head_s] = '\0';
                head_s -= 1;
            }
        }
        
        char stack_t[201] = {'\0'};
        int head_t = 0;
        string::iterator it_t = T.begin();
        for (;it_t != T.end(); it_t++) {
            char c_t = *it_t;
            if (c_t != '#') {
                head_t += 1;
                stack_t[head_t] = c_t;
            } else {
                if (head_t == 0) {
                    continue;
                }
                stack_t[head_t] = '\0';
                head_t -= 1;
            }
        }

        if (head_s != head_t) {
            return false;
        }
        int i;
        for (i = 0; i <= head_s; i++) {
            if (stack_s[i] != stack_t[i]) {
                return false;
            }
        }
        return true;
    }
};
