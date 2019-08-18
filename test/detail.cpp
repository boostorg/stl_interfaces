// Copyright (C) 2019 T. Zachary Laine
//
// Distributed under the Boost Software License, Version 1.0. (See
// accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
#include <boost/stl_interfaces/iterator_interface.hpp>
#include <boost/stl_interfaces/view_interface.hpp>
#include <boost/stl_interfaces/container_interface.hpp>

#include <boost/mpl/assert.hpp>

#include <gtest/gtest.h>

#include <array>
#include <list>
#include <vector>


namespace detail = boost::stl_interfaces::detail;


// iter_difference_t
BOOST_MPL_ASSERT(
    (std::is_same<detail::iter_difference_t<int *>, std::ptrdiff_t>));
BOOST_MPL_ASSERT((std::is_same<
                  detail::iter_difference_t<std::vector<double>::iterator>,
                  std::vector<double>::difference_type>));
BOOST_MPL_ASSERT((std::is_same<
                  detail::iter_difference_t<std::list<double>::iterator>,
                  std::list<double>::difference_type>));

struct ridiculous_range
{
    int * begin() { return nullptr; }
    double end() { return 1.3; }
};

// iterator_t
BOOST_MPL_ASSERT((std::is_same<
                  detail::iterator_t<std::vector<double>>,
                  std::vector<double>::iterator>));
BOOST_MPL_ASSERT((std::is_same<
                  detail::iterator_t<std::list<double>>,
                  std::list<double>::iterator>));
BOOST_MPL_ASSERT((std::is_same<detail::iterator_t<ridiculous_range>, int *>));

// sentinel_t
BOOST_MPL_ASSERT((std::is_same<
                  detail::sentinel_t<std::vector<double>>,
                  std::vector<double>::iterator>));
BOOST_MPL_ASSERT((std::is_same<
                  detail::sentinel_t<std::list<double>>,
                  std::list<double>::iterator>));
BOOST_MPL_ASSERT((std::is_same<detail::sentinel_t<ridiculous_range>, double>));

// range_difference_t
BOOST_MPL_ASSERT(
    (std::is_same<
        detail::range_difference_t<std::vector<double>>,
        std::iterator_traits<std::vector<double>::iterator>::difference_type>));
BOOST_MPL_ASSERT(
    (std::is_same<
        detail::range_difference_t<std::list<double>>,
        std::iterator_traits<std::list<double>::iterator>::difference_type>));
BOOST_MPL_ASSERT((std::is_same<
                  detail::range_difference_t<ridiculous_range>,
                  std::ptrdiff_t>));

// common_range
static_assert(detail::common_range<std::vector<double>>::value, "");
static_assert(detail::common_range<std::list<double>>::value, "");
static_assert(!detail::common_range<ridiculous_range>::value, "");


struct no_clear
{};

TEST(detail, clear_impl)
{
    {
        no_clear nc;
        detail::clear_impl<no_clear>::call(nc);
    }
    {
        std::vector<int> vec(10);
        detail::clear_impl<std::vector<int>>::call(vec);
        EXPECT_TRUE(vec.empty());
    }
}

TEST(detail, n_iter)
{
    std::array<int, 5> ints = {{0, 1, 2, 3, 4}};
    int const new_value = 6;
    detail::n_iter<int, int> first = detail::make_n_iter(new_value, 3);
    detail::n_iter<int, int> last = detail::make_n_iter_end(new_value, 3);
    std::copy(first, last, &ints[1]);
    EXPECT_EQ(ints, (std::array<int, 5>{{0, 6, 6, 6, 4}}));
}
