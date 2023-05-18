#pragma once
#include "util.h"
#include <memory>
#include <stdexcept>
#include <initializer_list>

template<typename T>
class SmartPtrLinkedList
{
    struct Node
    {
        T value;
        std::shared_ptr<Node> next;
    };

    std::shared_ptr<Node> head;
public:
    SmartPtrLinkedList() : head(nullptr) {}
    ~SmartPtrLinkedList() { clear(); }

    SmartPtrLinkedList(std::initializer_list<T> inlist)
    : head(nullptr)
    {
        copy(inlist);
    }

    SmartPtrLinkedList(const SmartPtrLinkedList& other)
    : head(nullptr)
    {
        copy(other);
    }

    SmartPtrLinkedList& operator=(const SmartPtrLinkedList& other)
    {
        clear();
        copy(other);
        return *this;
    }

    SmartPtrLinkedList(SmartPtrLinkedList&& other) noexcept
    : head(std::move(other.head))
    {}

    SmartPtrLinkedList operator=(SmartPtrLinkedList&& other) noexcept
    {
        head = std::move(other.head);
        return *this;
    }

private:
    template<template <class> class Container>
    void copy(const Container<T>& other)
    {
        if (other.size() == 0) return;
        auto it = other.begin();
        head = std::make_shared<Node>(*it, nullptr);
        auto cur = head;

        while(++it != other.end())
        {
            cur->next = std::make_shared<Node>(*it, nullptr);
            cur = cur->next;
        }
    }
public:
    inline T& front()        { *begin(); }
    inline T  front() const  { *cbegin(); }

    inline bool empty() const noexcept { return !head; }

    size_t size() const noexcept
    {
        size_t result = 0;
        auto it = begin();
        while (it != end()) {
            ++it;
            ++result;
        }
        return result;
    }

    inline void clear() noexcept
    {
        while(head != nullptr)
        {
            head = head->next;
        }
    }

    struct bad_iterator : public std::exception {};

    template<bool constiter> 
    class IteratorT
    {
        friend class SmartPtrLinkedList<T>;
        std::weak_ptr<Node> ptr;

        inline auto node() const
        {
            auto shptr = ptr.lock();
            if (!shptr) throw bad_iterator{}; 
            return shptr;
        }
    public:
        IteratorT() noexcept : ptr {nullptr} {}
        IteratorT(const std::shared_ptr<Node>& node) noexcept : ptr {node} {}
        IteratorT(const IteratorT& other) noexcept : ptr {other.ptr} {}
        IteratorT(IteratorT&& other) noexcept : ptr {std::move(other.ptr)} {}

        using DereferenceType = Select<const T&, T&, constiter>;

        T& operator*() const
        {
           return node()->value;
        }

        T& operator->() const
        {
            return *node()->value;
        }

        IteratorT& operator++()
        {
            auto n = node();
            ptr = n->next;
            return *this;
        }

        inline operator bool() const noexcept { return !ptr.expired(); }

        template<bool b>
        inline bool operator==(const IteratorT<b>& other) const noexcept
        {
            return !ptr.owner_before(other.ptr) && !other.ptr.owner_before(ptr);
        }

        template<bool b>
        bool operator !=(const IteratorT<b>& other) const noexcept
        {
            return !(*this == other);
        }
    };

    using Iterator = IteratorT<false>;
    using ConstIterator = IteratorT<true>;

    inline Iterator         begin()         noexcept { return head; }
    inline Iterator         end()           noexcept { return {nullptr};}
    inline ConstIterator    begin() const   noexcept { return head; }
    inline ConstIterator    end()   const   noexcept { return {nullptr};}
    inline ConstIterator    cbegin()const   noexcept { return head; }
    inline ConstIterator    cend()  const   noexcept { return {nullptr};}

    template<bool b>
    inline IteratorT<b> insertAfter(IteratorT<b> it, const T& value)
    {
        auto ptr = it.node();
        auto next = std::make_shared<Node>(value, ptr->next);
        ptr->next = next;
        return ++it;
    }

    template<bool b>
    inline IteratorT<b> insertAfter(IteratorT<b> it, T&& value)
    {
        auto ptr = it.node();
        auto next = std::make_shared<Node>(std::move(value), ptr->next);
        ptr->next = next;
        return ++it;
    }

    template<bool b>
    inline IteratorT<b> eraseAfter(IteratorT<b> it)
    {
        auto ptr = it.node();
        ptr->next = ptr->next->next;
        return ++it;
    }

    inline void pushFront(const T& value) noexcept
    {
        auto next = std::make_shared<Node>(value, head);
        head = next;
    }

    inline void pushFront(T&& value) noexcept
    {
        auto next = std::make_shared<Node>(std::move(value), head);
        head = next;
    }

    inline void popFront()
    {
        head = head->next;
    }

};

template<typename T>
bool operator==(const SmartPtrLinkedList<T>& lhs, const SmartPtrLinkedList<T>& rhs) noexcept
{
    auto it1 = lhs.cbegin();
    auto it2 = rhs.cbegin();
    if (it1 == it2) return true;
    while (it1 && it2) {
        if (*it1 != *it2) return false;
        ++it1; ++it2;
    }
    return it1 == it2; // both are nullptr
};

template<typename T>
inline bool operator!=(const SmartPtrLinkedList<T>& lhs, const SmartPtrLinkedList<T>& rhs) noexcept
{
    return !(lhs == rhs);
};