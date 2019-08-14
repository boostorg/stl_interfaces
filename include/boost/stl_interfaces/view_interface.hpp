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
        template<typename Iter>
        using iter_difference_t =
            typename std::iterator_traits<Iter>::difference_type;

        template<typename Range, typename = void>
        struct iterator;
        template<typename Range>
        struct iterator<
            Range,
            void_t<decltype(std::declval<Range &>().begin())>>
        {
            using type = decltype(std::declval<Range &>().begin());
        };
        template<typename Range>
        using iterator_t = typename iterator<Range>::type;

        template<typename Range, typename = void>
        struct sentinel;
        template<typename Range>
        struct sentinel<
            Range,
            void_t<decltype(std::declval<Range &>().begin())>>
        {
            using type = decltype(std::declval<Range &>().begin());
        };
        template<typename Range>
        using sentinel_t = typename sentinel<Range>::type;

        template<typename Range>
        using range_difference_t = iter_difference_t<iterator_t<Range>>;

        template<typename Range>
        using common_range = std::is_same<iterator_t<Range>, sentinel_t<Range>>;
    }

    /** A CRTP template that one may derive from to make it easier to define
        `std::ranges::view`-like types with a container-like interface.  This
        is a pre-C++20 version of C++20's `view_interface` (see
        [view.interface] in the C++ standard). */
    template<
        typename Derived,
        bool Contiguous = discontiguous,
        typename E = std::enable_if_t<
            std::is_class<Derived>::value &&
            std::is_same<Derived, std::remove_cv_t<Derived>>::value>>
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
        using derived_view_type = Derived;

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

        template<
            typename D = Derived,
            typename Enable = std::enable_if_t<detail::common_range<D>::value>>
        constexpr auto
        back() noexcept(noexcept(*std::prev(std::declval<D &>().end())))
            -> decltype(*std::prev(std::declval<D &>().end()))
        {
            return *std::prev(derived().end());
        }
        template<
            typename D = Derived,
            typename Enable = std::enable_if_t<detail::common_range<D>::value>>
        constexpr auto back() const
            noexcept(noexcept(*std::prev(std::declval<D &>().end())))
                -> decltype(*std::prev(std::declval<D &>().end()))
        {
            return *std::prev(derived().end());
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

    /** Implementation of `operator!=()` for all views derived from
        `view_interface`.  */
    template<typename ViewInterface>
    constexpr auto operator!=(ViewInterface lhs, ViewInterface rhs) noexcept(
        noexcept(lhs == rhs))
        -> decltype(
            detail::dummy<typename ViewInterface::derived_view_type>(),
            lhs == rhs)
    {
        return !(lhs == rhs);
    }

}}

#endif
