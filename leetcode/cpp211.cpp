class WordDictionary {
public:
    /** Initialize your data structure here. */
    WordDictionary() {
        
    }
    
    /** Adds a word into the data structure. */
    void addWord(string word) {
        WordDictionary *cur = this;
        for (char c : word) {
            if (!cur->next[c - 'a']) cur->next[c - 'a'] = new WordDictionary();
            cur = cur->next[c - 'a'];
        }
        cur->isWord = true;
    }
    
    /** Returns if the word is in the data structure. A word could contain the dot character '.' to represent any one letter. */
    bool search(string word) {        
        return _dfs(0, word, this);
    }
    
    bool _dfs(int idx, string word, WordDictionary *t) {
        if (word.empty() || idx >= word.size() || !t) return false;
        if (idx == word.size() - 1) {
            if (word[idx] == '.') {
                for (int i = 0; i < 26; i++)
                    if (t->next[i] && t->next[i]->isWord) return true;
            } else if (t->next[word[idx] - 'a'] && t->next[word[idx] - 'a']->isWord) return true;
            return false;
        }
        if (word[idx] == '.') {
            for (int i = 0; i < 26; i++)
                if (_dfs(idx + 1, word, t->next[i])) return true;
        } else if (_dfs(idx + 1, word, t->next[word[idx] - 'a'])) return true;
        return false;
    }
    
private:
    WordDictionary *next[26] = {};
    bool isWord = false;
};

/**
 * Your WordDictionary object will be instantiated and called as such:
 * WordDictionary* obj = new WordDictionary();
 * obj->addWord(word);
 * bool param_2 = obj->search(word);
 */