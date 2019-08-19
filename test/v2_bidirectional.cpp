// Copyright (C) 2019 T. Zachary Laine
//
// Distributed under the Boost Software License, Version 1.0. (See
// accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
#include <boost/stl_interfaces/iterator_interface.hpp>

#include "ill_formed.hpp"

#include <gtest/gtest.h>

#include <array>
#include <numeric>
#include <list>
#include <type_traits>


namespace bsi = boost::stl_interfaces::v2;

struct basic_bidirectional_iter : bsi::iterator_interface<
                                      basic_bidirectional_iter,
                                      std::bidirectional_iterator_tag,
                                      int>
{
    basic_bidirectional_iter() : it_(nullptr) {}
    basic_bidirectional_iter(int * it) : it_(it) {}

    int & operator*() const { return *it_; }
    basic_bidirectional_iter & operator++()
    {
        ++it_;
        return *this;
    }
    basic_bidirectional_iter & operator--()
    {
        --it_;
        return *this;
    }
    friend bool operator==(
        basic_bidirectional_iter lhs, basic_bidirectional_iter rhs) noexcept
    {
        return lhs.it_ == rhs.it_;
    }

    using base_type = bsi::iterator_interface<
        basic_bidirectional_iter,
        std::bidirectional_iterator_tag,
        int>;
    using base_type::operator++;
    using base_type::operator--;

private:
    int * it_;
};

BOOST_STL_INTERFACES_STATIC_ASSERT_CONCEPT(
    basic_bidirectional_iter, bsi::ranges::bidirectional_iterator)
BOOST_STL_INTERFACES_STATIC_ASSERT_ITERATOR_TRAITS(
    basic_bidirectional_iter,
    std::bidirectional_iterator_tag,
    std::bidirectional_iterator_tag,
    int,
    int &,
    int *,
    std::ptrdiff_t)

static_assert(
    !boost::stl_interfaces::v1::v1_dtl::
        plus_eq<basic_bidirectional_iter, std::ptrdiff_t>::value,
    "");

struct basic_adapted_bidirectional_ptr_iter
    : bsi::iterator_interface<
          basic_adapted_bidirectional_ptr_iter,
          std::bidirectional_iterator_tag,
          int>
{
    basic_adapted_bidirectional_ptr_iter() : it_(nullptr) {}
    basic_adapted_bidirectional_ptr_iter(int * it) : it_(it) {}

private:
    friend boost::stl_interfaces::access;
    int *& base_reference() noexcept { return it_; }
    int * base_reference() const noexcept { return it_; }

    int * it_;
};

// TODO: Broken.
// BOOST_STL_INTERFACES_STATIC_ASSERT_CONCEPT(
//     basic_adapted_bidirectional_ptr_iter, bsi::ranges::bidirectional_iterator)
BOOST_STL_INTERFACES_STATIC_ASSERT_ITERATOR_TRAITS(
    basic_adapted_bidirectional_ptr_iter,
    std::bidirectional_iterator_tag,
    std::bidirectional_iterator_tag,
    int,
    int &,
    int *,
    std::ptrdiff_t)

struct basic_adapted_bidirectional_list_iter
    : bsi::iterator_interface<
          basic_adapted_bidirectional_list_iter,
          std::bidirectional_iterator_tag,
          int>
{
    basic_adapted_bidirectional_list_iter() : it_() {}
    basic_adapted_bidirectional_list_iter(std::list<int>::iterator it) : it_(it) {}

private:
    friend boost::stl_interfaces::access;
    std::list<int>::iterator & base_reference() noexcept { return it_; }
    std::list<int>::iterator base_reference() const noexcept { return it_; }

    std::list<int>::iterator it_;
};

// TODO: Broken.
// BOOST_STL_INTERFACES_STATIC_ASSERT_CONCEPT(
//     basic_adapted_bidirectional_list_iter, bsi::ranges::bidirectional_iterator)
BOOST_STL_INTERFACES_STATIC_ASSERT_ITERATOR_TRAITS(
    basic_adapted_bidirectional_list_iter,
    std::bidirectional_iterator_tag,
    std::bidirectional_iterator_tag,
    int,
    int &,
    int *,
    std::ptrdiff_t)

template<typename ValueType>
struct bidirectional_iter : bsi::iterator_interface<
                                bidirectional_iter<ValueType>,
                                std::bidirectional_iterator_tag,
                                ValueType>
{
    bidirectional_iter() : it_(nullptr) {}
    bidirectional_iter(ValueType * it) : it_(it) {}
    template<
        typename ValueType2,
        typename E = std::enable_if_t<std::is_convertible<
            typename ValueType2::value_type *,
            ValueType *>::value>>
    bidirectional_iter(ValueType2 it) : it_(it.it_)
    {}

    ValueType & operator*() const { return *it_; }
    bidirectional_iter & operator++()
    {
        ++it_;
        return *this;
    }
    bidirectional_iter & operator--()
    {
        --it_;
        return *this;
    }
    friend bool
    operator==(bidirectional_iter lhs, bidirectional_iter rhs) noexcept
    {
        return lhs.it_ == rhs.it_;
    }

    using base_type = bsi::iterator_interface<
        bidirectional_iter<ValueType>,
        std::bidirectional_iterator_tag,
        ValueType>;
    using base_type::operator++;
    using base_type::operator--;

private:
    ValueType * it_;

    template<typename ValueType2>
    friend struct bidirectional_iter;
};

using bidirectional = bidirectional_iter<int>;
using const_bidirectional = bidirectional_iter<int const>;

BOOST_STL_INTERFACES_STATIC_ASSERT_CONCEPT(
    bidirectional, bsi::ranges::bidirectional_iterator)
BOOST_STL_INTERFACES_STATIC_ASSERT_ITERATOR_TRAITS(
    bidirectional,
    std::bidirectional_iterator_tag,
    std::bidirectional_iterator_tag,
    int,
    int &,
    int *,
    std::ptrdiff_t)

BOOST_STL_INTERFACES_STATIC_ASSERT_CONCEPT(
    const_bidirectional, bsi::ranges::bidirectional_iterator)
BOOST_STL_INTERFACES_STATIC_ASSERT_ITERATOR_TRAITS(
    const_bidirectional,
    std::bidirectional_iterator_tag,
    std::bidirectional_iterator_tag,
    int const,
    int const &,
    int const *,
    std::ptrdiff_t)

#if 0 // TODO: Call ranges algorithms with this.
struct basic_proxy_bidirectional_iter : bsi::iterator_interface<
                                            basic_proxy_bidirectional_iter,
                                            std::bidirectional_iterator_tag,
                                            int,
                                            int>
{
    basic_proxy_bidirectional_iter() : it_(nullptr) {}
    basic_proxy_bidirectional_iter(int * it) : it_(it) {}

    int operator*() const { return *it_; }
    basic_proxy_bidirectional_iter & operator++() { ++it_; }
    basic_proxy_bidirectional_iter & operator--() { --it_; }
    friend bool operator==(
        basic_proxy_bidirectional_iter lhs,
        basic_proxy_bidirectional_iter rhs) noexcept
    {
        return lhs.it_ == rhs.it_;
    }

    using base_type = bsi::iterator_interface<
        basic_proxy_bidirectional_iter,
        std::bidirectional_iterator_tag,
        int,
        int>;
    using base_type::operator++;
    using base_type::operator--;

private:
    int * it_;
};
#endif

std::array<int, 10> ints = {{0, 1, 2, 3, 4, 5, 6, 7, 8, 9}};


TEST(bidirectional, basic_std_copy)
{
    basic_bidirectional_iter first(ints.data());
    basic_bidirectional_iter last(ints.data() + ints.size());

    {
        std::array<int, 10> ints_copy;
        std::copy(first, last, ints_copy.begin());
        EXPECT_EQ(ints_copy, ints);
    }

    {
        std::array<int, 10> ints_copy;
        std::copy(
            std::make_reverse_iterator(last),
            std::make_reverse_iterator(first),
            ints_copy.begin());
        std::reverse(ints_copy.begin(), ints_copy.end());
        EXPECT_EQ(ints_copy, ints);
    }

    {
        std::array<int, 10> iota_ints;
        basic_bidirectional_iter first(iota_ints.data());
        basic_bidirectional_iter last(iota_ints.data() + iota_ints.size());
        std::iota(first, last, 0);
        EXPECT_EQ(iota_ints, ints);
    }

    {
        std::array<int, 10> iota_ints;
        basic_bidirectional_iter first(iota_ints.data());
        basic_bidirectional_iter last(iota_ints.data() + iota_ints.size());
        std::iota(
            std::make_reverse_iterator(last),
            std::make_reverse_iterator(first),
            0);
        std::reverse(iota_ints.begin(), iota_ints.end());
        EXPECT_EQ(iota_ints, ints);
    }
}

TEST(bidirectional, basic_adapted_ptr_std_copy)
{
    basic_adapted_bidirectional_ptr_iter first(ints.data());
    basic_adapted_bidirectional_ptr_iter last(ints.data() + ints.size());

    {
        std::array<int, 10> ints_copy;
        std::copy(first, last, ints_copy.begin());
        EXPECT_EQ(ints_copy, ints);
    }

    {
        std::array<int, 10> ints_copy;
        std::copy(
            std::make_reverse_iterator(last),
            std::make_reverse_iterator(first),
            ints_copy.begin());
        std::reverse(ints_copy.begin(), ints_copy.end());
        EXPECT_EQ(ints_copy, ints);
    }

    {
        std::array<int, 10> iota_ints;
        basic_adapted_bidirectional_ptr_iter first(iota_ints.data());
        basic_adapted_bidirectional_ptr_iter last(
            iota_ints.data() + iota_ints.size());
        std::iota(first, last, 0);
        EXPECT_EQ(iota_ints, ints);
    }

    {
        std::array<int, 10> iota_ints;
        basic_adapted_bidirectional_ptr_iter first(iota_ints.data());
        basic_adapted_bidirectional_ptr_iter last(
            iota_ints.data() + iota_ints.size());
        std::iota(
            std::make_reverse_iterator(last),
            std::make_reverse_iterator(first),
            0);
        std::reverse(iota_ints.begin(), iota_ints.end());
        EXPECT_EQ(iota_ints, ints);
    }
}

TEST(bidirectional, basic_adapted_list_std_copy)
{
    std::list<int> ints = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};

    basic_adapted_bidirectional_list_iter first(ints.begin());
    basic_adapted_bidirectional_list_iter last(ints.end());

    {
        std::list<int> ints_copy;
        std::copy(first, last, std::back_inserter(ints_copy));
        EXPECT_EQ(ints_copy, ints);
    }

    {
        std::list<int> ints_copy;
        std::copy(
            std::make_reverse_iterator(last),
            std::make_reverse_iterator(first),
            std::back_inserter(ints_copy));
        std::reverse(ints_copy.begin(), ints_copy.end());
        EXPECT_EQ(ints_copy, ints);
    }
}

TEST(bidirectional, mutable_to_const_conversions)
{
    bidirectional first(ints.data());
    bidirectional last(ints.data() + ints.size());
    const_bidirectional first_copy(first);
    const_bidirectional last_copy(last);
    std::equal(first, last, first_copy, last_copy);
}

TEST(bidirectional, postincrement_preincrement)
{
    {
        bidirectional first(ints.data());
        bidirectional last(ints.data() + ints.size());
        while (first != last)
            first++;
    }

    {
        bidirectional first(ints.data());
        bidirectional last(ints.data() + ints.size());
        while (first != last)
            last--;
    }

    {
        basic_bidirectional_iter first(ints.data());
        basic_bidirectional_iter last(ints.data() + ints.size());
        while (first != last)
            first++;
    }

    {
        basic_bidirectional_iter first(ints.data());
        basic_bidirectional_iter last(ints.data() + ints.size());
        while (first != last)
            last--;
    }

    {
        basic_adapted_bidirectional_ptr_iter first(ints.data());
        basic_adapted_bidirectional_ptr_iter last(ints.data() + ints.size());
        while (first != last)
            first++;
    }

    {
        basic_adapted_bidirectional_ptr_iter first(ints.data());
        basic_adapted_bidirectional_ptr_iter last(ints.data() + ints.size());
        while (first != last)
            last--;
    }

    {
        std::list<int> ints = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
        basic_adapted_bidirectional_list_iter first(ints.begin());
        basic_adapted_bidirectional_list_iter last(ints.end());
        while (first != last)
            first++;
    }

    {
        std::list<int> ints = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
        basic_adapted_bidirectional_list_iter first(ints.begin());
        basic_adapted_bidirectional_list_iter last(ints.end());
        while (first != last)
            last--;
    }
}

TEST(bidirectional, std_copy)
{
    bidirectional first(ints.data());
    bidirectional last(ints.data() + ints.size());

    {
        std::array<int, 10> ints_copy;
        std::copy(first, last, ints_copy.begin());
        EXPECT_EQ(ints_copy, ints);
    }

    {
        std::array<int, 10> ints_copy;
        std::copy(
            std::make_reverse_iterator(last),
            std::make_reverse_iterator(first),
            ints_copy.begin());
        std::reverse(ints_copy.begin(), ints_copy.end());
        EXPECT_EQ(ints_copy, ints);
    }

    {
        std::array<int, 10> iota_ints;
        bidirectional first(iota_ints.data());
        bidirectional last(iota_ints.data() + iota_ints.size());
        std::iota(first, last, 0);
        EXPECT_EQ(iota_ints, ints);
    }

    {
        std::array<int, 10> iota_ints;
        bidirectional first(iota_ints.data());
        bidirectional last(iota_ints.data() + iota_ints.size());
        std::iota(
            std::make_reverse_iterator(last),
            std::make_reverse_iterator(first),
            0);
        std::reverse(iota_ints.begin(), iota_ints.end());
        EXPECT_EQ(iota_ints, ints);
    }
}

TEST(bidirectional, const_std_copy)
{
    const_bidirectional first(ints.data());
    const_bidirectional last(ints.data() + ints.size());

    {
        std::array<int, 10> ints_copy;
        std::copy(first, last, ints_copy.begin());
        EXPECT_EQ(ints_copy, ints);
    }

    {
        EXPECT_TRUE(std::binary_search(first, last, 3));
        EXPECT_TRUE(std::binary_search(
            std::make_reverse_iterator(last),
            std::make_reverse_iterator(first),
            3,
            std::greater<>{}));
    }
}
