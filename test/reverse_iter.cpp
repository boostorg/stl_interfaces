// Copyright (C) 2019 T. Zachary Laine
//
// Distributed under the Boost Software License, Version 1.0. (See
// accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
#include <boost/stl_interfaces/iterator_interface.hpp>
#include <boost/stl_interfaces/reverse_iterator.hpp>

#include <gtest/gtest.h>

#include <algorithm>
#include <array>
#include <list>
#include <vector>


struct zip_iter : boost::stl_interfaces::proxy_iterator_interface<
                      zip_iter,
                      std::random_access_iterator_tag,
                      std::tuple<int, int>,
                      std::tuple<int &, int &>>
{
    zip_iter() : it1_(nullptr), it2_(nullptr) {}
    zip_iter(int * it1, int * it2) : it1_(it1), it2_(it2) {}

    std::tuple<int &, int &> operator*() const
    {
        return std::tuple<int &, int &>{*it1_, *it2_};
    }
    zip_iter & operator+=(std::ptrdiff_t i)
    {
        it1_ += i;
        it2_ += i;
        return *this;
    }
    friend std::ptrdiff_t operator-(zip_iter lhs, zip_iter rhs) noexcept
    {
        return lhs.it1_ - rhs.it1_;
    }

private:
    int * it1_;
    int * it2_;
};


TEST(reverse_iter, std_list_iterator)
{
    std::list<int> ints = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};

    auto first = boost::stl_interfaces::make_reverse_iterator(ints.end());
    auto last = boost::stl_interfaces::make_reverse_iterator(ints.begin());

    {
        auto ints_copy = ints;
        std::reverse(ints_copy.begin(), ints_copy.end());
        EXPECT_TRUE(
            std::equal(first, last, ints_copy.begin(), ints_copy.end()));
    }

    {
        std::list<int> ints_copy;
        std::reverse_copy(first, last, std::back_inserter(ints_copy));
        EXPECT_EQ(ints_copy, ints);
    }

    {
        std::size_t count = 0;
        for (auto it = first; it != last; ++it) {
            ++count;
        }
        EXPECT_EQ(count, ints.size());
    }

    {
        std::size_t count = 0;
        for (auto it = first; it != last; it++) {
            ++count;
        }
        EXPECT_EQ(count, ints.size());
    }

    {
        std::size_t count = 0;
        for (auto it = last; it != first; --it) {
            ++count;
        }
        EXPECT_EQ(count, ints.size());
    }

    {
        std::size_t count = 0;
        for (auto it = last; it != first; it--) {
            ++count;
        }
        EXPECT_EQ(count, ints.size());
    }
}

TEST(reverse_iter, std_vector_iterator)
{
    std::vector<int> ints = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};

    auto first = boost::stl_interfaces::make_reverse_iterator(ints.end());
    auto last = boost::stl_interfaces::make_reverse_iterator(ints.begin());

    {
        auto ints_copy = ints;
        std::reverse(ints_copy.begin(), ints_copy.end());
        EXPECT_EQ(first - last, ints_copy.begin() - ints_copy.end());
        EXPECT_TRUE(
            std::equal(first, last, ints_copy.begin(), ints_copy.end()));
    }

    {
        std::vector<int> ints_copy;
        std::reverse_copy(first, last, std::back_inserter(ints_copy));
        EXPECT_EQ(ints_copy, ints);
    }

    {
        std::size_t count = 0;
        for (auto it = first; it != last; ++it) {
            ++count;
        }
        EXPECT_EQ(count, ints.size());
    }

    {
        std::size_t count = 0;
        for (auto it = first; it != last; it++) {
            ++count;
        }
        EXPECT_EQ(count, ints.size());
    }

    {
        std::size_t count = 0;
        for (auto it = last; it != first; --it) {
            ++count;
        }
        EXPECT_EQ(count, ints.size());
    }

    {
        std::size_t count = 0;
        for (auto it = last; it != first; it--) {
            ++count;
        }
        EXPECT_EQ(count, ints.size());
    }
}

TEST(reverse_iter, zip_iterator)
{
    std::array<int, 10> ints = {{0, 1, 2, 3, 4, 5, 6, 7, 8, 9}};
    std::array<int, 10> ones = {{1, 1, 1, 1, 1, 1, 1, 1, 1, 1}};
    std::array<std::tuple<int, int>, 10> tuples = {{
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

    auto first = boost::stl_interfaces::make_reverse_iterator(
        zip_iter(ints.data() + ints.size(), ones.data() + ones.size()));
    auto last = boost::stl_interfaces::make_reverse_iterator(
        zip_iter(ints.data(), ones.data()));

    {
        auto tuples_copy = tuples;
        std::reverse(tuples_copy.begin(), tuples_copy.end());
        EXPECT_EQ(first - last, tuples_copy.begin() - tuples_copy.end());
        EXPECT_TRUE(
            std::equal(first, last, tuples_copy.begin(), tuples_copy.end()));
    }

    {
        std::array<std::tuple<int, int>, 10> tuples_copy;
        std::reverse_copy(first, last, tuples_copy.begin());
        EXPECT_EQ(tuples_copy, tuples);
    }

    {
        std::size_t count = 0;
        for (auto it = first; it != last; ++it) {
            ++count;
        }
        EXPECT_EQ(count, tuples.size());
    }

    {
        std::size_t count = 0;
        for (auto it = first; it != last; it++) {
            ++count;
        }
        EXPECT_EQ(count, tuples.size());
    }

    {
        std::size_t count = 0;
        for (auto it = last; it != first; --it) {
            ++count;
        }
        EXPECT_EQ(count, tuples.size());
    }

    {
        std::size_t count = 0;
        for (auto it = last; it != first; it--) {
            ++count;
        }
        EXPECT_EQ(count, tuples.size());
    }
}
