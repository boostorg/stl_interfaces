// Copyright (C) 2019 T. Zachary Laine
//
// Distributed under the Boost Software License, Version 1.0. (See
// accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//[ reverse_iterator
#include <boost/iterator_facade/iterator_facade.hpp>

#include <algorithm>
#include <list>
#include <vector>

#include <cassert>


// In all the previous examples, we only had to implement a subset of the six
// possible user-defined basis operations.  For reverse_iterator, we want to
// support bidirectional, random access, and contiguous iterators.  We
// therefore need to provide all the basis operations that might be needed.
// Unfortunately, that's all six of them.
template<typename BidiIter>
struct reverse_iterator
    : boost::iterator_facade::iterator_facade<
          reverse_iterator<BidiIter>,
#if 201703L < __cplusplus
          typename std::iterator_traits<BidiIter>::iterator_concept,
#else
          typename std::iterator_traits<BidiIter>::iterator_category,
#endif
          typename std::iterator_traits<BidiIter>::value_type>
{
    reverse_iterator() : it_() {}
    reverse_iterator(BidiIter it) : it_(it) {}

private:
    friend boost::iterator_facade::access;

    using ref_t = typename std::iterator_traits<BidiIter>::reference;
    using diff_t = typename std::iterator_traits<BidiIter>::difference_type;

    ref_t dereference() const { return *std::prev(it_); }

    // These three are used only when BidiIter::iterator_category is
    // std::bidirectional_iterator_tag.
    bool equals(reverse_iterator other) const { return it_ == other.it_; }
    void next() { --it_; }
    void prev() { ++it_; }

    // These two are only used when BidiIter::iterator_category is
    // std::random_access_iterator_tag or std::contiguous_iterator_tag.  Even
    // so, they need to compile even when BidiIter::iterator_category is
    // std::bidirectional_iterator_tag.  That means we have to use
    // std::distance() and std::advance() instead of operator-() and
    // operator+=().
    //
    // Don't worry, the O(n) bidirectional implementations of std::distance()
    // and std::advance() are dead code, because compare() and advance() are
    // never even called when BidiIter::iterator_category is
    // std::bidirectional_iterator_tag.
    diff_t compare(reverse_iterator other) const
    {
        return std::distance(it_, other.it_);
    }
    void advance(diff_t n) { std::advance(it_, -n); }

    BidiIter it_;
};

using rev_bidi_iter = reverse_iterator<std::list<int>::iterator>;
using rev_ra_iter = reverse_iterator<std::vector<int>::iterator>;


int main()
{
    {
        std::list<int> ints = {4, 3, 2};
        std::list<int> ints_copy;
        std::copy(
            rev_bidi_iter(ints.end()),
            rev_bidi_iter(ints.begin()),
            std::back_inserter(ints_copy));
        std::reverse(ints.begin(), ints.end());
        assert(ints_copy == ints);
    }

    {
        std::vector<int> ints = {4, 3, 2};
        std::vector<int> ints_copy(ints.size());
        std::copy(
            rev_ra_iter(ints.end()),
            rev_ra_iter(ints.begin()),
            ints_copy.begin());
        std::reverse(ints.begin(), ints.end());
        assert(ints_copy == ints);
    }

    {
        using rev_ptr_iter = reverse_iterator<int *>;

        int ints[3] = {4, 3, 2};
        int ints_copy[3];
        std::copy(
            rev_ptr_iter(std::end(ints)),
            rev_ptr_iter(std::begin(ints)),
            std::begin(ints_copy));
        std::reverse(std::begin(ints), std::end(ints));
        assert(std::equal(
            std::begin(ints_copy),
            std::end(ints_copy),
            std::begin(ints),
            std::end(ints)));
    }
}
//]
