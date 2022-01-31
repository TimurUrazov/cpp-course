#pragma once

#include <cstddef>

template<typename T>
class vector {
public:
    using iterator = T*;
    using const_iterator = T const*;

    vector() :
        size_(0),
        capacity_(0),
        data_(nullptr) {}

    vector(vector const& other) :
        size_(other.size_),
        capacity_(other.size_),
        data_(nullptr) {
            if (size_ != 0) {
                T* new_data = data_copy(other.data_, size_);
                std::swap(data_, new_data);
            }
        }

    vector& operator=(vector const& other) {
        if (this != &other) {
            vector t = vector(other);
            t.swap(*this);
        }
        return *this;
    }

    ~vector() {
        clear();
        operator delete(data_);
    }

    T& operator[](size_t i) {
        return data_[i];
    }

    T const& operator[](size_t i) const {
        return data_[i];
    }

    T* data() {
        return data_;
    }

    T const* data() const {
        return data_;
    }

    size_t size() const {
        return size_;
    }

    T& front() {
        return *data_;
    }

    T const& front() const {
        return *data_;
    }

    T& back() {
        return data_[size_ - 1];
    }

    T const& back() const {
        return data_[size_ - 1];
    }

    void push_back(T const& t) {
        if (capacity_ == size_) {
            T t_tmp = t;
            extend_buffer(capacity_ == 0 ? 1 : capacity_ * 2);
            new (data_ + size_) T(t_tmp);
        } else {
            new (data_ + size_) T(t);
        }
        size_++;
    }

    void pop_back() {
        --size_;
        data_[size_].~T();
    }

    bool empty() const {
        return size_ == 0;
    }

    size_t capacity() const {
        return capacity_;
    }

    void reserve(size_t new_capacity) {
        if (new_capacity > capacity_) {
            extend_buffer(new_capacity);
        }
    }

    void shrink_to_fit() {
        if (size_ != capacity_) {
            vector t = vector(*this);
            t.swap(*this);
        }
    }

    void clear() {
        while (!empty()) {
            pop_back();
        }
    }

    void swap(vector& other) {
        std::swap(other.size_, size_);
        std::swap(other.capacity_, capacity_);
        std::swap(other.data_, data_);
    }

    iterator begin() {
        return data_;
    }

    iterator end() {
        return data_ + size_;
    }

    const_iterator begin() const {
        return data_;
    }

    const_iterator end() const {
        return data_ + size_;
    }

    iterator insert(const_iterator pos, T const& value) {
        std::size_t delta = pos - data_;
        push_back(value);
        for (std::size_t i = size_ - 1; i != delta; --i) {
            std::swap(data_[i - 1], data_[i]);
        }
        return data_ + delta;
    }

    iterator erase(const_iterator pos) {
        return erase(pos, pos + 1);
    }

    iterator erase(const_iterator first, const_iterator last) {
        std::size_t delta = first - data_;
        std::size_t bound = last - first;
        for (std::size_t i = delta; i != size_ - bound; ++i) {
            std::swap(data_[i + bound], data_[i]);
        }
        while (bound--) {
            pop_back();
        }
        return data_ + delta;
    }

private:
    T* data_copy(T* data, size_t new_capacity) {
        T* new_data = static_cast<T*>(operator new(new_capacity * sizeof(T)));
        for (size_t i = 0; i != size_; ++i) {
            try {
                new(new_data + i) T(data[i]);
            } catch (...) {
                for (std::size_t j = i; j > 0; j--) {
                    new_data[j - 1].~T();
                }
                operator delete(new_data);
                throw;
            }
        }
        return new_data;
    }

    void extend_buffer(size_t new_capacity) {
        std::size_t size_copy = size_;
        T* new_data = data_copy(data_, new_capacity);
        clear();
        std::swap(data_, new_data);
        operator delete(new_data);
        capacity_ = new_capacity;
        size_ = size_copy;
    }

    T* data_;
    size_t size_;
    size_t capacity_;
};
