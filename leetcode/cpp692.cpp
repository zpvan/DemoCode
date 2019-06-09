#include <map>

using namespace std;

class Solution {
public:
    vector<string> topKFrequent(vector<string>& words, int k) {
        map<string, int> wordsMap;
        
        for (vector<string>::iterator it = words.begin();
             it != words.end(); it++) {
            string word = *it;
            auto it_wsm = wordsMap.find(word);
            if (it_wsm != wordsMap.end()) {
                // exist element
                it_wsm->second++;
            } else {
                // new element
                wordsMap.insert(pair<string, int>(word, 1));
            }
            
        }
        
        vector<pair<string, int>> wordsSort;
        sortMap(wordsMap, wordsSort);
        
        vector<string> topWords;
        int i = 0;
        for (vector<pair<string, int>>::iterator it = wordsSort.begin();
             it != wordsSort.end(); it++) {
            if (i >= k) {
                break;
            }
            topWords.push_back(it->first);
            i++;
        }
        
        return topWords;
    }
    
private:
    static bool cmp(pair<string, int>& p1, pair<string, int>& p2) {
        if (p1.second != p2.second) {
            return p1.second > p2.second;
        } else {
            return strcmp(p2.first.c_str(), p1.first.c_str()) >= 0;
        }
    }
    
    void sortMap(map<string, int>& m, vector<pair<string, int>>& v) {
        for (map<string, int>::iterator it = m.begin(); it != m.end(); it++) {
            v.push_back(*it);
        }
        
        sort(v.begin(), v.end(), cmp);
    }
};
