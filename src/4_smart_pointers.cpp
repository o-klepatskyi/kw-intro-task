#include "4_smart_pointers.h"

#include <iostream>
#include <vector>
#include <memory>
#include <initializer_list>

struct MyCollection::Impl
{
    std::vector<int> vec;

    Impl() {};
    Impl(std::initializer_list<int> list) noexcept : vec {list} {}

    void add(int i)
    {
        vec.push_back(i);
    }

    void remove(int i)
    {
        std::erase(vec, i);
    }

    int size() const noexcept
    {
        return vec.size();
    }
};

MyCollection::MyCollection()
: impl_ {std::make_unique<Impl>()}
{}

MyCollection::MyCollection(std::initializer_list<int> l)
: impl_(std::make_unique<Impl>(l))
{}

MyCollection::MyCollection(const MyCollection& other)
: impl_(std::make_unique<MyCollection::Impl>(*other.impl_))
{}

MyCollection& MyCollection::operator=(const MyCollection& other)
{
    impl_ = std::make_unique<MyCollection::Impl>(*other.impl_);
    return *this;
}

MyCollection::~MyCollection() {}

void MyCollection::add(int i)
{
    impl_->add(i);
}

void MyCollection::remove(int i)
{
    impl_->remove(i);
}

int MyCollection::size() const noexcept
{
    return impl_->size();
}