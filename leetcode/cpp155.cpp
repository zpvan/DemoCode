class MinStack {
public:
    /** initialize your data structure here. */
    MinStack() {
        
    }
    
    void push(int x) {
        s.push(x);
        m[x] += 1;
    }
    
    void pop() {
        int x = s.top();
        m[x] -= 1;
        if (m[x] == 0) {
            m.erase(x);
        }
        s.pop();
    }
    
    int top() {
        return s.top();
    }
    
    int getMin() {
        return m.begin()->first;
    }
    
private:
    stack<int> s;
    map<int, int> m;
};

/**
 * Your MinStack object will be instantiated and called as such:
 * MinStack* obj = new MinStack();
 * obj->push(x);
 * obj->pop();
 * int param_3 = obj->top();
 * int param_4 = obj->getMin();
 */
