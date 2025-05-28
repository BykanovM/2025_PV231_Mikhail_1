#pragma once

template <typename T>
class smart_ptr {
    T* obj;
public:
    explicit smart_ptr(T* ptr = nullptr) : obj(ptr) {}
    ~smart_ptr() { delete obj; }

    smart_ptr(const smart_ptr&) = delete;
    smart_ptr& operator=(const smart_ptr&) = delete;

    smart_ptr(smart_ptr&& other) noexcept : obj(other.obj) { other.obj = nullptr; }
    smart_ptr& operator=(smart_ptr&& other) noexcept {
        if (this != &other) {
            delete obj;
            obj = other.obj;
            other.obj = nullptr;
        }
        return *this;
    }

    T* operator->() const { return obj; }
    T& operator*() const { return *obj; }

    T* get() const { return obj; }
    void reset(T* ptr = nullptr) {
        delete obj;
        obj = ptr;
    }
};