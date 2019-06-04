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
    ListNode* reverseList(ListNode* head) {
        ListNode *head_prev = NULL;
        while (head) {
            ListNode *head_next = head->next;
            head->next = head_prev;
            head_prev = head;
            head = head_next;
        }
        return head_prev;
    }
};
