// М8О-206Б-19 Киреев А.К.
#pragma once
#include <memory>
#include <iostream>
#include <exception>
#include <iterator>
#include <queue>

template <class T, size_t CAP = 10>
struct custom_allocator {
    using value_type = T;
    using pointer = T *;
    using const_pointer = const T *;
    using size_type = std::size_t;

    static const size_t cap = CAP;
    std::size_t size = 0;
    pointer buf;
    std::size_t free_size = 0;
    std::queue<pointer> free_mem;

    custom_allocator() noexcept {}

    template <class U, size_t CU>
    struct rebind {
        using other = custom_allocator<U, CU>;
    };

    T *allocate(std::size_t n) {
        if (size == 0) {
            buf = reinterpret_cast<T*>(operator new(CAP * sizeof(T)));
            for (std::size_t i = 0; i < CAP; i++) {
                free_mem.push(&(buf[i]));
                free_size++;
            }
            size = CAP;
        }
        T* to_return = free_mem.front();
        for (std::size_t i = 0; i < n; ++i) {
            free_mem.pop();
            free_size--;
        }
        std::cout << "Kek: " << free_size << "\n";
        return to_return;
    }

    void deallocate(pointer p, std::size_t) {
        free_mem.push(p);
        free_size++;
        if (free_size >= CAP) {
            ::operator delete(buf);
            free_mem.~queue();
            size--;
        }
    }

    template <typename U, typename... Args>
    void construct(U *p, Args &&... args) {
        new (p) U(std::forward<Args>(args)...);
    }

    void destroy(pointer p){
        p->~T();
    }
};

template <class T, class Allocator>
class List {
    private:
        struct ListNode {
            using allocator_type = typename Allocator::template rebind<ListNode, Allocator::cap>::other;

            std::unique_ptr<ListNode> _next;
            T _val;
            T& get(std::size_t idx);
            void insert(ListNode &prev, std::size_t idx,const T& val);
            void erase(ListNode &prev, std::size_t idx);

            void *operator new(std::size_t size) {
                return get_allocator().allocate(1);
            }
            void operator delete(void *p) {
                get_allocator().destroy((ListNode*)p);
                get_allocator().deallocate((ListNode *)p, 1);
            }
            static allocator_type get_allocator() {
                static allocator_type allocator;
                return allocator;
            }
        };
        std::unique_ptr<ListNode> _head;
        std::size_t _size;
    public:
        using value_type = T;

        class ListIterator {
            private: 
                List& _list;
                size_t _idx;
                friend class List;
            public:
                using difference_type = int;
                using value_type = List::value_type;
                using reference = List::value_type&;
                using pointer = List::value_type*;
                using iterator_category = std::forward_iterator_tag;

                ListIterator(List& l, std::size_t idx) : _list(l), _idx(idx) {}
                ListIterator& operator++();
                reference  operator*();
                pointer operator->();
                bool operator!=(const ListIterator& other);
                bool operator==(const ListIterator& other);
        }; 
        List<T, Allocator>::ListIterator begin();
        List<T, Allocator>::ListIterator end();
        ListIterator insert(ListIterator& it, T& val);
        ListIterator erase(ListIterator& it);
        T& operator[](size_t idx);
        size_t size();
};

template <class T, class Allocator>
size_t List<T, Allocator>::size() {
    return _size;
}

template <class T, class Allocator>
typename List<T, Allocator>::ListIterator& List<T, Allocator>::ListIterator::operator++() {
    ++_idx;
    return *this;
}

template <class T, class Allocator>
typename List<T, Allocator>::ListIterator::reference List<T, Allocator>::ListIterator::operator*() {
    return _list[_idx];
}

template <class T, class Allocator>
typename List<T, Allocator>::ListIterator::pointer List<T, Allocator>::ListIterator::operator->(){
    return &_list[_idx];
}

template <class T, class Allocator>
bool List<T, Allocator>::ListIterator::operator!=(const ListIterator& other){
    if(_idx != other._idx) {
        return true;
    }
    if(&_list != &(other._list)) {
        return true;
    }
    return false;
}

template <class T, class Allocator>
bool List<T, Allocator>::ListIterator::operator==(const ListIterator& other){
    if(_idx != other._idx) {
        return false;
    }
    if(&_list != &(other._list)) {
        return false;
    }
    return true;
}

template <class T, class Allocator>
T& List<T, Allocator>::ListNode::get(size_t idx){
    if(idx == 0) {
        return _val;
    }
    if(_next) {
        return _next->get(--idx);
    }
    throw std::logic_error("Out of bounds"); 
}

template <class T, class Allocator>
void List<T, Allocator>::ListNode::insert(ListNode &prev, size_t idx, const T& val) {
    if(idx == 0) {
        prev._next = std::make_unique<ListNode>(ListNode{std::move(prev._next), val});
        return;
    } else {
        return _next->insert(*this, --idx, val);
    }
    throw std::logic_error("Out of bounds"); 
}

template <class T, class Allocator>
void List<T, Allocator>::ListNode::erase(ListNode &prev, size_t idx) {
    if(idx == 0) {
        prev._next = std::move(_next);
        return;
    } else {
        if(_next) {
            return _next->erase(*this, --idx);
        }
        throw std::logic_error("Out of bounds"); 
    }
}

template<class T, class Allocator>
typename List<T, Allocator>::ListIterator List<T, Allocator>::begin() {
    return ListIterator(*this, 0);
}

template<class T, class Allocator>
typename List<T, Allocator>::ListIterator List<T, Allocator>::end() {
    return ListIterator(*this, _size);
}

template<class T, class Allocator>
typename List<T, Allocator>::ListIterator List<T, Allocator>::insert(typename List<T, Allocator>::ListIterator& it, T& val) {
    if (it._idx > size()) {
        throw std::logic_error("Out of bounds");
    }
    if (it._idx == 0) {
        _head = std::make_unique<ListNode>(ListNode{std::move(_head), val});
        _size++;
    } else {
        _head->_next->insert(*_head,it._idx-1, val);
        _size++;
    }
    if (it._idx <= size()) {
        return it;
    }
    return ListIterator(*this, size());
}

template<class T, class Allocator>
T& List<T, Allocator>::operator[](size_t idx){
    if(!_head) {
        throw std::logic_error("Out of bounds");
    }
    return _head->get(idx);
}

template <class T, class Allocator>
typename List<T, Allocator>::ListIterator List<T, Allocator>::erase(ListIterator& iter) {
    if (iter._idx >= size()) {
        throw std::logic_error("Out of bounds");
    }
    if (iter._idx == 0){
        _head = std::move(_head->_next);
        _size--;
    } else {
        if (_head->_next) {
            _head->_next->erase(*_head,iter._idx-1);
            _size--;
        }
    }
    if(iter._idx < size()) {
        return iter;
    }
    return ListIterator(*this, size());
}
          