#include <iostream>
using namespace std;


template <typename T>
class my_shared_ptr {
    private:
        int* count_;
        T* ptr_;
    
    public:
        my_shared_ptr() {
            count_ = nullptr;
            ptr_ = nullptr;
        }
    
        my_shared_ptr(T* origin_ptr)
            : ptr_(origin_ptr) {
            count_ = new int(1);
            log("my_shared_ptr(T*)");
        }

        my_shared_ptr(const my_shared_ptr& ptr)
            : ptr_(ptr.get()),
            count_(ptr.count_ptr()) {
            (*count_)++;
            log("my_shared_ptr(my_shared_ptr& ptr)");
        }

        my_shared_ptr& operator= (const my_shared_ptr& ptr) {
            if (this == &ptr) {
                return *this;
            } else {
                this->~my_shared_ptr();
            }
            ptr_ = ptr.get();
            count_ = ptr.count_ptr();
            (*count_)++;
            log("operator=");
            return *this;
        }

        int use_count() {
            return *count_;
        }
    
        T* get() const {
            return ptr_;
        }
    
        int* count_ptr() const {
            return count_;
        }

        ~my_shared_ptr() {
            if (count_) {
                (*count_)--;
                log("~my_shared_ptr");
                if (*count_ == 0) {
                    delete count_;
                    delete ptr_;
                }
            }
        }

        void log(string s) {
            cout << s << " count_: " << *count_ << endl;
        }
        
};