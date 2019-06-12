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
        if (!root) {
            return 0;
        }
        Dfs(1, root);
        return mMin;
    }
    
private:
    void Dfs(int level, TreeNode *node) {
        if (NULL == node) {
            return;
        }
        
        Dfs(level + 1, node->left);
        Dfs(level + 1, node->right);
        
        if (!node->left && !node->right && (mMin > level)) {
            mMin = level;
        }
    }
    
    int mMin = INT_MAX;
};
