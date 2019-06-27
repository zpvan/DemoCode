/**
 * Definition for a binary tree node.
 * struct TreeNode {
 *     int val;
 *     TreeNode *left;
 *     TreeNode *right;
 *     TreeNode(int x) : val(x), left(NULL), right(NULL) {}
 * };
 */
class Solution {
public:
    int minDepth(TreeNode* root) {
        if (!root) return 0;
        return _dfs(0, root);
    }
private:
    int _dfs(int level, TreeNode* r) {
        if (!r->left && !r->right) {
            return level + 1;
        }
        
        int m_level = INT_MAX;
        if (r->left) {
            m_level = min(m_level, _dfs(level + 1, r->left));
        }
        
        if (r->right) {
            m_level = min(m_level, _dfs(level + 1, r->right));
        }
        
        return m_level;
    }
};