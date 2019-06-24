class Solution {
public:
    int lengthOfLongestSubstring(string s) {
        set<char> S;
        deque<char> D;
        int max = 0;
        for (char c : s) {
            while (S.end() != S.find(c)) {
                if (S.size() > max) max = S.size();
                auto it = S.find(D.front());
                S.erase(it);
                D.pop_front();
            }
            S.insert(c);
            D.push_back(c);
        }
        if (S.size() > max) max = S.size();
        return max;
    }
};
