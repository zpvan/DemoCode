class Solution {
public:
    string countOfAtoms(string formula) {
        map<string, int> m;
        stack<int> st_times;
        st_times.push(1);
        int num = 1;
        stack<char> st_word;
        bool lastCharIsDigit = false;
        for (int i = formula.size() - 1; i >= 0; i--) {
            if (isdigit(formula[i])) {
                // 数字
                num = lastCharIsDigit ? num + ((formula[i] - '0') * 10) : (formula[i] - '0');
                printf("digit %d\n", num);
                lastCharIsDigit = true;
            } else if (formula[i] == ')') {
                // 右括号
                st_times.push(st_times.top() * num);
                lastCharIsDigit = false;
                num = 1;
            } else if (formula[i] == '(') {
                // 左括号
                st_times.pop();
                lastCharIsDigit = false;
                num = 1;
            } else if (formula[i] < 'a') {
                // 大写字母
                
                
                string s;
                s.push_back(formula[i]);
                while (!st_word.empty()) {
                    s.push_back(st_word.top());
                    st_word.pop();
                }
                printf("map ele-1 %d\n",m[s]);
                printf("ele %s %d\n",s.c_str(), num );
                m[s] += num * st_times.top();
                // m[formula[i]] += num;
                printf("map ele-2 %d\n",m[s]);
                lastCharIsDigit = false;
                num = 1;
            } else {
                // 小写字母
                st_word.push(formula[i]);
                lastCharIsDigit = false;
            }
        }
        printf("m.size() %d\n", m.size());
        
        string res = "";
        for (auto p : m) {
            res.append(p.first);
            if (p.second != 1) res.append(to_string(p.second));
            printf("pair %s, %i\n", p.first.c_str(), p.second);
        }
        return res;
        
    }
};