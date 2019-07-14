class Solution {
public:
    class Trie {
    public:
        void insert(string word) {
            Trie *cur = this;
            for (char c : word) {
                if (!cur->next[c - 'a']) cur->next[c - 'a'] = new Trie();
                cur = cur->next[c - 'a'];
            }
            cur->isWord = true;
        }
        
        bool search(string word) {
            Trie *cur = this;
            for (char c : word) {
                if (!cur->next[c - 'a']) return false;
                cur = cur->next[c - 'a'];
            }
            return cur->isWord;
        }
        
        Trie *next[26] = {};
        bool isWord = false;
    };
    
    vector<string> findWords(vector<vector<char>>& board, vector<string>& words) {
        vector<string> res;
        if (board.empty()) return res;
        
        Trie t;
        for (string word : words) {
            t.insert(word);
        }

        unordered_set<string> s;
        for (int i = 0; i < board.size(); i++) {
            for (int j = 0; j < board[0].size(); j++) {
                _dfs(board, &t, s, i, j, "");
            }
        }
        for (string str : s) {
            res.push_back(str);
        }
        return res;
    }
    
    void _dfs(vector<vector<char>>& board, Trie *t, unordered_set<string>& res, int x, int y, string str) {
        if (x < 0 || x >= board.size() || y < 0 || y >= board[0].size()) return;
        char c = board[x][y];
        if (c == '.') return;
        if (!t->next[c - 'a']) return;
        str.push_back(c);
        if (t->next[c - 'a']->isWord) res.insert(str);
        string org = str;
        board[x][y] = '.';
        _dfs(board, t->next[c - 'a'], res, x - 1, y, org);
        _dfs(board, t->next[c - 'a'], res, x + 1, y, org);
        _dfs(board, t->next[c - 'a'], res, x, y - 1, org);
        _dfs(board, t->next[c - 'a'], res, x, y + 1, org);
        board[x][y] = c;
    }
};
