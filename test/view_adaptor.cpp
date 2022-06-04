// Copyright (C) 2022 T. Zachary Laine
//
// Distributed under the Boost Software License, Version 1.0. (See
// accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
#include <boost/stl_interfaces/reverse_iterator.hpp>
#include <boost/stl_interfaces/view_interface.hpp>
#include <boost/stl_interfaces/view_adaptor.hpp>

#include "ill_formed.hpp"
#include "../example/all_view.hpp"
#include "../example/reverse_view.hpp"

#include <boost/core/lightweight_test.hpp>

#include <algorithm>
#include <vector>


    namespace detail {
#if BOOST_STL_INTERFACES_USE_CONCEPTS
        template<std::ranges::view View>
        requires std::is_object_v<View>
#else
        template<
            typename View,
            typename Enable = std::enable_if_t<std::is_object<View>::value>>
#endif
        struct take_view
            : boost::stl_interfaces::view_interface<take_view<View>>
        {
            template<typename Iter>
            struct counted_iterator
                : boost::stl_interfaces::iterator_interface<
                      counted_iterator<Iter>,
                      std::forward_iterator_tag,
                      typename std::iterator_traits<Iter>::value_type,
                      typename std::iterator_traits<Iter>::reference,
                      typename std::iterator_traits<Iter>::pointer,
                      typename std::iterator_traits<Iter>::difference_type>
            {
                constexpr counted_iterator() = default;
                constexpr explicit counted_iterator(Iter it, int n) :
                    it_(std::move(it)), n_(n)
                {}

                constexpr Iter base() const { return it_; }
                constexpr int count() const { return n_; }

                constexpr counted_iterator & operator++()
                {
                    ++it_;
                    --n_;
                    return *this;
                }

            private:
                friend boost::stl_interfaces::access;
                constexpr Iter & base_reference() { return it_; }
                constexpr Iter const & base_reference() const { return it_; }

                template<typename Iter2>
                friend struct counted_iterator;

                Iter it_;
                int n_;
            };

            template<typename Sentinel>
            struct counted_sentinel
            {
                counted_sentinel() = default;
                explicit counted_sentinel(Sentinel sent) : sent_(sent) {}

                template<typename Iter>
                friend constexpr bool
                operator==(counted_iterator<Iter> it, counted_sentinel s)
                {
                    return !it.count() || it.base() == s.sent_;
                }
                template<typename Iter>
                friend constexpr bool
                operator!=(counted_iterator<Iter> it, counted_sentinel s)
                {
                    return !(it == s);
                }

            private:
                Sentinel sent_;
            };

            using iterator = counted_iterator<iterator_t<View>>;
            using sentinel = counted_sentinel<sentinel_t<View>>;

#if BOOST_STL_INTERFACES_USE_CONCEPTS
            template<typename View2>
            requires std::is_same_v<std::remove_reference_t<View2>, View>
#else
            template<
                typename View2,
                typename E = std::enable_if_t<
                    std::is_same<std::remove_reference_t<View2>, View>::value>>
#endif
            explicit take_view(View2 && r, int n) :
                first_(r.begin(), n), last_(r.end())
            {}

            iterator begin() const { return first_; }
            sentinel end() const { return last_; }

        private:
            iterator first_;
            sentinel last_;
        };

        struct take_impl
        {
            template<typename R>
            constexpr auto operator()(R && r, int n) const
            {
                return take_view<std::remove_reference_t<R>>((R &&) r, n);
            }

            constexpr auto operator()(int n) const
            {
                using closure_func_type =
                    decltype(boost::stl_interfaces::bind_back(*this, n));
                return boost::stl_interfaces::closure<closure_func_type>(
                    boost::stl_interfaces::bind_back(*this, n));
            }
        };

        template<
            typename R,
            bool ReverseView = is_reverse_view<std::decay_t<R>>::value>
        struct reverse_impl_impl
        {
            static constexpr auto call(R && r) { return ((R &&) r).base(); }
        };
        template<typename R>
        struct reverse_impl_impl<R, false>
        {
            static constexpr auto call(R && r)
            {
                return reverse_view<std::remove_reference_t<R>>(0, (R &&) r);
            }
        };

        struct reverse_impl
            : boost::stl_interfaces::range_adaptor_closure<reverse_impl>
        {
            template<typename R>
            constexpr auto operator()(R && r) const
            {
                return reverse_impl_impl<R>::call((R &&) r);
            }
        };
    }

#if BOOST_STL_INTERFACES_USE_CONCEPTS

namespace std::ranges {
    template<typename View>
    inline constexpr bool enable_borrowed_range<detail::take_view<View>> = true;
}

#endif

#if defined(__cpp_inline_variables)
inline constexpr detail::reverse_impl old_reverse;
#else
namespace {
    constexpr detail::reverse_impl old_reverse;
}
#endif

#if defined(__cpp_inline_variables)
inline constexpr detail::take_impl old_take;
#else
namespace {
    constexpr detail::take_impl old_take;
}
#endif

#if 201703L <= __cplusplus
inline constexpr boost::stl_interfaces::adaptor take =
    []<typename R>(R && r, int n) {
        return detail::take_view<std::remove_reference_t<R>>((R &&) r, n);
    };
#endif

int main()
{
    // non-closures
    {
        std::vector<int> vec1 = {0, 1, 2, 3, 4, 5, 6, 7};

        std::vector<int> vec2;
        for (auto x : old_all(vec1) | old_reverse) {
            vec2.push_back(x);
        }

        std::reverse(vec2.begin(), vec2.end());
        BOOST_TEST(vec1 == vec2);
    }

    {
        std::vector<int> const vec1 = {0, 1, 2, 3, 4, 5, 6, 7};

        std::vector<int> vec2;
        for (auto x : old_all(vec1) | old_reverse) {
            vec2.push_back(x);
        }

        std::reverse(vec2.begin(), vec2.end());
        BOOST_TEST(vec1 == vec2);
    }

    {
        std::vector<int> const vec1 = {0, 1, 2, 3, 4, 5, 6, 7};

        std::vector<int> vec2;
        for (auto x : old_all(vec1) | old_reverse | old_reverse) {
            vec2.push_back(x);
        }

        BOOST_TEST(vec1 == vec2);
    }

    // Mismatched begin and end; only test this in C++17 and later.
#if 201703L <= __cplusplus
    {
        std::vector<int> const vec1 = {0, 1, 2, 3, 4, 5, 6, 7};

        std::vector<int> vec2;
        for (auto x : old_all(vec1) | old_take(3)) {
            vec2.push_back(x);
        }

        BOOST_TEST(vec2 == (std::vector<int>{0, 1, 2}));
    }

    {
        std::vector<int> const vec1 = {0, 1, 2, 3, 4, 5, 6, 7};

        std::vector<int> vec2;
        for (auto x : old_all(vec1) | old_reverse | old_take(3)) {
            vec2.push_back(x);
        }

        BOOST_TEST(vec2 == (std::vector<int>{7, 6, 5}));
    }
#endif

#if 201703L <= __cplusplus
    // closures
    {
        std::vector<int> vec1 = {0, 1, 2, 3, 4, 5, 6, 7};

        std::vector<int> vec2;
        for (auto x : all(vec1) | reverse) {
            vec2.push_back(x);
        }

        std::reverse(vec2.begin(), vec2.end());
        BOOST_TEST(vec1 == vec2);
    }

    {
        std::vector<int> const vec1 = {0, 1, 2, 3, 4, 5, 6, 7};

        std::vector<int> vec2;
        for (auto x : all(vec1) | reverse) {
            vec2.push_back(x);
        }

        std::reverse(vec2.begin(), vec2.end());
        BOOST_TEST(vec1 == vec2);
    }

    {
        std::vector<int> const vec1 = {0, 1, 2, 3, 4, 5, 6, 7};

        std::vector<int> vec2;
        for (auto x : all(vec1) | reverse | reverse) {
            vec2.push_back(x);
        }

        BOOST_TEST(vec1 == vec2);
    }

    {
        std::vector<int> const vec1 = {0, 1, 2, 3, 4, 5, 6, 7};

        std::vector<int> vec2;
        for (auto x : all(vec1) | take(3)) {
            vec2.push_back(x);
        }

        BOOST_TEST(vec2 == (std::vector<int>{0, 1, 2}));
    }

    {
        std::vector<int> const vec1 = {0, 1, 2, 3, 4, 5, 6, 7};

        std::vector<int> vec2;
        for (auto x : all(vec1) | reverse | take(3)) {
            vec2.push_back(x);
        }

        BOOST_TEST(vec2 == (std::vector<int>{7, 6, 5}));
    }
#endif

    return boost::report_errors();
}
