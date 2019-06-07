/**
 * Definition for singly-linked list.
 * struct ListNode {
 *     int val;
 *     ListNode *next;
 *     ListNode(int x) : val(x), next(NULL) {}
 * };
 */
class Solution {
public:
    ListNode* swapPairs(ListNode* head) {
        ListNode *head_org = head;
        while (head) {
            ListNode *head_next = head->next;
            if (head_next) {
                int temp = head->val;
                head->val = head_next->val;
                head_next->val = temp;
                head = head_next->next;
            } else {
                break;
            }
        }
        return head_org;
    }
};
