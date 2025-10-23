// MyQueue.h
#pragma once
#include <cstddef>
#include <utility>
#include <stdexcept>
using std::swap;

template <typename T>
class MyQueue {
public:
    MyQueue() : data_(nullptr), cap_(0), head_(0), tail_(0), size_(0) {}

    ~MyQueue() { delete[] data_; }

        MyQueue(const MyQueue& other)
        : data_(nullptr), cap_(0), head_(0), tail_(0), size_(0) {
        if (other.size_ == 0) return;
        cap_ = other.size_;
        data_ = new T[cap_];
        for (size_t i = 0; i < other.size_; ++i) {
            data_[i] = other.data_[(other.head_ + i) % other.cap_];
        }
        head_ = 0;
        size_ = other.size_;
        tail_ = size_ % cap_;
    }

    MyQueue(MyQueue&& other) noexcept
        : data_(other.data_), cap_(other.cap_),
          head_(other.head_), tail_(other.tail_), size_(other.size_) {
        other.data_ = nullptr;
        other.cap_ = other.head_ = other.tail_ = other.size_ = 0;
    }

    MyQueue& operator=(MyQueue other) noexcept {
        mySwapQueue(other);
        return *this;
    }

    void mySwapQueue(MyQueue& other) noexcept {
        swap(data_, other.data_);
        swap(cap_, other.cap_);
        swap(head_, other.head_);
        swap(tail_, other.tail_);
        swap(size_, other.size_);
    }

    void push(const T& value) {
        ensureCapacity(size_ + 1);
        data_[tail_] = value;
        tail_ = (tail_ + 1) % cap_;
        ++size_;
    }

    void push(T&& value) {
        ensureCapacity(size_ + 1);
        data_[tail_] = std::move(value);
        tail_ = (tail_ + 1) % cap_;
        ++size_;
    }

    void pop() {
        if (size_ == 0) return;
        head_ = (head_ + 1) % cap_;
        --size_;
    }

    T& front() {
        if (size_ == 0) throw std::out_of_range("Queue is empty");
        return data_[head_];
    }

    const T& front() const {
        if (size_ == 0) throw std::out_of_range("Queue is empty");
        return data_[head_];
    }

    bool empty() const noexcept { return size_ == 0; }

    size_t size() const noexcept { return size_; }

private:
    void ensureCapacity(size_t desired) {
        if (cap_ == 0) {
            cap_ = 8;
            data_ = new T[cap_];
            head_ = tail_ = size_ = 0;
            return;
        }
        if (desired <= cap_) return;
        size_t newCap = cap_ * 2;
        while (newCap < desired) newCap *= 2;
        T* nd = new T[newCap];
        for (size_t i = 0; i < size_; ++i) {
            nd[i] = std::move(data_[(head_ + i) % cap_]);
        }
        delete[] data_;
        data_ = nd;
        cap_ = newCap;
        head_ = 0;
        tail_ = size_ % cap_;
    }

    T* data_;
    size_t cap_;
    size_t head_;
    size_t tail_;
    size_t size_;
};
