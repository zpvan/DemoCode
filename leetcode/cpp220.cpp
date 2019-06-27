class Solution {
public:
    bool containsNearbyAlmostDuplicate(vector<int>& nums, int k, int t) {
        set<long> s;
        if (!k) return false;
        for (int i = 0; i < nums.size(); i++) {
            if (s.size() > k) s.erase(s.find(nums[i - k - 1]));
            auto itu = s.lower_bound(nums[i]);
            if (itu != s.end() && (*itu) - nums[i] <= t) {
                return true;
            }

            s.insert(nums[i]);
            
            auto l = s.find(nums[i]);
            if (l != s.begin()) {
                l--;
                if (nums[i] - (*l) <= t) {
                    return true;
                } 
            }
        }
        return false;
    }
};
