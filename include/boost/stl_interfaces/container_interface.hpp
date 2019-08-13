// Copyright (C) 2019 T. Zachary Laine
//
// Distributed under the Boost Software License, Version 1.0. (See
// accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
#ifndef BOOST_STL_INTERFACES_CONTAINER_INTERFACE_HPP
#define BOOST_STL_INTERFACES_CONTAINER_INTERFACE_HPP

#include <boost/stl_interfaces/fwd.hpp>


namespace boost { namespace stl_interfaces {

    namespace detail {
        template<typename T, typename = void>
        struct range_difference;
        template<typename T>
        struct range_difference<
            T,
            void_t<typename std::iterator_traits<std::decay_t<decltype(
                std::begin(std::declval<T>()))>>::difference_type>>
        {
            using type = typename std::iterator_traits<std::decay_t<decltype(
                std::begin(std::declval<T>()))>>::difference_type;
        };
        using range_difference_t = typename range_difference<T>::type;
    }

    // Container requirements:
    // begin/end -> cbegin/cend
    // c1 == c2 -> c1 != c2
    // random access begin/end -> size,empty

    // Reversible container requirements:
    // begin/end -> rbegin/rend (requires detection of proxies for pre-C++20 code, because reverse_iterator does not handle proxies properly before that)
    // rbegin/rend -> crbegin/crend

    // Sequence container requirements:
    // C(il) -> c = il
    // emplace -> insert(p,t),insert(p,rv),insert(p,n,t),insert(p,i,j),insert(p,il)
    // clear,emplace -> assign(i,j),assign(il),assign(n,t)
    // erase(q) -> erase(q1,q2)
    // emplace_front -> push_front(t),push_front(rv),pop_front
    // emplace_back -> push_back(t),push_back(rv),pop_back
    // random access begin/end -> operator[]
    // operator[] -> at

    // Associative container requirements (only those not covered above):
    // try_emplace -> all unique emplaces and inserts, except emplace_hint, hinted insert, and node handle inserts
    // emplace (in absence of try_emplace) -> all non-unique emplaces and inserts, except emplace_hint, hinted insert, and node handle inserts
    // emplace_hint -> hinted insert
    // lower_bound,upper_bound -> equal_range,find(tran?),count(tran?),contains(tran?)
    // erase(q),equal_range -> erase(k),erase(r),erase(q1,q2)

    /** A CRTP template that one may derive from to make it easier to define
        container types. */
    template<typename Derived, typename Contiguity = discontiguous_data_tag>
    struct container_interface
    {
#ifndef BOOST_STL_INTERFACES_DOXYGEN
    private:
        constexpr Derived & derived() noexcept
        {
            return static_cast<Derived &>(*this);
        }
        constexpr const Derived & derived() const noexcept
        {
            return static_cast<Derived const &>(*this);
        }
        constexpr static bool contiguous =
            std::is_same<Contiguity, contiguous_data_tag>::value;
#endif

    public:
        template<typename D = Derived>
        constexpr bool empty() const noexcept(
            noexcept(std::declval<D &>().begin() == std::declval<D &>().end()))
            ->decltype(std::declval<D &>().begin() == std::declval<D &>().end())
        {
            return derived().begin() == derived().end();
        }

        template<
            typename D = Derived,
            typename Enable = std::enable_if_t<contiguous>>
        constexpr auto data() noexcept(noexcept(std::declval<D &>().begin()))
            -> decltype(std::addressof(std::declval<D &>().begin()))
        {
            return std::to_address(derived().begin());
        }
        template<
            typename D = Derived,
            typename Enable = std::enable_if_t<contiguous>>
        constexpr auto data() const
            noexcept(noexcept(std::declval<D &>().begin()))
                -> decltype(std::declval<D &>().begin())
        {
            return std::to_address(derived().begin());
        }

        template<typename D = Derived>
        constexpr auto size() const noexcept(
            noexcept(std::declval<D &>().end() - std::declval<D &>().begin()))
            -> decltype(std::declval<D &>().end() - std::declval<D &>().begin())
        {
            return derived().end() - derived().begin();
        }

        template<typename D = Derived>
        constexpr decltype(auto)
        front() noexcept(noexcept(*std::declval<D &>().begin()))
            -> decltype(*std::declval<D &>().begin())
        {
            *derived().begin();
        }
        template<typename D = Derived>
        constexpr decltype(auto) front() const
            noexcept(noexcept(*std::declval<D &>().begin()))
                -> decltype(*std::declval<D &>().begin())
        {
            *derived().begin();
        }

        template<typename D = Derived>
        constexpr decltype(auto)
        back() noexcept(noexcept(*std::prev(std::declval<D &>().end())))
            -> decltype(*std::prev(std::declval<D &>().end()))
        {
            *std::prev(derived().end());
        }
        template<typename D = Derived>
        constexpr decltype(auto) back() const
            noexcept(noexcept(*std::prev(std::declval<D &>().end())))
                -> decltype(*std::prev(std::declval<D &>().end()))
        {
            *std::prev(derived().end());
        }

        template<typename D = Derived>
        constexpr decltype(auto)
        operator[](deatil::range_difference_t<D> n) noexcept(
            noexcept(std::declval<D &>().begin()[n]))
            -> decltype(std::declval<D &>().begin()[n])
        {
            return derived().begin()[n];
        }
        template<typename D = Derived>
        constexpr decltype(auto)
        operator[](detail::range_difference_t<D> n) const
            noexcept(noexcept(std::declval<D &>().begin()[n]))
                -> decltype(std::declval<D &>().begin()[n])
        {
            return derived().begin()[n];
        }
    };

}}

#endif
