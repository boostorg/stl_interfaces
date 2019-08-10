//[ reverse_iterator
// Copyright (C) 2019 T. Zachary Laine
//
// Distributed under the Boost Software License, Version 1.0. (See
// accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
#include <boost/iterator_facade/iterator_facade.hpp>

#include <algorithm>
#include <list>
#include <vector>

#include <cassert>


template<typename BidiIter>
struct reverse_iterator : boost::iterator_facade::iterator_facade<
                              reverse_iterator<BidiIter>,
                              typename BidiIter::iterator_category,
                              typename BidiIter::value_type>
{
    reverse_iterator() : it_() {}
    reverse_iterator(BidiIter it) : it_(it) {}

private:
    friend boost::iterator_facade::access;
    typename BidiIter::value_type dereference() { return *std::prev(it_); }
    bool equals(reverse_iterator other) const { return it_ == other.it_; }
    typename BidiIter::difference_type compare(reverse_iterator other) const
    {
        std::distance(other.it_, it_);
    }
    void next() { --it_; }
    void prev() { ++it_; }
    void advance(typename BidiIter::difference_type n)
    {
        std::advance(it_, -n);
    }

    BidiIter it_;
};


using reverse_bidi = reverse_iterator<std::list<int>::iterator>;
using reverse_ra = reverse_iterator<std::vector<int>::iterator>;


int main()
{
    // TODO
}
//]
