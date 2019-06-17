class Solution {
public:
    vector<int> twoSum(vector<int>& nums, int target) {
        map<int, int> m;
        for (auto it = nums.begin(); it != nums.end(); it++) {
            auto it2 = m.find(target - *it);
            if (it2 != m.end()) return {it2->second, it - nums.begin()};
            m.insert(pair<int, int>(*it, it - nums.begin()));
        }
        return {};
    }
};
