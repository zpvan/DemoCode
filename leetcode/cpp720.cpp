class Solution {
public:
    string longestWord(vector<string>& words) {
        sort(words.begin(), words.end());
        unordered_set<string> s;
        string str;
        for (string word : words) {
            if (word.size() == 1 || s.count(word.substr(0, word.size() - 1))) {
                str = word.size() > str.size() ? word : str;
                s.insert(word);
            }
        }
        return str;
    }
};
