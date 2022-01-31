#pragma once
#include <cassert>
#include <iterator>

template <typename T>
struct list {
private:
    struct node; 
    struct vnode;
    
    template <typename V>
    struct base_iterator;

public:
    using iterator = base_iterator<T>;
    using const_iterator = base_iterator<T const>;
    using reverse_iterator = std::reverse_iterator<base_iterator<T>>;
    using const_reverse_iterator = std::reverse_iterator<base_iterator<T const>>;

public:
    // O(1)
    list() noexcept;

    // O(n), strong
    list(list const&);

    // O(n), strong
    list& operator=(list const&);

    // O(n)
    ~list();

    // O(1)
    bool empty() const noexcept;

    // O(1)
    T& front() noexcept;
    // O(1)
    T const& front() const noexcept;

    // O(1), strong
    void push_front(T const&);
    // O(1)
    void pop_front() noexcept;

    // O(1)
    T& back() noexcept;
    // O(1)
    T const& back() const noexcept;

    // O(1), strong
    void push_back(T const&);
    // O(1)
    void pop_back() noexcept;

    // O(1)
    iterator begin() noexcept;
    // O(1)
    const_iterator begin() const noexcept;

    // O(1)
    iterator end() noexcept;
    // O(1)
    const_iterator end() const noexcept;

    // O(1)
    reverse_iterator rbegin() noexcept;
    // O(1)
    const_reverse_iterator rbegin() const noexcept;

    // O(1)
    reverse_iterator rend() noexcept;
    // O(1)
    const_reverse_iterator rend() const noexcept;

    // O(n)
    void clear() noexcept;

    // O(1), strong
    iterator insert(const_iterator pos, T const& val);
    // O(1)
    iterator erase(const_iterator pos) noexcept;
    // O(n)
    iterator erase(const_iterator first, const_iterator last) noexcept;
    // O(1)
    void splice(const_iterator pos, list& that,
                const_iterator first, const_iterator last) noexcept;

    template <typename V>
    friend void swap(list<V>& a, list<V>& b) noexcept;

private:
    mutable node fake;
};

template<typename T>
template<typename V>
struct list<T>::base_iterator : std::iterator<std::bidirectional_iterator_tag, V> {

public:
    using reference = V&;
    using pointer = V*;

    base_iterator() {};

    base_iterator(base_iterator<T> const& that) :
        p(that.p) {}

    base_iterator& operator++() {
        p = p->next;
        return *this;
    }

    reference operator*() const {
        return static_cast<vnode*>(p)->value;
    }

    pointer operator->() const {
        return &static_cast<vnode*>(p)->value;
    }

    base_iterator operator++(int) {
        base_iterator<V> copy(*this);
        ++*this;
        return copy;
    }
    
    base_iterator& operator--() {
        p = p->prev;
        return *this;
    }
    base_iterator operator--(int) {
        base_iterator<V> copy(*this);
        --*this;
        return copy;
    }

    friend bool operator==(const base_iterator& a, const base_iterator& b) {
        return a.p == b.p;
    }

    friend bool operator!=(const base_iterator& a, const base_iterator& b) {
        return a.p != b.p;
    }

private:
    explicit base_iterator(node* p) :
        p(p) {}

    node* p;

    friend struct list;
};

template<typename T>
struct list<T>::node {

    friend struct list<T>;

    node() :
        next(this),
        prev(this) {}

    node(node* prev, node* next) :
        next(next),
        prev(prev) {}

    virtual ~node() = default;

    node* next;
    node* prev;
};

template<typename T>
struct list<T>::vnode : node {

    vnode(T const &value, node* prev, node* next) :
        node(prev, next),
        value(value) {}

    T value;

    friend struct node;
};

template<typename T>
list<T>::list() noexcept
{}

template<typename T>
list<T>::list(const list<T> &that)
    : list() {
        for (auto x : that) {
            try {
                push_back(x);
            } catch (...) {
                clear();
                throw;
            }
        }
    }

template<typename T>
list<T> &list<T>::operator=(const list<T> &that) {
    if (this != &that) {
        list<T> copy(that);
        swap(copy, *this);
    }
    return *this;
}

template<typename T>
list<T>::~list() {
    clear();
}

template<typename T>
bool list<T>::empty() const noexcept {
    return &fake == fake.next && &fake == fake.prev;
}

template<typename T>
T &list<T>::front() noexcept {
    return *begin();
}

template<typename T>
T const &list<T>::front() const noexcept {
    return *begin();
}

template<typename T>
void list<T>::push_front(const T &value) {
    insert(begin(), value);
}

template<typename T>
void list<T>::pop_front() noexcept {
    erase(begin());
}

template<typename T>
T &list<T>::back() noexcept {
    return *(--end());;
}

template<typename T>
T const &list<T>::back() const noexcept {
    return *(--end());
}

template<typename T>
void list<T>::push_back(const T &value) {
    insert(end(), value);
}

template<typename T>
void list<T>::pop_back() noexcept {
    erase(--end());
}

template<typename T>
typename list<T>::iterator list<T>::begin() noexcept {
    return list::iterator(fake.next);
}

template<typename T>
typename list<T>::const_iterator list<T>::begin() const noexcept {
    return list::iterator(fake.next);
}

template<typename T>
typename list<T>::iterator list<T>::end() noexcept {
    return iterator(&fake);
}

template<typename T>
typename list<T>::const_iterator list<T>::end() const noexcept {
    return const_iterator(const_cast<node*>(&fake));
}

template<typename T>
typename list<T>::reverse_iterator list<T>::rbegin() noexcept {
    return reverse_iterator(end());
}

template<typename T>
typename list<T>::const_reverse_iterator list<T>::rbegin() const noexcept {
    return const_reverse_iterator(end());
}

template<typename T>
typename list<T>::reverse_iterator list<T>::rend() noexcept {
    return reverse_iterator(begin());
}

template<typename T>
typename list<T>::const_reverse_iterator list<T>::rend() const noexcept {
    return const_reverse_iterator(begin());
}

template<typename T>
void list<T>::clear() noexcept {
    while (!empty()) {
        pop_back();
    }

    fake.next = &fake;
    fake.prev = &fake;
}

template<typename T>
typename list<T>::iterator list<T>::insert(list::const_iterator pos, const T &value) {
    const_iterator prev = pos;
    prev--;
    node* cur = nullptr;
    try {
        cur = new vnode(value, prev.p, pos.p);
    } catch (...) {
        throw;
    }
    pos.p->prev = cur;
    prev.p->next = cur;
    return iterator(cur);
}

template<typename T>
typename list<T>::iterator list<T>::erase(list::const_iterator pos) noexcept {
    const_iterator next = pos;
    const_iterator prev = pos;
    next++;
    prev--;
    next.p->prev = prev.p;
    delete static_cast<vnode*>(prev.p->next);
    prev.p->next = next.p;
    return iterator(next.p);
}

template<typename T>
typename list<T>::iterator list<T>::erase(list::const_iterator first, list::const_iterator last) noexcept {
    while (first != last) {
        first = erase(first);
    }
    return iterator(first.p);
}

template<typename T>
void list<T>::splice(list::const_iterator pos, list<T> &that,
                     list::const_iterator begin, list::const_iterator end)  noexcept {
    if (begin != end) {
        const_iterator it_begin = begin;
        const_iterator it_end = end;
        const_iterator it_pos = pos;
        it_begin--;
        it_end--;
        it_pos--;
        it_end.p->next = pos.p;
        pos.p->prev = it_end.p;

        it_pos.p->next = begin.p;
        begin.p->prev = it_pos.p;

        it_begin.p->next = end.p;
        end.p->prev = it_begin.p;
    }
}

template <typename V>
void swap(list<V>& a, list<V>& b) noexcept {
    const bool is_empty_fst = a.empty();
    const bool is_empty_scd = b.empty();

    std::swap(a.fake.prev, b.fake.prev);
    std::swap(a.fake.next, b.fake.next);

    if (is_empty_fst && is_empty_scd) {
        std::swap(a.fake, b.fake);
    } else {
        if (is_empty_fst) {
            b.fake.prev = &b.fake;
            b.fake.next = &b.fake;
        } else {
            b.fake.prev->next = &b.fake;
            b.fake.next->prev = &b.fake;
        }

        if (is_empty_scd) {
            a.fake.next = &a.fake;
            a.fake.prev = &a.fake;
        } else {
            a.fake.next->prev = &a.fake;
            a.fake.prev->next = &a.fake;
        }
    }
}
