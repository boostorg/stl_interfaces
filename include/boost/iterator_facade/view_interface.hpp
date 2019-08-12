// Copyright (C) 2019 T. Zachary Laine
//
// Distributed under the Boost Software License, Version 1.0. (See
// accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
#ifndef BOOST_ITERATOR_FACADE_VIEW_INTERFACE_HPP
#define BOOST_ITERATOR_FACADE_VIEW_INTERFACE_HPP

#include <boost/iterator_facade/fwd.hpp>


namespace boost { namespace iterator_facade {

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

    /** A CRTP template that one may derive from to make it easier to define
        `std::ranges::view`-like types with a container-like interface.  This
        is a pre-C++20 version of C++20's `view_interface` (see
        [view.interface] in the C++ standard). */
    template<typename Derived, typename Contiguity = discontiguous_data_tag>
    struct view_interface
    {
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
        constexpr bool empty() const noexcept(
            noexcept(std::declval<D &>().begin() == std::declval<D &>().end()))
            ->decltype(std::declval<D &>().begin() == std::declval<D &>().end())
        {
            return derived().begin() == derived().end();
        }

        template<typename D = Derived>
        constexpr explicit
        operator bool() noexcept(noexcept(std::declval<D &>().empty()))
            ->decltype(std::declval<D &>().empty())
        {
            return !derived().empty();
        }
        template<typename D = Derived>
        constexpr explicit operator bool() const
            noexcept(noexcept(std::declval<D &>().empty()))
                ->decltype(std::declval<D &>().empty())
        {
            return !derived().empty();
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
        constexpr auto size() noexcept(
            noexcept(std::declval<D &>().end() - std::declval<D &>().begin()))
            -> decltype(std::declval<D &>().end() - std::declval<D &>().begin())
        {
            return derived().end() - derived().begin()
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

#if 0 // From C++20, [view.interface]:

  template<class D>
    requires is_class_v<D> && same_as<D, remove_cv_t<D>>
  class view_interface : public view_base {
  private:
    constexpr D& derived() noexcept {
      return static_cast<D&>(*this);
    }
    constexpr const D& derived() const noexcept {
      return static_cast<const D&>(*this);
    }
  public:
    constexpr bool empty() requires forward_range<D> {
      return ranges::begin(derived()) == ranges::end(derived());
    }
    constexpr bool empty() const requires forward_range<const D> {
      return ranges::begin(derived()) == ranges::end(derived());
    }

    constexpr explicit operator bool()
      requires requires { ranges::empty(derived()); } {
        return !ranges::empty(derived());
      }
    constexpr explicit operator bool() const
      requires requires { ranges::empty(derived()); } {
        return !ranges::empty(derived());
      }

    constexpr auto data() requires contiguous_iterator<iterator_t<D>> {
      return to_address(ranges::begin(derived()));
    }
    constexpr auto data() const
      requires range<const D> && contiguous_iterator<iterator_t<const D>> {
        return to_address(ranges::begin(derived()));
      }

    constexpr auto size() requires forward_range<D> &&
      sized_sentinel_for<sentinel_t<D>, iterator_t<D>> {
        return ranges::end(derived()) - ranges::begin(derived());
      }
    constexpr auto size() const requires forward_range<const D> &&
      sized_sentinel_for<sentinel_t<const D>, iterator_t<const D>> {
        return ranges::end(derived()) - ranges::begin(derived());
      }

    constexpr decltype(auto) front() requires forward_range<D>;
    constexpr decltype(auto) front() const requires forward_range<const D>;

    constexpr decltype(auto) back() requires bidirectional_range<D> && common_range<D>;
    constexpr decltype(auto) back() const
      requires bidirectional_range<const D> && common_range<const D>;

    template<random_access_range R = D>
      constexpr decltype(auto) operator[](range_difference_t<R> n) {
        return ranges::begin(derived())[n];
      }
    template<random_access_range R = const D>
      constexpr decltype(auto) operator[](range_difference_t<R> n) const {
        return ranges::begin(derived())[n];
      }
  };

#endif

}}

#endif
