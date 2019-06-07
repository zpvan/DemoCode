#include <queue>

using namespace std;

class MyStack {
public:
    /** Initialize your data structure here. */
    MyStack() {
        
    }
    
    /** Push element x onto stack. */
    void push(int x) {
        main_queue.push(x);
    }
    
    /** Removes the element on top of the stack and returns that element. */
    int pop() {
        if (main_queue.empty()) {
            while (!back_queue.empty()) {
                main_queue.push(back_queue.front());
                back_queue.pop();
            }
        }
        while (main_queue.size() > 1) {
            back_queue.push(main_queue.front());
            main_queue.pop();
        }
        int front = main_queue.front();
        main_queue.pop();
        return front;
    }
    
    /** Get the top element. */
    int top() {
        if (main_queue.empty()) {
            while (!back_queue.empty()) {
                main_queue.push(back_queue.front());
                back_queue.pop();
            }
        }
        while (main_queue.size() > 1) {
            back_queue.push(main_queue.front());
            main_queue.pop();
        }
        int front = main_queue.front();
        return front;
    }
    
    /** Returns whether the stack is empty. */
    bool empty() {
        return main_queue.empty() && back_queue.empty();
    }
    
private:
    queue<int> main_queue;
    queue<int> back_queue;
};

/**
 * Your MyStack object will be instantiated and called as such:
 * MyStack* obj = new MyStack();
 * obj->push(x);
 * int param_2 = obj->pop();
 * int param_3 = obj->top();
 * bool param_4 = obj->empty();
 */
