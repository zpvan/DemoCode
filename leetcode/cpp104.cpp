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
    int maxDepth(TreeNode* root) {
        Dfs(1, root);
        return mMax;
    }
    
private:
    void Dfs(int level, TreeNode *node) {
        if (!node) {
            if (mMax < (level -1)) {
                mMax = level - 1;
            }
            return;
        }
        
        Dfs(level + 1, node->left);
        Dfs(level + 1, node->right);
    }
    int mMax = 0;
};
