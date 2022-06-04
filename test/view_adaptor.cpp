// Copyright (C) 2022 T. Zachary Laine
//
// Distributed under the Boost Software License, Version 1.0. (See
// accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
#include <boost/stl_interfaces/reverse_iterator.hpp>
#include <boost/stl_interfaces/view_interface.hpp>
#include <boost/stl_interfaces/view_adaptor.hpp>

#include "ill_formed.hpp"

#include <boost/core/lightweight_test.hpp>

#include <algorithm>
#include <vector>


    namespace detail {
#if BOOST_STL_INTERFACES_USE_CONCEPTS
        template<typename T>
        using iterator_t = std::ranges::iterator_t<T>;
        template<typename T>
        using sentinel_t = std::ranges::sentinel_t<T>;
#else
        template<typename T>
        using iterator_t = decltype(std::declval<T &>().begin());
        template<typename T>
        using sentinel_t = decltype(std::declval<T &>().end());
#endif

#if BOOST_STL_INTERFACES_USE_CONCEPTS
        template<typename R>
        requires std::is_object_v<R>
#else
        template<
            typename R,
            typename Enable = std::enable_if_t<std::is_object<R>::value>>
#endif
        struct all_view : boost::stl_interfaces::view_interface<all_view<R>>
        {
            using iterator = iterator_t<R>;
            using sentinel = sentinel_t<R>;

#if BOOST_STL_INTERFACES_USE_CONCEPTS
            template<typename R2>
            requires std::is_same_v<std::remove_reference_t<R2>, R>
#else
            template<
                typename R2,
                typename E = std::enable_if_t<
                    std::is_same<std::remove_reference_t<R2>, R>::value>>
#endif
            explicit all_view(int, R2 && r) : first_(r.begin()), last_(r.end())
            {}

            iterator begin() const { return first_; }
            sentinel end() const { return last_; }

        private:
            iterator first_;
            sentinel last_;
        };

        struct all_impl : boost::stl_interfaces::range_adaptor_closure<all_impl>
        {
            template<typename R>
            constexpr auto operator()(R && r) const
            {
                return all_view<std::remove_reference_t<R>>(0, (R &&) r);
            }
        };

#if BOOST_STL_INTERFACES_USE_CONCEPTS
        template<typename R>
        requires std::is_object_v<R>
#else
        template<
            typename R,
            typename Enable = std::enable_if_t<std::is_object<R>::value>>
#endif
        struct take_view : boost::stl_interfaces::view_interface<take_view<R>>
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

            using iterator = counted_iterator<iterator_t<R>>;
            using sentinel = counted_sentinel<sentinel_t<R>>;

#if BOOST_STL_INTERFACES_USE_CONCEPTS
            template<typename R2>
            requires std::is_same_v<std::remove_reference_t<R2>, R>
#else
            template<
                typename R2,
                typename E = std::enable_if_t<
                    std::is_same<std::remove_reference_t<R2>, R>::value>>
#endif
            explicit take_view(R2 && r, int n) :
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

        template<bool CommonRange>
        struct set_rev_rng_first
        {
            template<typename V>
            static auto call(V const & v)
            {
                return boost::stl_interfaces::make_reverse_iterator(v.end());
            }
        };

        template<>
        struct set_rev_rng_first<false>
        {
            template<typename V>
            static auto call(V const & v)
            {
                auto v_f = v.begin();
                auto const v_l = v.end();
                while (v_f != v_l) {
                    ++v_f;
                }
                return boost::stl_interfaces::make_reverse_iterator(v_f);
            }
        };

#if BOOST_STL_INTERFACES_USE_CONCEPTS
        template<typename View>
        requires std::is_object_v<View>
#else
        template<
            typename View,
            typename Enable = std::enable_if_t<std::is_object<View>::value>>
#endif
        struct reverse_view : boost::stl_interfaces::view_interface<reverse_view<View>>
        {
            using v_iter = iterator_t<View>;
            using v_sent = sentinel_t<View>;

            static_assert(
                std::is_base_of<
                    std::bidirectional_iterator_tag,
                    typename std::iterator_traits<v_iter>::iterator_category>::
                    value,
                "A reversed view must have bidirectional iterators.");

            using iterator = boost::stl_interfaces::reverse_iterator<v_iter>;

            constexpr reverse_view() = default;

#if BOOST_STL_INTERFACES_USE_CONCEPTS
            template<typename V>
            requires std::is_same_v<std::remove_reference_t<V>, View>
#else
            template<
                typename V,
                typename E = std::enable_if_t<
                    std::is_same<std::remove_reference_t<V>, View>::value>>
#endif
            constexpr reverse_view(int, V && v) : v_{(V &&) v}
            {
                first_ = set_rev_rng_first<
                    std::is_same<v_iter, v_sent>::value>::call(v_);
            }

            constexpr iterator begin() const { return first_; }
            constexpr iterator end() const
            {
                return boost::stl_interfaces::make_reverse_iterator(v_.begin());
            }

            constexpr View base() const { return v_; }

        private:
            View v_ = View();
            iterator first_;
        };

        template<typename T>
        struct is_reverse_view : std::false_type
        {};
        template<typename T>
        struct is_reverse_view<reverse_view<T>> : std::true_type
        {};

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
    inline constexpr bool enable_borrowed_range<detail::all_view<View>> = true;

    template<typename View>
    inline constexpr bool enable_borrowed_range<detail::reverse_view<View>> =
        true;
}

#endif

#if defined(__cpp_inline_variables)
/** A simplified version of the `std::views::all` range adaptor for
    pre-C++20 builds.  Prefer `std::views::all` if you have it. */
inline constexpr detail::all_impl all;
#else
namespace {
    constexpr detail::all_impl all;
}
#endif

#if defined(__cpp_inline_variables)
/** A simplified version of the `std::views::reverse` range adaptor for
    pre-C++20 builds.  Prefer `std::views::reverse` if you have it. */
inline constexpr detail::reverse_impl reverse;
#else
namespace {
    constexpr detail::reverse_impl reverse;
}
#endif

#if defined(__cpp_inline_variables)
/** A simplified version of the `std::views::take` range adaptor for
    pre-C++20 builds.  Prefer `std::views::take` if you have it. */
inline constexpr detail::take_impl take;
#else
namespace {
    constexpr detail::take_impl take;
}
#endif

#if 201703L <= __cplusplus
inline constexpr boost::stl_interfaces::closure all2 = []<typename R>(R && r) {
    return detail::all_view<std::remove_reference_t<R>>(0, (R &&) r);
};

inline constexpr boost::stl_interfaces::closure reverse2 =
    []<typename R>(R && r) {
        return detail::reverse_view<std::remove_reference_t<R>>(0, (R &&) r);
    };

inline constexpr boost::stl_interfaces::adaptor take2 =
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

    // Mismatched begin and end; only test this in C++17 and later.
#if 201703L <= __cplusplus
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

#if 201703L <= __cplusplus
    // closures
    {
        std::vector<int> vec1 = {0, 1, 2, 3, 4, 5, 6, 7};

        std::vector<int> vec2;
        for (auto x : all2(vec1) | reverse2) {
            vec2.push_back(x);
        }

        std::reverse(vec2.begin(), vec2.end());
        BOOST_TEST(vec1 == vec2);
    }

    {
        std::vector<int> const vec1 = {0, 1, 2, 3, 4, 5, 6, 7};

        std::vector<int> vec2;
        for (auto x : all2(vec1) | reverse2) {
            vec2.push_back(x);
        }

        std::reverse(vec2.begin(), vec2.end());
        BOOST_TEST(vec1 == vec2);
    }

    {
        std::vector<int> const vec1 = {0, 1, 2, 3, 4, 5, 6, 7};

        std::vector<int> vec2;
        for (auto x : all2(vec1) | reverse2 | reverse2) {
            vec2.push_back(x);
        }

        BOOST_TEST(vec1 == vec2);
    }

    {
        std::vector<int> const vec1 = {0, 1, 2, 3, 4, 5, 6, 7};

        std::vector<int> vec2;
        for (auto x : all2(vec1) | take2(3)) {
            vec2.push_back(x);
        }

        BOOST_TEST(vec2 == (std::vector<int>{0, 1, 2}));
    }

    {
        std::vector<int> const vec1 = {0, 1, 2, 3, 4, 5, 6, 7};

        std::vector<int> vec2;
        for (auto x : all2(vec1) | reverse2 | take2(3)) {
            vec2.push_back(x);
        }

        BOOST_TEST(vec2 == (std::vector<int>{7, 6, 5}));
    }
#endif

    return boost::report_errors();
}
