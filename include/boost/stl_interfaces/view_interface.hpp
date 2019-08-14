// Copyright (C) 2019 T. Zachary Laine
//
// Distributed under the Boost Software License, Version 1.0. (See
// accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
#ifndef BOOST_STL_INTERFACES_VIEW_INTERFACE_HPP
#define BOOST_STL_INTERFACES_VIEW_INTERFACE_HPP

#include <boost/stl_interfaces/fwd.hpp>

#include <iterator>


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
        template<typename T>
        using range_difference_t = typename range_difference<T>::type;
    }

    /** A CRTP template that one may derive from to make it easier to define
        `std::ranges::view`-like types with a container-like interface.  This
        is a pre-C++20 version of C++20's `view_interface` (see
        [view.interface] in the C++ standard). */
    template<typename Derived, bool Contiguous = discontiguous>
    struct view_interface
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
#endif

    public:
        template<typename D = Derived>
        constexpr auto empty() noexcept(
            noexcept(std::declval<D &>().begin() == std::declval<D &>().end()))
            -> decltype(
                std::declval<D &>().begin() == std::declval<D &>().end())
        {
            return derived().begin() == derived().end();
        }
        template<typename D = Derived>
        constexpr auto empty() const noexcept(
            noexcept(std::declval<D &>().begin() == std::declval<D &>().end()))
            -> decltype(
                std::declval<D &>().begin() == std::declval<D &>().end())
        {
            return derived().begin() == derived().end();
        }

        template<
            typename D = Derived,
            typename R = decltype(std::declval<D &>().empty())>
        constexpr explicit
        operator R() noexcept(noexcept(std::declval<D &>().empty()))
        {
            return !derived().empty();
        }
        template<
            typename D = Derived,
            typename R = decltype(std::declval<D &>().empty())>
        constexpr explicit operator bool() const
            noexcept(noexcept(std::declval<D &>().empty()))
        {
            return !derived().empty();
        }

        template<
            typename D = Derived,
            bool C = Contiguous,
            typename Enable = std::enable_if_t<C>>
        constexpr auto data() noexcept(noexcept(std::declval<D &>().begin()))
            -> decltype(std::declval<D &>().begin())
        {
            return std::addressof(*derived().begin());
        }
        template<
            typename D = Derived,
            bool C = Contiguous,
            typename Enable = std::enable_if_t<C>>
        constexpr auto data() const
            noexcept(noexcept(std::declval<D &>().begin()))
                -> decltype(std::declval<D &>().begin())
        {
            return std::addressof(*derived().begin());
        }

        template<typename D = Derived>
        constexpr auto size() noexcept(
            noexcept(std::declval<D &>().end() - std::declval<D &>().begin()))
            -> decltype(std::declval<D &>().end() - std::declval<D &>().begin())
        {
            return derived().end() - derived().begin();
        }
        template<typename D = Derived>
        constexpr auto size() const noexcept(
            noexcept(std::declval<D &>().end() - std::declval<D &>().begin()))
            -> decltype(std::declval<D &>().end() - std::declval<D &>().begin())
        {
            return derived().end() - derived().begin();
        }

        template<typename D = Derived>
        constexpr auto front() noexcept(noexcept(*std::declval<D &>().begin()))
            -> decltype(*std::declval<D &>().begin())
        {
            *derived().begin();
        }
        template<typename D = Derived>
        constexpr auto front() const
            noexcept(noexcept(*std::declval<D &>().begin()))
                -> decltype(*std::declval<D &>().begin())
        {
            *derived().begin();
        }

        template<typename D = Derived>
        constexpr auto
        back() noexcept(noexcept(*std::prev(std::declval<D &>().end())))
            -> decltype(*std::prev(std::declval<D &>().end()))
        {
            *std::prev(derived().end());
        }
        template<typename D = Derived>
        constexpr auto back() const
            noexcept(noexcept(*std::prev(std::declval<D &>().end())))
                -> decltype(*std::prev(std::declval<D &>().end()))
        {
            *std::prev(derived().end());
        }

        template<typename D = Derived>
        constexpr auto operator[](detail::range_difference_t<D> n) noexcept(
            noexcept(std::declval<D &>().begin()[n]))
            -> decltype(std::declval<D &>().begin()[n])
        {
            return derived().begin()[n];
        }
        template<typename D = Derived>
        constexpr auto operator[](detail::range_difference_t<D> n) const
            noexcept(noexcept(std::declval<D &>().begin()[n]))
                -> decltype(std::declval<D &>().begin()[n])
        {
            return derived().begin()[n];
        }
    };

}}

#endif
