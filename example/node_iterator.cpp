// Copyright (C) 2019 T. Zachary Laine
//
// Distributed under the Boost Software License, Version 1.0. (See
// accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
#include <boost/iterator_facade/iterator_facade.hpp>

#include <algorithm>
#include <array>
#include <iostream>

#include <cassert>


//[ node_defn
template<typename T>
struct node
{
    T value_;
    node * next_; // == nullptr in the tail node
};
//]

//[ node_iterator
//[ node_iterator_class_head
template<typename T>
struct node_iterator
    : boost::iterator_facade::
          iterator_facade<node_iterator<T>, std::forward_iterator_tag, T>
//]
{
    //[ node_iterator_ctors
    constexpr node_iterator() noexcept : it_(nullptr) {}
    constexpr node_iterator(node<T> * it) noexcept : it_(it) {}
    //]

    //[ node_iterator_private
private:
    friend boost::iterator_facade::access;
    constexpr T & dereference() const noexcept { return it_->value_; }
    constexpr void next() noexcept { it_ = it_->next_; }
    constexpr bool equals(node_iterator other) const noexcept
    {
        return it_ == other.it_;
    }

    node<T> * it_;
    //]
};
//]

//[ node_iterator_concept_check
#if 201703L < __cplusplus
static_assert(std::forward_iterator<node_iterator>);
#endif
//]


int main()
{
    std::array<node<int>, 5> nodes;
    std::generate(nodes.begin(), nodes.end(), [] {
        static int i = 0;
        return node<int>{i++};
    });
    std::adjacent_find(
        nodes.begin(), nodes.end(), [](node<int> & a, node<int> & b) {
            a.next_ = &b;
            return false;
        });
    nodes.back().next_ = nullptr;

    //[ node_iterator_usage
    node_iterator<int> const first(&nodes[0]);
    node_iterator<int> const last;
    for (auto it = first; it != last; ++it) {
        std::cout << *it << " "; // Prints 0 1 2 3 4
    }
    std::cout << "\n";
    //]
}
