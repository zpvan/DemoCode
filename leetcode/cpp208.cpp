class Trie {
public:
    
    /** Initialize your data structure here. */
    Trie() {
        
    }
    
    /** Inserts a word into the trie. */
    void insert(string word) {
        Trie *cur = this;
        for (char c : word) {
            if (!cur->next[c - 'a']) cur->next[c - 'a'] = new Trie();
            cur = cur->next[c - 'a'];
        }
        cur->isWord = true;
    }
    
    /** Returns if the word is in the trie. */
    bool search(string word) {
        Trie *cur = this;
        for (char c : word) {
            if (cur->next[c - 'a']) cur = cur->next[c - 'a'];
            else return false;
        }
        return cur->isWord;
    }
    
    /** Returns if there is any word in the trie that starts with the given prefix. */
    bool startsWith(string prefix) {
        Trie *cur = this;
        for (char c : prefix) {
            if (cur->next[c - 'a']) cur = cur->next[c - 'a'];
            else return false;
        }
        return true;
    }
    
private:
    bool isWord = false;
    Trie *next[26] = {};
};

/**
 * Your Trie object will be instantiated and called as such:
 * Trie* obj = new Trie();
 * obj->insert(word);
 * bool param_2 = obj->search(word);
 * bool param_3 = obj->startsWith(prefix);
 */
