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
    vector<vector<int>> levelOrder(TreeNode* root) {
        queue<TreeNode *> q;
        vector<vector<int>> vv;

        if (NULL == root) {
            return vv;
        }
        q.push(root);
        while (1) {
            if (q.empty()) {
                break;
            }
            vector<int> v;
            int cnt = q.size();
            for (int i = 0; i < cnt; i++) {
                TreeNode *cur = q.front();
                q.pop();
            
                v.push_back(cur->val);
                if (NULL != cur->left) {
                    q.push(cur->left);
                }
                if (NULL != cur->right) {
                    q.push(cur->right);
                }
            }
            vv.push_back(v);
        }
        return vv;
    }
};
