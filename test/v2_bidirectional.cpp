// Copyright (C) 2019 T. Zachary Laine
//
// Distributed under the Boost Software License, Version 1.0. (See
// accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
#include <boost/stl_interfaces/iterator_interface.hpp>

#include "ill_formed.hpp"

#include <boost/test/minimal.hpp>

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

BOOST_STL_INTERFACES_STATIC_ASSERT_CONCEPT(
    basic_adapted_bidirectional_ptr_iter, bsi::ranges::bidirectional_iterator)
BOOST_STL_INTERFACES_STATIC_ASSERT_ITERATOR_TRAITS(
    basic_adapted_bidirectional_ptr_iter,
    std::bidirectional_iterator_tag,
    std::bidirectional_iterator_tag,
    int,
    int &,
    int *,
    std::ptrdiff_t)

template<typename ValueType>
struct adapted_bidirectional_ptr_iter
    : bsi::iterator_interface<
          adapted_bidirectional_ptr_iter<ValueType>,
          std::bidirectional_iterator_tag,
          ValueType>
{
    adapted_bidirectional_ptr_iter() : it_(nullptr) {}
    adapted_bidirectional_ptr_iter(ValueType * it) : it_(it) {}

    template<typename ValueType2>
    adapted_bidirectional_ptr_iter(
        adapted_bidirectional_ptr_iter<ValueType2> other) :
        it_(other.it_)
    {}

    template<typename ValueType2>
    friend struct adapted_bidirectional_ptr_iter;

private:
    friend boost::stl_interfaces::access;
    ValueType *& base_reference() noexcept { return it_; }
    ValueType * base_reference() const noexcept { return it_; }

    ValueType * it_;
};

static_assert(
    !boost::stl_interfaces::v1_dtl::ra_iter<
        adapted_bidirectional_ptr_iter<int>>::value,
    "");
static_assert(
    !boost::stl_interfaces::v1_dtl::ra_iter<
        adapted_bidirectional_ptr_iter<int const>>::value,
    "");

BOOST_STL_INTERFACES_STATIC_ASSERT_CONCEPT(
    adapted_bidirectional_ptr_iter<int>, bsi::ranges::bidirectional_iterator)
BOOST_STL_INTERFACES_STATIC_ASSERT_ITERATOR_TRAITS(
    adapted_bidirectional_ptr_iter<int>,
    std::bidirectional_iterator_tag,
    std::bidirectional_iterator_tag,
    int,
    int &,
    int *,
    std::ptrdiff_t)
BOOST_STL_INTERFACES_STATIC_ASSERT_CONCEPT(
    adapted_bidirectional_ptr_iter<int const>,
    bsi::ranges::bidirectional_iterator)
BOOST_STL_INTERFACES_STATIC_ASSERT_ITERATOR_TRAITS(
    adapted_bidirectional_ptr_iter<int const>,
    std::bidirectional_iterator_tag,
    std::bidirectional_iterator_tag,
    int const,
    int const &,
    int const *,
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

BOOST_STL_INTERFACES_STATIC_ASSERT_CONCEPT(
    basic_adapted_bidirectional_list_iter, bsi::ranges::bidirectional_iterator)
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

struct basic_proxy_bidirectional_iter : bsi::proxy_iterator_interface<
                                            basic_proxy_bidirectional_iter,
                                            std::bidirectional_iterator_tag,
                                            int>
{
    basic_proxy_bidirectional_iter() : it_(nullptr) {}
    basic_proxy_bidirectional_iter(int * it) : it_(it) {}

    int operator*() const { return *it_; }
    basic_proxy_bidirectional_iter & operator++()
    {
        ++it_;
        return *this;
    }
    basic_proxy_bidirectional_iter & operator--()
    {
        --it_;
        return *this;
    }
    friend bool operator==(
        basic_proxy_bidirectional_iter lhs,
        basic_proxy_bidirectional_iter rhs) noexcept
    {
        return lhs.it_ == rhs.it_;
    }

    using base_type = bsi::proxy_iterator_interface<
        basic_proxy_bidirectional_iter,
        std::bidirectional_iterator_tag,
        int>;
    using base_type::operator++;
    using base_type::operator--;

private:
    int * it_;
};

BOOST_STL_INTERFACES_STATIC_ASSERT_CONCEPT(
    basic_proxy_bidirectional_iter, bsi::ranges::bidirectional_iterator)
BOOST_STL_INTERFACES_STATIC_ASSERT_ITERATOR_TRAITS(
    basic_proxy_bidirectional_iter,
    std::bidirectional_iterator_tag,
    std::bidirectional_iterator_tag,
    int,
    int,
    boost::stl_interfaces::proxy_arrow_result<int>,
    std::ptrdiff_t)

std::array<int, 10> ints = {{0, 1, 2, 3, 4, 5, 6, 7, 8, 9}};


int test_main(int, char * [])
{

{
    basic_bidirectional_iter first(ints.data());
    basic_bidirectional_iter last(ints.data() + ints.size());

    {
        std::array<int, 10> ints_copy;
        std::copy(first, last, ints_copy.begin());
        BOOST_CHECK(ints_copy == ints);
    }

    {
        std::array<int, 10> ints_copy;
        std::copy(
            std::make_reverse_iterator(last),
            std::make_reverse_iterator(first),
            ints_copy.begin());
        std::reverse(ints_copy.begin(), ints_copy.end());
        BOOST_CHECK(ints_copy == ints);
    }

    {
        std::array<int, 10> iota_ints;
        basic_bidirectional_iter first(iota_ints.data());
        basic_bidirectional_iter last(iota_ints.data() + iota_ints.size());
        std::iota(first, last, 0);
        BOOST_CHECK(iota_ints == ints);
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
        BOOST_CHECK(iota_ints == ints);
    }
}


{
    basic_adapted_bidirectional_ptr_iter first(ints.data());
    basic_adapted_bidirectional_ptr_iter last(ints.data() + ints.size());

    {
        std::array<int, 10> ints_copy;
        std::copy(first, last, ints_copy.begin());
        BOOST_CHECK(ints_copy == ints);
    }

    {
        std::array<int, 10> ints_copy;
        std::copy(
            std::make_reverse_iterator(last),
            std::make_reverse_iterator(first),
            ints_copy.begin());
        std::reverse(ints_copy.begin(), ints_copy.end());
        BOOST_CHECK(ints_copy == ints);
    }

    {
        std::array<int, 10> iota_ints;
        basic_adapted_bidirectional_ptr_iter first(iota_ints.data());
        basic_adapted_bidirectional_ptr_iter last(
            iota_ints.data() + iota_ints.size());
        std::iota(first, last, 0);
        BOOST_CHECK(iota_ints == ints);
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
        BOOST_CHECK(iota_ints == ints);
    }
}


{
    std::list<int> ints = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};

    basic_adapted_bidirectional_list_iter first(ints.begin());
    basic_adapted_bidirectional_list_iter last(ints.end());

    {
        std::list<int> ints_copy;
        std::copy(first, last, std::back_inserter(ints_copy));
        BOOST_CHECK(ints_copy == ints);
    }

    {
        std::list<int> ints_copy;
        std::copy(
            std::make_reverse_iterator(last),
            std::make_reverse_iterator(first),
            std::back_inserter(ints_copy));
        std::reverse(ints_copy.begin(), ints_copy.end());
        BOOST_CHECK(ints_copy == ints);
    }
}


{
    {
        bidirectional first(ints.data());
        bidirectional last(ints.data() + ints.size());
        const_bidirectional first_copy(first);
        const_bidirectional last_copy(last);
        std::equal(first, last, first_copy, last_copy);
    }

    {
        adapted_bidirectional_ptr_iter<int> first(ints.data());
        adapted_bidirectional_ptr_iter<int> last(ints.data() + ints.size());
        adapted_bidirectional_ptr_iter<int const> first_copy(
            (int const *)ints.data());
        adapted_bidirectional_ptr_iter<int const> last_copy(
            (int const *)ints.data() + ints.size());
        std::equal(first, last, first_copy, last_copy);
    }
}


{
    {
        bidirectional first(ints.data());
        bidirectional last(ints.data() + ints.size());
        const_bidirectional first_const(first);
        const_bidirectional last_const(last);

        BOOST_CHECK(first == first_const);
        BOOST_CHECK(first_const == first);
        BOOST_CHECK(first != last_const);
        BOOST_CHECK(last_const != first);
    }

    {
        adapted_bidirectional_ptr_iter<int> first(ints.data());
        adapted_bidirectional_ptr_iter<int> last(ints.data() + ints.size());
        adapted_bidirectional_ptr_iter<int const> first_const(
            (int const *)ints.data());
        adapted_bidirectional_ptr_iter<int const> last_const(
            (int const *)ints.data() + ints.size());

        static_assert(
            !boost::stl_interfaces::v1_dtl::ra_iter<
                adapted_bidirectional_ptr_iter<int>>::value,
            "");

        BOOST_CHECK(first == first_const);
        BOOST_CHECK(first_const == first);
        BOOST_CHECK(first != last_const);
        BOOST_CHECK(last_const != first);
    }
}


{
    {
        bidirectional first(ints.data());
        bidirectional last(ints.data() + ints.size());
        while (first != last && !(first == last))
            first++;
    }

    {
        bidirectional first(ints.data());
        bidirectional last(ints.data() + ints.size());
        while (first != last && !(first == last))
            last--;
    }

    {
        basic_bidirectional_iter first(ints.data());
        basic_bidirectional_iter last(ints.data() + ints.size());
        while (first != last && !(first == last))
            first++;
    }

    {
        basic_bidirectional_iter first(ints.data());
        basic_bidirectional_iter last(ints.data() + ints.size());
        while (first != last && !(first == last))
            last--;
    }

    {
        basic_adapted_bidirectional_ptr_iter first(ints.data());
        basic_adapted_bidirectional_ptr_iter last(ints.data() + ints.size());
        while (first != last && !(first == last))
            first++;
    }

    {
        basic_adapted_bidirectional_ptr_iter first(ints.data());
        basic_adapted_bidirectional_ptr_iter last(ints.data() + ints.size());
        while (first != last && !(first == last))
            last--;
    }

    {
        std::list<int> ints = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
        basic_adapted_bidirectional_list_iter first(ints.begin());
        basic_adapted_bidirectional_list_iter last(ints.end());
        while (first != last && !(first == last))
            first++;
    }

    {
        std::list<int> ints = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
        basic_adapted_bidirectional_list_iter first(ints.begin());
        basic_adapted_bidirectional_list_iter last(ints.end());
        while (first != last && !(first == last))
            last--;
    }
}


{
    bidirectional first(ints.data());
    bidirectional last(ints.data() + ints.size());

    {
        std::array<int, 10> ints_copy;
        std::copy(first, last, ints_copy.begin());
        BOOST_CHECK(ints_copy == ints);
    }

    {
        std::array<int, 10> ints_copy;
        std::copy(
            std::make_reverse_iterator(last),
            std::make_reverse_iterator(first),
            ints_copy.begin());
        std::reverse(ints_copy.begin(), ints_copy.end());
        BOOST_CHECK(ints_copy == ints);
    }

    {
        std::array<int, 10> iota_ints;
        bidirectional first(iota_ints.data());
        bidirectional last(iota_ints.data() + iota_ints.size());
        std::iota(first, last, 0);
        BOOST_CHECK(iota_ints == ints);
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
        BOOST_CHECK(iota_ints == ints);
    }
}


{
    const_bidirectional first(ints.data());
    const_bidirectional last(ints.data() + ints.size());

    {
        std::array<int, 10> ints_copy;
        std::copy(first, last, ints_copy.begin());
        BOOST_CHECK(ints_copy == ints);
    }

    {
        BOOST_CHECK(std::binary_search(first, last, 3));
        BOOST_CHECK(std::binary_search(
            std::make_reverse_iterator(last),
            std::make_reverse_iterator(first),
            3,
            std::greater<>{}));
    }
}

    return 0;
}
