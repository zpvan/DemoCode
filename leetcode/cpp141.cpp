/**
 * Definition for singly-linked list.
 * struct ListNode {
 *     int val;
 *     ListNode *next;
 *     ListNode(int x) : val(x), next(NULL) {}
 * };
 */

#include <set>
using namespace std;

class Solution {
public:
    bool hasCycle(ListNode *head) {
        set<ListNode *>s;
        while (head) {
            if (s.find(head) == s.end()) {
                s.insert(head);
                head = head->next;
            } else {
                return true;
            }
        }
        return false;
    }
};
