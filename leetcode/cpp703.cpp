class KthLargest {
public:
    KthLargest(int k, vector<int>& nums) {
        mK = k;
        mRes = INT_MAX;
        if (nums.empty()) {
            return;
        }
        auto it_n = nums.begin();
        int i = 0;
        for(; it_n < nums.end(); it_n++) {
            int val = *it_n;
            if (i < mK) {
                // mKnums is not full
                if (val < mRes) {
                    mRes = val;
                }
                mKnums.push_back(val);
            } else {
                // mKnums is full
                if (val > mRes) {
                    // find the mininum element
                    auto it_k = mKnums.begin();
                    
                    int j = 0;
                    int mMin;
                    mMin = mKnums.at(j);
                    int j_pos = j;
                    for (j = 0; j < mK; j++) {
                        if (mMin > mKnums.at(j)) {
                            mMin = mKnums.at(j);
                            j_pos = j;
                        }
                    }
                    
                    mKnums.erase((it_k + j_pos));
                    mKnums.insert((it_k + j_pos), val);
                    
                    j = 0;
                    mMin = mKnums.at(j);
                    j_pos = j;
                    for (j = 0; j < mK; j++) {
                        if (mMin > mKnums.at(j)) {
                            mMin = mKnums.at(j);
                            j_pos = j;
                        }
                    }
                    
                    mRes = val < mMin ? val : mMin;
                }
            }
            i++;
        }
        
    }
    
    int add(int val) {
        
        if (mKnums.size() < mK) {
            // mKnums is not full
            if (val < mRes) {
                mRes = val;
            }
            mKnums.push_back(val);
            return mRes;
        }
        
        // mKnums is full
        if (val <= mRes) {
            return mRes;
        }
        
        // find the mininum element
        auto it_k = mKnums.begin();
                  
        int mMin;
        int j = 0;
        mMin = mKnums.at(j);
        int j_pos = j;
        int sMin = mMin;
        for (j = 0; j < mK; j++) {
            if (mMin > mKnums.at(j)) {
                sMin = mMin;
                mMin = mKnums.at(j);
                j_pos = j;
            }
        }
        mKnums.erase((it_k + j_pos));
        mKnums.insert((it_k + j_pos), val);
        
        j = 0;
        mMin = mKnums.at(j);
        j_pos = j;
        for (j = 0; j < mK; j++) {
            if (mMin > mKnums.at(j)) {
                mMin = mKnums.at(j);
                j_pos = j;
            }
        }
        
        mRes = val < mMin ? val : mMin;
        return mRes;
    }
    
private:
    int mRes;
    int mK;
    vector<int> mKnums;
};

/**
 * Your KthLargest object will be instantiated and called as such:
 * KthLargest* obj = new KthLargest(k, nums);
 * int param_1 = obj->add(val);
 */
