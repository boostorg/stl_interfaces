// Copyright (C) 2019 T. Zachary Laine
//
// Distributed under the Boost Software License, Version 1.0. (See
// accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
#include <boost/stl_interfaces/iterator_interface.hpp>

#include <gtest/gtest.h>

#include <array>
#include <numeric>
#include <type_traits>


struct basic_input_iter
    : boost::stl_interfaces::
          iterator_interface<basic_input_iter, std::input_iterator_tag, int>
{
    basic_input_iter() : it_(nullptr) {}
    basic_input_iter(int * it) : it_(it) {}

    int & operator*() const noexcept { return *it_; }
    basic_input_iter & operator++() noexcept
    {
        ++it_;
        return *this;
    }
    friend bool operator==(basic_input_iter lhs, basic_input_iter rhs) noexcept
    {
        return lhs.it_ == rhs.it_;
    }

    using base_type = boost::stl_interfaces::
        iterator_interface<basic_input_iter, std::input_iterator_tag, int>;
    using base_type::operator++;

private:
    int * it_;
};

BOOST_STL_INTERFACES_STATIC_ASSERT_CONCEPT(
    basic_input_iter, std::input_iterator)
BOOST_STL_INTERFACES_STATIC_ASSERT_ITERATOR_TRAITS(
    basic_input_iter,
    std::input_iterator_tag,
    std::input_iterator_tag,
    int,
    int &,
    int *,
    std::ptrdiff_t)

template<typename ValueType>
struct input_iter : boost::stl_interfaces::iterator_interface<
                        input_iter<ValueType>,
                        std::input_iterator_tag,
                        ValueType>
{
    input_iter() : it_(nullptr) {}
    input_iter(ValueType * it) : it_(it) {}
    template<
        typename ValueType2,
        typename E = std::enable_if_t<std::is_convertible<
            typename ValueType2::value_type *,
            ValueType *>::value>>
    input_iter(ValueType2 it) : it_(it.it_)
    {}

    ValueType & operator*() const noexcept { return *it_; }
    input_iter & operator++() noexcept
    {
        ++it_;
        return *this;
    }
    friend bool operator==(input_iter lhs, input_iter rhs) noexcept
    {
        return lhs.it_ == rhs.it_;
    }

    using base_type = boost::stl_interfaces::iterator_interface<
        input_iter<ValueType>,
        std::input_iterator_tag,
        ValueType>;
    using base_type::operator++;

private:
    ValueType * it_;

    template<typename ValueType2>
    friend struct input_iter;
};

BOOST_STL_INTERFACES_STATIC_ASSERT_CONCEPT(
    input_iter<int>, std::input_iterator)
BOOST_STL_INTERFACES_STATIC_ASSERT_ITERATOR_TRAITS(
    input_iter<int>,
    std::input_iterator_tag,
    std::input_iterator_tag,
    int,
    int &,
    int *,
    std::ptrdiff_t)

using int_input = input_iter<int>;
using const_int_input = input_iter<int const>;
using pair_input = input_iter<std::pair<int, int>>;
using const_pair_input = input_iter<std::pair<int, int> const>;

template<typename ValueType>
struct proxy_input_iter : boost::stl_interfaces::proxy_iterator_interface<
                              proxy_input_iter<ValueType>,
                              std::input_iterator_tag,
                              ValueType>
{
    proxy_input_iter() : it_(nullptr) {}
    proxy_input_iter(ValueType * it) : it_(it) {}
    template<
        typename ValueType2,
        typename E = std::enable_if_t<std::is_convertible<
            typename ValueType2::value_type *,
            ValueType *>::value>>
    proxy_input_iter(ValueType2 it) : it_(it.it_)
    {}

    ValueType operator*() const noexcept { return *it_; }
    proxy_input_iter & operator++() noexcept
    {
        ++it_;
        return *this;
    }
    friend bool operator==(proxy_input_iter lhs, proxy_input_iter rhs) noexcept
    {
        return lhs.it_ == rhs.it_;
    }

    using base_type = boost::stl_interfaces::proxy_iterator_interface<
        proxy_input_iter<ValueType>,
        std::input_iterator_tag,
        ValueType>;
    using base_type::operator++;

private:
    ValueType * it_;

    template<typename ValueType2>
    friend struct proxy_input_iter;
};

using int_pair = std::pair<int, int>;
BOOST_STL_INTERFACES_STATIC_ASSERT_CONCEPT(
    proxy_input_iter<int_pair>, std::input_iterator)
BOOST_STL_INTERFACES_STATIC_ASSERT_ITERATOR_TRAITS(
    proxy_input_iter<int_pair>,
    std::input_iterator_tag,
    std::input_iterator_tag,
    int_pair,
    int_pair,
    boost::stl_interfaces::proxy_arrow_result<int_pair>,
    std::ptrdiff_t)

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

TEST(input, postincrement)
{
    int_input first(ints.data());
    int_input last(ints.data() + ints.size());
    while (first != last)
        first++;
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


////////////////////
// view_interface //
////////////////////
#include "view_tests.hpp"

TEST(input, basic_subrange)
{
    basic_input_iter first(ints.data());
    basic_input_iter last(ints.data() + ints.size());

    auto r = range<boost::stl_interfaces::discontiguous>(first, last);
    auto empty = range<boost::stl_interfaces::discontiguous>(first, first);

    // range begin/end
    {
        std::array<int, 10> ints_copy;
        std::copy(r.begin(), r.end(), ints_copy.begin());
        EXPECT_EQ(ints_copy, ints);

        EXPECT_EQ(empty.begin(), empty.end());
    }

    // empty/op bool
    {
        EXPECT_FALSE(r.empty());
        EXPECT_TRUE(r);

        EXPECT_TRUE(empty.empty());
        EXPECT_FALSE(empty);

        auto const cr = r;
        EXPECT_FALSE(cr.empty());
        EXPECT_TRUE(cr);

        auto const cempty = empty;
        EXPECT_TRUE(cempty.empty());
        EXPECT_FALSE(cempty);
    }

#if 0 // TODO: COMPILE-FAIL test for this.
    // data
    {
        EXPECT_NE(r.data(), nullptr);
        EXPECT_EQ(r.data()[2], 2);

        EXPECT_NE(empty.data(), nullptr);

        auto const cr = r;
        EXPECT_NE(cr.data(), nullptr);
        EXPECT_EQ(cr.data()[2], 2);

        auto const cempty = empty;
        EXPECT_NE(cempty.data(), nullptr);
    }
#endif

#if 0 // TODO: COMPILE-FAIL test for this.
    // size
    {
        EXPECT_EQ(r.size(), 10u);

        EXPECT_EQ(empty.size(), 0u);

        auto const cr = r;
        EXPECT_EQ(cr.size(), 10u);

        auto const cempty = empty;
        EXPECT_EQ(cempty.size(), 0u);
    }
#endif

    // front/back
    {
        EXPECT_EQ(r.front(), 0);
#if 0 // TODO: COMPILE-FAIL test for this.
        EXPECT_EQ(r.back(), 9);
#endif

        auto const cr = r;
        EXPECT_EQ(cr.front(), 0);
    }

#if 0 // TODO: COMPILE-FAIL test for this.
    // op[]
    {
        EXPECT_EQ(r[2], 2);

        auto const cr = r;
        EXPECT_EQ(cr[2], 2);
    }
#endif
}
