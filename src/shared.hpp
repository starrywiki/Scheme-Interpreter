#ifndef Shared_Ptr
#define Shared_Ptr
template <typename T>
class SharedPtr {
   private:
    T* _p;
    int* ucount;

   public:
    // Default constructor
    SharedPtr() : _p(nullptr), ucount(nullptr) {}
    // constructor with raw pointer
    SharedPtr(T* ptr) : _p(ptr), ucount(new int(1)) {}
    // Copy constructor
    SharedPtr(const SharedPtr& other) : _p(other._p), ucount(other.ucount) {
        if (ucount) {
            ++(*ucount);
        }
    }
    // Copy assignment operator
    SharedPtr& operator=(const SharedPtr& other) {
        if (this == &other) {
            return *this;
        }
        if (ucount && (--(*ucount) == 0)) {
            delete _p;
            delete ucount;
        }
        // 共享新的对象
        _p = other._p;
        ucount = other.ucount;
        if (ucount) {
            ++(*ucount);
        }
        return *this;
    }

    // Destructor
    ~SharedPtr() {
        if (ucount && (--(*ucount) == 0)) {
            delete _p;
            delete ucount;
            _p = nullptr;
            ucount = nullptr;
        }
    }

    // Get the number of shared owners
    int use_count() const { return ucount ? *ucount : 0; }
    
    // Basic operations
    T* get() const { return _p; }
    T& operator*() const { return *_p; }
    T* operator->() const { return _p; }
    operator bool() const { return _p != nullptr; }

    // Reset to empty state
    void reset() {
        if (ucount && (--(*ucount) == 0)) {
            delete _p;
            delete ucount;
        }
        _p = nullptr;
        ucount = nullptr;
    }
    void reset(T* ptr) {
        if (ucount && (--(*ucount) == 0)) {
            delete _p;
            delete ucount;
        }
        _p = ptr;
        ucount = new int(1);
    }
};
// make_shared 辅助函数
template <typename T, typename... Args>
SharedPtr<T> make_shared(Args&&... args) {
    return SharedPtr<T>(new T(std::forward<Args>(args)...));
}
#endif