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
    ListNode* reverseKGroup(ListNode* head, int k) {
        bool first_reverse = false;
        ListNode *head_org = head;
        int i;
        ListNode *sublinked_head = NULL;
        ListNode *sublinked_tail = NULL;
        ListNode *sublinked_end = NULL;
        ListNode *prev = NULL;
        ListNode *cur = NULL;
        ListNode *next = NULL;
        ListNode *end = NULL;
        ListNode *org = NULL;
        ListNode *tail = NULL;
        do {
            // cut sublinked
            tail = org;
            sublinked_head = head;
            for (i = 0; i < k; i++) {
                if (!head) {
                    return head_org;
                }
                sublinked_tail = head;
                head = head->next;
            }
            sublinked_end = head;
            
            // reverse sublinked
            org = sublinked_head;
            cur = sublinked_head;
            end = sublinked_end;
            next = cur->next;
            while (end != next) {
                prev = cur;
                cur = next;
                next = next->next;
                cur->next = prev;
            }
            org->next = end;
            
            if (first_reverse) {
                tail->next = cur;
            } else {
                head_org = cur;
                first_reverse = true; 
            }
        } while(1);
    }
};
