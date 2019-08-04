// Copyright (C) 2019 T. Zachary Laine
//
// Distributed under the Boost Software License, Version 1.0. (See
// accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
#include <boost/iterator_facade/iterator_facade.hpp>

#include <gtest/gtest.h>

#include <array>
#include <numeric>
#include <type_traits>


struct basic_input_iter
    : boost::iterator_facade::
          iterator_facade<basic_input_iter, std::input_iterator_tag, int>
{
    basic_input_iter(int * it) : it_(it) {}

private:
    friend boost::iterator_facade::access;
    int & dereference() const { return *it_; }
    void next() { ++it_; }
    bool equals(basic_input_iter other) const { return it_ == other.it_; }

    int * it_;
};

template<typename ValueType>
struct input_iter : boost::iterator_facade::iterator_facade<
                        input_iter<ValueType>,
                        std::input_iterator_tag,
                        ValueType>
{
    input_iter(ValueType * it) : it_(it) {}
    template<
        typename ValueType2,
        typename E = std::enable_if_t<std::is_convertible<
            typename ValueType2::value_type *,
            ValueType *>::value>>
    input_iter(ValueType2 it) : it_(it.it_)
    {}

private:
    friend boost::iterator_facade::access;
    ValueType & dereference() const { return *it_; }
    void next() { ++it_; }
    bool equals(input_iter other) const { return it_ == other.it_; }

    ValueType * it_;

    template<typename ValueType2>
    friend struct input_iter;
};

using int_input = input_iter<int>;
using const_int_input = input_iter<int const>;
using pair_input = input_iter<std::pair<int, int>>;
using const_pair_input = input_iter<std::pair<int, int> const>;

template<typename ValueType>
struct proxy_input_iter : boost::iterator_facade::proxy_iterator_facade<
                              proxy_input_iter<ValueType>,
                              std::input_iterator_tag,
                              ValueType>
{
    proxy_input_iter(ValueType * it) : it_(it) {}
    template<
        typename ValueType2,
        typename E = std::enable_if_t<std::is_convertible<
            typename ValueType2::value_type *,
            ValueType *>::value>>
    proxy_input_iter(ValueType2 it) : it_(it.it_)
    {}

private:
    friend boost::iterator_facade::access;
    ValueType dereference() const { return *it_; }
    void next() { ++it_; }
    bool equals(proxy_input_iter other) const { return it_ == other.it_; }

    ValueType * it_;

    template<typename ValueType2>
    friend struct proxy_input_iter;
};

std::array<int, 10> ints = {{0, 1, 2, 3, 4, 5, 6, 7, 8, 9}};
std::array<std::pair<int, int>, 10> pairs = {{
    {0, 1},
    {1, 1},
    {2, 1},
    {3, 1},
    {4, 1},
    {5, 1},
    {6, 1},
    {7, 1},
    {8, 1},
    {9, 1},
}};


// TODO: Call std::ranges::copy below too.


TEST(input, basic_std_copy)
{
    basic_input_iter first(ints.data());
    basic_input_iter last(ints.data() + ints.size());

    {
        std::array<int, 10> ints_copy;
        std::copy(first, last, ints_copy.begin());
        EXPECT_EQ(ints_copy, ints);
    }
}

TEST(input, mutable_to_const_conversions)
{
    int_input first(ints.data());
    int_input last(ints.data() + ints.size());
    const_int_input first_copy(first);
    const_int_input last_copy(last);
    std::equal(first, last, first_copy, last_copy);
}

TEST(input, std_copy)
{
    {
        std::array<int, 10> ints_copy;
        int_input first(ints.data());
        int_input last(ints.data() + ints.size());
        std::copy(first, last, ints_copy.begin());
        EXPECT_EQ(ints_copy, ints);
    }

    {
        std::array<std::pair<int, int>, 10> pairs_copy;
        pair_input first(pairs.data());
        pair_input last(pairs.data() + pairs.size());
        std::copy(first, last, pairs_copy.begin());
        EXPECT_EQ(pairs_copy, pairs);
    }

    {
        std::array<int, 10> firsts_copy;
        pair_input first(pairs.data());
        pair_input last(pairs.data() + pairs.size());
        for (auto out = firsts_copy.begin(); first != last; ++first) {
            *out++ = first->first;
        }
        EXPECT_EQ(firsts_copy, ints);
    }

    {
        std::array<int, 10> firsts_copy;
        proxy_input_iter<std::pair<int, int>> first(pairs.data());
        proxy_input_iter<std::pair<int, int>> last(pairs.data() + pairs.size());
        for (auto out = firsts_copy.begin(); first != last; ++first) {
            *out++ = first->first;
        }
        EXPECT_EQ(firsts_copy, ints);
    }
}

TEST(input, const_std_copy)
{
    {
        std::array<int, 10> ints_copy;
        const_int_input first(ints.data());
        const_int_input last(ints.data() + ints.size());
        std::copy(first, last, ints_copy.begin());
        EXPECT_EQ(ints_copy, ints);
    }

    {
        std::array<std::pair<int, int>, 10> pairs_copy;
        const_pair_input first(pairs.data());
        const_pair_input last(pairs.data() + pairs.size());
        std::copy(first, last, pairs_copy.begin());
        EXPECT_EQ(pairs_copy, pairs);
    }

    {
        std::array<int, 10> firsts_copy;
        const_pair_input first(pairs.data());
        const_pair_input last(pairs.data() + pairs.size());
        for (auto out = firsts_copy.begin(); first != last; ++first) {
            *out++ = first->first;
        }
        EXPECT_EQ(firsts_copy, ints);
    }

    {
        std::array<int, 10> firsts_copy;
        proxy_input_iter<std::pair<int, int> const> first(pairs.data());
        proxy_input_iter<std::pair<int, int> const> last(
            pairs.data() + pairs.size());
        for (auto out = firsts_copy.begin(); first != last; ++first) {
            *out++ = first->first;
        }
        EXPECT_EQ(firsts_copy, ints);
    }
}
