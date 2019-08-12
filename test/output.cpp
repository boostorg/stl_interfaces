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


struct basic_output_iter
    : boost::iterator_facade::
          iterator_interface<basic_output_iter, std::output_iterator_tag, int>
{
    basic_output_iter() : it_(nullptr) {}
    basic_output_iter(int * it) : it_(it) {}

    int & operator*() noexcept { return *it_; }
    void next() noexcept { ++it_; }

private:
    int * it_;
};

using output = basic_output_iter;

BOOST_ITERATOR_FACADE_STATIC_ASSERT_CONCEPT(output, std::output_iterator)
BOOST_ITERATOR_FACADE_STATIC_ASSERT_ITERATOR_TRAITS(
    output,
    std::output_iterator_tag,
    std::output_iterator_tag,
    int,
    int &,
    void,
    std::ptrdiff_t)

template<typename Container>
struct back_insert_iter : boost::iterator_facade::iterator_interface<
                              back_insert_iter<Container>,
                              std::output_iterator_tag,
                              typename Container::value_type,
                              back_insert_iter<Container> &>
{
    back_insert_iter() : c_(nullptr) {}
    back_insert_iter(Container & c) : c_(std::addressof(c)) {}

    back_insert_iter & operator*() noexcept { return *this; }
    void next() noexcept {}

    back_insert_iter & operator=(typename Container::value_type const & v)
    {
        c_->push_back(v);
        return *this;
    }
    back_insert_iter & operator=(typename Container::value_type && v)
    {
        c_->push_back(std::move(v));
        return *this;
    }

private:
    Container * c_;
};

using back_insert = back_insert_iter<std::vector<int>>;

BOOST_ITERATOR_FACADE_STATIC_ASSERT_CONCEPT(back_insert, std::output_iterator)
BOOST_ITERATOR_FACADE_STATIC_ASSERT_ITERATOR_TRAITS(
    back_insert,
    std::output_iterator_tag,
    std::output_iterator_tag,
    int,
    back_insert &,
    void,
    std::ptrdiff_t)


std::vector<int> ints = {{0, 1, 2, 3, 4, 5, 6, 7, 8, 9}};


TEST(output, basic_std_copy)
{
    std::vector<int> ints_copy(ints.size());
    std::copy(ints.begin(), ints.end(), output(ints_copy.data()));
    EXPECT_EQ(ints_copy, ints);
}

TEST(output, back_insert_std_copy)
{
    std::vector<int> ints_copy;
    std::copy(ints.begin(), ints.end(), back_insert(ints_copy));
    EXPECT_EQ(ints_copy, ints);
}
