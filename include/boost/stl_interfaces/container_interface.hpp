// Copyright (C) 2019 T. Zachary Laine
//
// Distributed under the Boost Software License, Version 1.0. (See
// accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
#ifndef BOOST_STL_INTERFACES_CONTAINER_INTERFACE_HPP
#define BOOST_STL_INTERFACES_CONTAINER_INTERFACE_HPP

#include <boost/stl_interfaces/reverse_iterator.hpp>

#include <boost/assert.hpp>

#include <stdexcept>


namespace boost { namespace stl_interfaces { namespace detail {

    template<typename T, typename SizeType>
    struct n_iter : iterator_interface<
                        n_iter<T, SizeType>,
                        std::random_access_iterator_tag,
                        T>
    {
        n_iter() : x_(nullptr), n_(0) {}
        n_iter(T const & x, SizeType n) : x_(&x), n_(n) {}

        constexpr std::ptrdiff_t operator-(n_iter other) const noexcept
        {
            return std::ptrdiff_t(n_) - std::ptrdiff_t(other.n_);
        }
        n_iter & operator+=(std::ptrdiff_t offset)
        {
            n_ += offset;
            return *this;
        }

    private:
        friend access;
        constexpr T const *& base_reference() noexcept { return x_; }
        constexpr T const * base_reference() const noexcept { return x_; }

        T const * x_;
        SizeType n_;
    };

    template<typename T, typename SizeType>
    constexpr auto make_n_iter(T const & x, SizeType n) noexcept(
        noexcept(n_iter<T, SizeType>(x, n)))
    {
        using result_type = n_iter<T, SizeType>;
        return result_type(x, SizeType(0));
    }
    template<typename T, typename SizeType>
    constexpr auto make_n_iter_end(T const & x, SizeType n) noexcept(
        noexcept(n_iter<T, SizeType>(x, n)))
    {
        return n_iter<T, SizeType>(x, n);
    }

}}}

namespace boost { namespace stl_interfaces { inline namespace v1 {

    /** A CRTP template that one may derive from to make it easier to define
        container types. */
    template<
        typename Derived,
        bool Contiguous = discontiguous
#ifndef BOOST_STL_INTERFACES_DOXYGEN
        ,
        typename E = std::enable_if_t<
            std::is_class<Derived>::value &&
            std::is_same<Derived, std::remove_cv_t<Derived>>::value>
#endif
        >
    struct container_interface;

    namespace v1_dtl {
        template<typename Iter>
        using in_iter = std::is_convertible<
            typename std::iterator_traits<Iter>::iterator_category,
            std::input_iterator_tag>;

        template<typename Derived, typename = void>
        struct clear_impl
        {
            static constexpr void call(Derived & d) noexcept {}
        };
        template<typename Derived>
        struct clear_impl<
            Derived,
            void_t<decltype(std::declval<Derived>().clear())>>
        {
            static constexpr void call(Derived & d) noexcept { d.clear(); }
        };

        template<typename Derived, bool Contiguous>
        void
        derived_container(container_interface<Derived, Contiguous> const &);
    }

    template<
        typename Derived,
        bool Contiguous
#ifndef BOOST_STL_INTERFACES_DOXYGEN
        ,
        typename E
#endif
        >
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
        constexpr Derived & mutable_derived() const noexcept
        {
            return const_cast<Derived &>(static_cast<Derived const &>(*this));
        }
#endif

    public:
        ~container_interface() { v1_dtl::clear_impl<Derived>::call(derived()); }

        template<typename D = Derived>
        constexpr auto empty() noexcept(
            noexcept(std::declval<D &>().begin() == std::declval<D &>().end()))
            -> decltype(
                std::declval<D &>().begin() == std::declval<D &>().end())
        {
            return derived().begin() == derived().end();
        }
        template<typename D = Derived>
        constexpr auto empty() const noexcept(noexcept(
            std::declval<D const &>().begin() ==
            std::declval<D const &>().end()))
            -> decltype(
                std::declval<D const &>().begin() ==
                std::declval<D const &>().end())
        {
            return derived().begin() == derived().end();
        }

        template<
            typename D = Derived,
            bool C = Contiguous,
            typename Enable = std::enable_if_t<C>>
        constexpr auto data() noexcept(noexcept(std::declval<D &>().begin()))
            -> decltype(std::addressof(*std::declval<D &>().begin()))
        {
            return std::addressof(*derived().begin());
        }
        template<
            typename D = Derived,
            bool C = Contiguous,
            typename Enable = std::enable_if_t<C>>
        constexpr auto data() const
            noexcept(noexcept(std::declval<D const &>().begin()))
                -> decltype(std::addressof(*std::declval<D const &>().begin()))
        {
            return std::addressof(*derived().begin());
        }

        template<typename D = Derived>
        constexpr auto size() noexcept(
            noexcept(std::declval<D &>().end() - std::declval<D &>().begin()))
            -> decltype(typename D::size_type(
                std::declval<D &>().end() - std::declval<D &>().begin()))
        {
            return derived().end() - derived().begin();
        }
        template<typename D = Derived>
        constexpr auto size() const noexcept(noexcept(
            std::declval<D const &>().end() -
            std::declval<D const &>().begin()))
            -> decltype(typename D::size_type(
                std::declval<D const &>().end() -
                std::declval<D const &>().begin()))
        {
            return derived().end() - derived().begin();
        }

        template<typename D = Derived>
        constexpr auto front() noexcept(noexcept(*std::declval<D &>().begin()))
            -> decltype(*std::declval<D &>().begin())
        {
            return *derived().begin();
        }
        template<typename D = Derived>
        constexpr auto front() const
            noexcept(noexcept(*std::declval<D const &>().begin()))
                -> decltype(*std::declval<D const &>().begin())
        {
            return *derived().begin();
        }

        template<typename D = Derived>
        constexpr auto push_front(typename D::value_type const & x) noexcept(
            noexcept(std::declval<D &>().emplace_front(x)))
            -> decltype((void)std::declval<D &>().emplace_front(x))
        {
            derived().emplace_front(x);
        }

        template<typename D = Derived>
        constexpr auto push_front(typename D::value_type && x) noexcept(
            noexcept(std::declval<D &>().emplace_front(std::move(x))))
            -> decltype((void)std::declval<D &>().emplace_front(std::move(x)))
        {
            derived().emplace_front(std::move(x));
        }

        template<typename D = Derived>
        constexpr auto pop_front() noexcept -> decltype(
            std::declval<D &>().emplace_front(
                std::declval<typename D::value_type &>()),
            (void)std::declval<D &>().erase(std::declval<D &>().begin()))
        {
            derived().erase(derived().begin());
        }

        template<
            typename D = Derived,
            typename Enable = std::enable_if_t<
                v1_dtl::decrementable_sentinel<D>::value &&
                v1_dtl::common_range<D>::value>>
        constexpr auto
        back() noexcept(noexcept(*std::prev(std::declval<D &>().end())))
            -> decltype(*std::prev(std::declval<D &>().end()))
        {
            return *std::prev(derived().end());
        }
        template<
            typename D = Derived,
            typename Enable = std::enable_if_t<
                v1_dtl::decrementable_sentinel<D>::value &&
                v1_dtl::common_range<D>::value>>
        constexpr auto back() const
            noexcept(noexcept(*std::prev(std::declval<D const &>().end())))
                -> decltype(*std::prev(std::declval<D const &>().end()))
        {
            return *std::prev(derived().end());
        }

        template<typename D = Derived>
        constexpr auto push_back(typename D::value_type const & x) noexcept(
            noexcept(std::declval<D &>().emplace_back(x)))
            -> decltype((void)std::declval<D &>().emplace_back(x))
        {
            derived().emplace_back(x);
        }

        template<typename D = Derived>
        constexpr auto push_back(typename D::value_type && x) noexcept(
            noexcept(std::declval<D &>().emplace_back(std::move(x))))
            -> decltype((void)std::declval<D &>().emplace_back(std::move(x)))
        {
            derived().emplace_back(std::move(x));
        }

        template<typename D = Derived>
        constexpr auto pop_back() noexcept -> decltype(
            std::declval<D &>().emplace_back(
                std::declval<typename D::value_type &>()),
            (void)std::declval<D &>().erase(
                std::prev(std::declval<D &>().end())))
        {
            derived().erase(std::prev(derived().end()));
        }

        template<typename D = Derived>
        constexpr auto operator[](typename D::size_type n) noexcept(
            noexcept(std::declval<D &>().begin()[n]))
            -> decltype(std::declval<D &>().begin()[n])
        {
            return derived().begin()[n];
        }
        template<typename D = Derived>
        constexpr auto operator[](typename D::size_type n) const
            noexcept(noexcept(std::declval<D const &>().begin()[n]))
                -> decltype(std::declval<D const &>().begin()[n])
        {
            return derived().begin()[n];
        }

        template<typename D = Derived>
        constexpr auto at(typename D::size_type i)
            -> decltype(std::declval<D &>().size(), std::declval<D &>()[i])
        {
            if (derived().size() <= i) {
                throw std::out_of_range(
                    "Bounds check failed in container_interface::at()");
            }
            return derived()[i];
        }

        template<typename D = Derived>
        constexpr auto at(typename D::size_type i) const -> decltype(
            std::declval<D const &>().size(), std::declval<D const &>()[i])
        {
            if (derived().size() <= i) {
                throw std::out_of_range(
                    "Bounds check failed in container_interface::at()");
            }
            return derived()[i];
        }

        template<typename D = Derived>
        constexpr auto resize(typename D::size_type n) noexcept(
            noexcept(std::declval<D &>().resize(
                n, std::declval<typename D::value_type const &>())))
            -> decltype(std::declval<D &>().resize(
                n, std::declval<typename D::value_type const &>()))
        {
            return derived().resize(n, typename D::value_type());
        }

        template<typename D = Derived>
        constexpr auto begin() const
            noexcept(noexcept(std::declval<D &>().begin()))
        {
            return typename D::const_iterator(mutable_derived().begin());
        }
        template<typename D = Derived>
        constexpr auto end() const noexcept(noexcept(std::declval<D &>().end()))
        {
            return typename D::const_iterator(mutable_derived().end());
        }

        template<typename D = Derived>
        constexpr auto cbegin() const
            noexcept(noexcept(std::declval<D const &>().begin()))
                -> decltype(std::declval<D const &>().begin())
        {
            return derived().begin();
        }
        template<typename D = Derived>
        constexpr auto cend() const
            noexcept(noexcept(std::declval<D const &>().end()))
                -> decltype(std::declval<D const &>().end())
        {
            return derived().end();
        }

        template<
            typename D = Derived,
            typename Enable = std::enable_if_t<v1_dtl::common_range<D>::value>>
        constexpr auto rbegin() noexcept(noexcept(
            stl_interfaces::make_reverse_iterator(std::declval<D &>().end())))
        {
            return stl_interfaces::make_reverse_iterator(derived().end());
        }
        template<
            typename D = Derived,
            typename Enable = std::enable_if_t<v1_dtl::common_range<D>::value>>
        constexpr auto rend() noexcept(noexcept(
            stl_interfaces::make_reverse_iterator(std::declval<D &>().begin())))
        {
            return stl_interfaces::make_reverse_iterator(derived().begin());
        }

        template<typename D = Derived>
        constexpr auto rbegin() const
            noexcept(noexcept(std::declval<D &>().rbegin()))
        {
            return
                typename D::const_reverse_iterator(mutable_derived().rbegin());
        }
        template<typename D = Derived>
        constexpr auto rend() const
            noexcept(noexcept(std::declval<D &>().rend()))
        {
            return typename D::const_reverse_iterator(mutable_derived().rend());
        }

        template<typename D = Derived>
        constexpr auto crbegin() const
            noexcept(noexcept(std::declval<D const &>().rbegin()))
                -> decltype(std::declval<D const &>().rbegin())
        {
            return derived().rbegin();
        }
        template<typename D = Derived>
        constexpr auto crend() const
            noexcept(noexcept(std::declval<D const &>().rend()))
                -> decltype(std::declval<D const &>().rend())
        {
            return derived().rend();
        }

        template<typename D = Derived>
        constexpr auto insert(
            typename D::const_iterator pos,
            typename D::value_type const &
                x) noexcept(noexcept(std::declval<D &>().emplace(pos, x)))
            -> decltype(std::declval<D &>().emplace(pos, x))
        {
            return derived().emplace(pos, x);
        }

        template<typename D = Derived>
        constexpr auto insert(
            typename D::const_iterator pos,
            typename D::value_type &&
                x) noexcept(noexcept(std::declval<D &>()
                                         .emplace(pos, std::move(x))))
            -> decltype(std::declval<D &>().emplace(pos, std::move(x)))
        {
            return derived().emplace(pos, std::move(x));
        }

        template<typename D = Derived>
        constexpr auto insert(
            typename D::const_iterator pos,
            typename D::size_type n,
            typename D::value_type const & x)
            // If you see an error in this noexcept() expression, that's
            // because this function is not properly constrained.  In other
            // words, Derived does not have a "range" insert like
            // insert(position, first, last).  If that is the case, this
            // function should be removed via SFINAE from overload resolution.
            // However, both the trailing decltype code below and a
            // std::enable_if in the template parameters do not work.  Sorry
            // about that.  See below for details.
            noexcept(noexcept(std::declval<D &>().insert(
                pos, detail::make_n_iter(x, n), detail::make_n_iter_end(x, n))))
        // This causes the compiler to infinitely recurse into this function's
        // declaration, even though the call below does not match the
        // signature of this function.
#if 0
            -> decltype(std::declval<D &>().insert(
                pos, detail::make_n_iter(x, n), detail::make_n_iter_end(x, n)))
#endif
        {
            return derived().insert(
                pos, detail::make_n_iter(x, n), detail::make_n_iter_end(x, n));
        }

        template<typename D = Derived>
        constexpr auto insert(
            typename D::const_iterator pos,
            std::initializer_list<typename D::value_type>
                il) noexcept(noexcept(std::declval<D &>()
                                          .insert(pos, il.begin(), il.end())))
            -> decltype(std::declval<D &>().insert(pos, il.begin(), il.end()))
        {
            return derived().insert(pos, il.begin(), il.end());
        }

        template<typename D = Derived>
        constexpr auto erase(typename D::const_iterator pos) noexcept
            -> decltype(std::declval<D &>().erase(pos, std::next(pos)))
        {
            return derived().erase(pos, std::next(pos));
        }

        template<
            typename InputIterator,
            typename D = Derived,
            typename Enable =
                std::enable_if_t<v1_dtl::in_iter<InputIterator>::value>>
        constexpr auto assign(InputIterator first, InputIterator last) noexcept(
            noexcept(std::declval<D &>().insert(
                std::declval<D &>().begin(), first, last)))
            -> decltype(
                std::declval<D &>().clear(),
                (void)std::declval<D &>().insert(
                    std::declval<D &>().begin(), first, last))
        {
            derived().clear();
            derived().insert(derived().begin(), first, last);
        }

        template<typename D = Derived>
        constexpr auto assign(
            typename D::size_type n,
            typename D::value_type const &
                x) noexcept(noexcept(std::declval<D &>()
                                         .insert(
                                             std::declval<D &>().begin(),
                                             detail::make_n_iter(x, n),
                                             detail::make_n_iter_end(x, n))))
            -> decltype(
                std::declval<D &>().clear(),
                (void)std::declval<D &>().insert(
                    std::declval<D &>().begin(),
                    detail::make_n_iter(x, n),
                    detail::make_n_iter_end(x, n)))
        {
            derived().clear();
            derived().insert(
                derived().begin(),
                detail::make_n_iter(x, n),
                detail::make_n_iter_end(x, n));
        }

        template<typename D = Derived>
        constexpr auto
        assign(std::initializer_list<typename D::value_type> il) noexcept(
            noexcept(std::declval<D &>().assign(il.begin(), il.end())))
            -> decltype((void)std::declval<D &>().assign(il.begin(), il.end()))
        {
            derived().assign(il.begin(), il.end());
        }

        template<typename D = Derived>
        constexpr auto
        operator=(std::initializer_list<typename D::value_type> il) noexcept(
            noexcept(std::declval<D &>().assign(il.begin(), il.end())))
            -> decltype(
                std::declval<D &>().assign(il.begin(), il.end()),
                std::declval<D &>())
        {
            derived().assign(il.begin(), il.end());
            return *this;
        }

        template<typename D = Derived>
        constexpr auto clear() noexcept
            -> decltype((void)std::declval<D &>().erase(
                std::declval<D &>().begin(), std::declval<D &>().end()))
        {
            derived().erase(derived().begin(), derived().end());
        }
    };

    /** Implementation of free function `swap()` for all containers derived
        from `container_interface`.  */
    template<typename ContainerInterface>
    constexpr auto swap(
        ContainerInterface & lhs,
        ContainerInterface & rhs) noexcept(noexcept(lhs.swap(rhs)))
        -> decltype(v1_dtl::derived_container(lhs), lhs.swap(rhs))
    {
        return lhs.swap(rhs);
    }

    /** Implementation of `operator==()` for all containers derived from
        `container_interface`.  */
    template<typename ContainerInterface>
    constexpr auto
    operator==(ContainerInterface const & lhs, ContainerInterface const & rhs) noexcept(
        noexcept(lhs.size() == rhs.size()) &&
        noexcept(*lhs.begin() == *rhs.begin()))
        -> decltype(
            v1_dtl::derived_container(lhs),
            lhs.size() == rhs.size(),
            *lhs.begin() == *rhs.begin(),
            true)
    {
        return lhs.size() == rhs.size() &&
               std::equal(lhs.begin(), lhs.end(), rhs.begin(), rhs.end());
    }

    /** Implementation of `operator!=()` for all containers derived from
        `container_interface`.  */
    template<typename ContainerInterface>
    constexpr auto operator!=(
        ContainerInterface const & lhs,
        ContainerInterface const & rhs) noexcept(noexcept(lhs == rhs))
        -> decltype(v1_dtl::derived_container(lhs), lhs == rhs)
    {
        return !(lhs == rhs);
    }

    /** Implementation of `operator<()` for all containers derived from
        `container_interface`.  */
    template<typename ContainerInterface>
    constexpr auto operator<(
        ContainerInterface const & lhs,
        ContainerInterface const &
            rhs) noexcept(noexcept(*lhs.begin() < *rhs.begin()))
        -> decltype(
            v1_dtl::derived_container(lhs), *lhs.begin() < *rhs.begin(), true)
    {
        auto it1 = lhs.begin();
        auto const last1 = lhs.end();
        auto it2 = rhs.begin();
        auto const last2 = rhs.end();
        for (; it1 != last1 && it2 != last2; ++it1, ++it2) {
            if (*it1 < *it2)
                return true;
            if (*it2 < *it1)
                return false;
        }
        return it1 == last1 && it2 != last2;
    }

    /** Implementation of `operator<=()` for all containers derived from
        `container_interface`.  */
    template<typename ContainerInterface>
    constexpr auto operator<=(
        ContainerInterface const & lhs,
        ContainerInterface const & rhs) noexcept(noexcept(lhs < rhs))
        -> decltype(v1_dtl::derived_container(lhs), lhs < rhs)
    {
        return !(rhs < lhs);
    }

    /** Implementation of `operator>()` for all containers derived from
        `container_interface`.  */
    template<typename ContainerInterface>
    constexpr auto operator>(
        ContainerInterface const & lhs,
        ContainerInterface const & rhs) noexcept(noexcept(lhs < rhs))
        -> decltype(v1_dtl::derived_container(lhs), lhs < rhs)
    {
        return rhs < lhs;
    }

    /** Implementation of `operator>=()` for all containers derived from
        `container_interface`.  */
    template<typename ContainerInterface>
    constexpr auto operator>=(
        ContainerInterface const & lhs,
        ContainerInterface const & rhs) noexcept(noexcept(lhs < rhs))
        -> decltype(v1_dtl::derived_container(lhs), lhs < rhs)
    {
        return !(lhs < rhs);
    }

}}}


namespace boost { namespace stl_interfaces { namespace v2 {

    // This is only here to satisfy clang-format.
    namespace v2_dtl {
    }

    // clang-format off

#if 201703L < __cplusplus && defined(__cpp_lib_concepts) || BOOST_STL_INTERFACES_DOXYGEN

    /** A CRTP template that one may derive from to make it easier to define
        container types. */
    template<typename Derived>
      requires std::is_class_v<Derived> &&
               std::same_as<Derived, std::remove_cv_t<Derived>>
    struct container_interface {
    private:
      constexpr Derived& derived() noexcept {
        return static_cast<Derived&>(*this);
      }
      constexpr const Derived& derived() const noexcept {
        return static_cast<const Derived&>(*this);
      }
      constexpr Derived & mutable_derived() const noexcept {
        return const_cast<Derived&>(static_cast<const Derived&>(*this));
      }
      static constexpr void clear_impl(Derived& d) noexcept {}
      static constexpr void clear_impl(Derived& d) noexcept
        requires requires { d.clear()); } {
          d.clear();
        }

    public:
      ~container_interface() { clear_impl(derived()); }

      constexpr bool empty() requires std::forward_range<Derived> {
        return std::ranges::begin(derived()) == std::ranges::end(derived());
      }
      constexpr bool empty() const requires std::forward_range<const Derived> {
        return std::ranges::begin(derived()) == std::ranges::end(derived());
      }

      constexpr auto data() requires std::contiguous_iterator<std::iterator_t<Derived>> {
        return std::to_address(std::ranges::begin(derived()));
      }
      constexpr auto data() const
        requires std::range<const Derived> &&
          std::contiguous_iterator<std::iterator_t<const Derived>> {
            return std::to_address(std::ranges::begin(derived()));
          }

      constexpr auto size() requires std::forward_range<Derived> &&
        std::sized_sentinel_for<std::sentinel_t<Derived>, std::iterator_t<Derived>>
        -> Derived::size_type {
          return Derived::size_type(
            std::ranges::end(derived()) - std::ranges::begin(derived()));
        }
      constexpr auto size() const requires std::forward_range<const Derived> &&
        std::sized_sentinel_for<std::sentinel_t<const Derived>, std::iterator_t<const Derived>>
        -> Derived::size_type {
          return Derived::size_type(
            std::ranges::end(derived()) - std::ranges::begin(derived()));
        }

      constexpr decltype(auto) front() requires std::forward_range<Derived> {
        BOOST_ASSERT(!empty());
        return *std::ranges::begin(derived());
      }
      constexpr decltype(auto) front() const
        requires std::forward_range<const Derived> {
          BOOST_ASSERT(!empty());
          return *std::ranges::begin(derived());
        }

      constexpr void push_front(const Derived::value_type& x)
        requires std::forward_range<Derived> &&
          requires { derived().emplace_front(x); } {
            return derived().emplace_front(x);
          }
      constexpr void push_front(Derived::value_type&& x)
        requires std::forward_range<Derived> &&
          requires { derived().emplace_front(std::move(x)); } {
            return derived().emplace_front(std::move(x));
          }
      constexpr void pop_front() noexcept
          requires std::forward_range<Derived> &&
            requires (Derived::value_type x, Derived::const_iterator position) {
            derived().emplace_front(x); derived().erase(position); } {
              return derived().erase(std::ranges::begin(derived()));
            }

      constexpr decltype(auto) back()
        requires std::bidirectional_range<Derived> && std::common_range<Derived> {
          BOOST_ASSERT(!empty());
          return *std::ranges::prev(std::ranges::begin(derived()));
        }
      constexpr decltype(auto) back() const
        requires std::bidirectional_range<const Derived> && std::common_range<const Derived> {
          BOOST_ASSERT(!empty());
          return *std::ranges::prev(std::ranges::begin(derived()));
        }

      constexpr void push_back(const Derived::value_type& x)
        requires std::bidirectional_range<Derived> && std::common_range<Derived> &&
          requires { derived().emplace_back(x); } {
            return derived().emplace_back(x);
          }
      constexpr void push_back(Derived::value_type&& x)
        requires std::bidirectional_range<Derived> && std::common_range<Derived> &&
          requires { derived().emplace_back(std::move(x)); } {
            return derived().emplace_back(std::move(x));
          }
      constexpr void pop_back() noexcept
          requires std::bidirectional_range<Derived> && std::common_range<Derived> &&
            requires (Derived::value_type x, Derived::const_iterator position) {
            derived().emplace_back(x); derived().erase(position); } {
              return derived().erase(std::ranges::prev(std::ranges::end(derived())));
            }

      template<std::random_access_range Container = Derived>
      constexpr decltype(auto) operator[](Derived::size_type n) {
        return std::ranges::begin(derived())[n];
      }
      template<std::random_access_range Container = const Derived>
      constexpr decltype(auto) operator[](Derived::size_type n) const {
        return std::ranges::begin(derived())[n];
      }

      template<std::random_access_range Container = Derived>
      constexpr decltype(auto) at(Derived::size_type n) {
        if (derived().size() < n)
          throw std::out_of_range("Bounds check failed in container_interface::at()");
        return std::ranges::begin(derived())[n];
      }
      template<std::random_access_range Container = const Derived>
      constexpr decltype(auto) at(Derived::size_type n) const {
        if (derived().size() < n)
          throw std::out_of_range("Bounds check failed in container_interface::at()");
        return std::ranges::begin(derived())[n];
      }

      constexpr void resize(Derived::size_type n)
        requires std::forward_range<Derived> &&
          requires { derived().resize(n, Derived::value_type()); } {
            derived().resize(n, Derived::value_type());
          }

      constexpr auto begin() const requires std::forward_range<Derived> {
        return Derived::const_iterator(std::ranges::begin(mutable_derived()));
      }
      constexpr auto end() const requires std::forward_range<Derived> {
        return Derived::const_iterator(std::ranges::end(mutable_derived()));
      }

      constexpr auto cbegin() const requires std::forward_range<Derived> {
        return std::ranges::begin(derived());
      }
      constexpr auto cend() const requires std::forward_range<Derived> {
        return std::ranges::end(derived());
      }

      constexpr auto rbegin()
        requires std::bidirectional_range<Derived> && std::common_range<Derived> {
          return std::reverse_iterator(std::ranges::end(derived()));
        }
      constexpr auto rend()
        requires std::bidirectional_range<Derived> && std::common_range<Derived> {
          return std::reverse_iterator(std::ranges::begin(derived()));
        }

      constexpr auto rbegin() const
        requires std::bidirectional_range<Derived> && std::common_range<Derived> {
          return std::reverse_iterator(std::ranges::rbegin(mutable_derived()));
        }
      constexpr auto rend() const
        requires std::bidirectional_range<Derived> && std::common_range<Derived> {
          return std::reverse_iterator(std::ranges::rend(mutable_derived()));
        }

      constexpr auto crbegin() const
        requires std::bidirectional_range<Derived>  && std::common_range<Derived>{
          return std::ranges::rbegin(derived());
        }
      constexpr auto crend() const
        requires std::bidirectional_range<Derived>  && std::common_range<Derived>{
          return std::ranges::rend(derived());
        }

      constexpr auto insert(Derived::const_iterator position, const Derived::value_type& x)
        requires std::forward_range<Derived> && requires {
          derived().emplace(position, x); } {
            derived().emplace(position, x);
          }
      constexpr auto insert(Derived::const_iterator position, Derived::value_type&& x)
        requires std::forward_range<Derived> && requires {
          derived().emplace(position, std::move(x)); } {
            derived().emplace(position, std::move(x));
          }
      constexpr auto insert(Derived::const_iterator position, Derived::size_type n,
                            const Derived::value_type& x)
        requires std::forward_range<Derived> && requires {
          derived().insert(position, detail::make_n_iter(x, n),
                           detail::make_n_iter_end(x, n)); } {
            derived().insert(position,  detail::make_n_iter(x, n),
                             detail::make_n_iter_end(x, n));
          }
      constexpr auto insert(Derived::const_iterator position,
                            std::initializer_list<Derived::value_type> il)
        requires std::forward_range<Derived> && requires {
          derived().insert(position, il.begin(), il.end()); } {
            derived().insert(position, il.begin(), il.end());
          }

      constexpr void erase(Derived::const_iterator position)
          requires std::forward_range<Derived> && requires {
            derived().erase(position, std::ranges::next(position)); } {
              derived().erase(position, std::ranges::next(position));
            }

      template<std::forward_iterator Iter>
        constexpr void assign(Iter first, Iter last)
            requires std::forward_range<Derived> && requires {
              derived().clear();
              derived().insert(std::ranges::begin(derived()), first, last); } {
                derived().clear();
                derived().insert(std::ranges::begin(derived()), first, last);
              }
      constexpr void assign(Derived::size_type n, const Derived::value_type& x)
          requires std::forward_range<Derived> && requires {
            derived().clear();
            derived().insert(std::ranges::begin(derived()),
                             detail::make_n_iter(x, n),
                             detail::make_n_iter_end(x, n)); } {
              derived().clear();
              derived().insert(std::ranges::begin(derived()),
                               detail::make_n_iter(x, n),
                               detail::make_n_iter_end(x, n));
            }
      constexpr void assign(std::initializer_list<Derived::value_type> il)
          requires std::forward_range<Derived> && requires {
            derived().clear();
            derived().insert(std::ranges::begin(derived()), il.begin(), il.end()); } {
              derived().clear();
              derived().insert(std::ranges::begin(derived()), il.begin(), il.end());
            }

      constexpr void clear() noexcept
        requires std::forward_range<Derived> && requires {
          derived().erase(std::ranges::begin(derived()), std::ranges::end(derived())); } {
            derived().erase(std::ranges::begin(derived()), std::ranges::end(derived()));
          }

      constexpr decltype(auto) operator=(std::initializer_list<Derived::value_type> il)
        requires std::forward_range<Derived> && requires {
          derived().assign(il.begin(), il.end()); } {
            derived().assign(il.begin(), il.end());
          }

      friend constexpr void swap(Derived& lhs, Derived& rhs)
        requires requires { lhs.swap(rhs); } {
          return lhs.swap(rhs);
      }

      friend constexpr std::strong_ordering operator<=>(const Derived& lhs,
                                                        const Derived& rhs) {
          return std::lexicographical_compare_three_way(lhs.begin(), lhs.end(),
                                                        rhs.begin(), rhs.end());
      }
    };

#elif 201703L <= __cplusplus && __has_include(<stl2/ranges.hpp>) && \
    !defined(BOOST_STL_INTERFACES_DISABLE_CMCSTL2)

    namespace v2_dtl {
        // These named concepts are used to work around
        // https://gcc.gnu.org/bugzilla/show_bug.cgi?id=82740
        template<typename Derived>
        BOOST_STL_INTERFACES_CONCEPT lt = requires (Derived & d) { d < d; };

        template<typename Derived, typename ValueType>
        BOOST_STL_INTERFACES_CONCEPT empl_frnt =
            requires (Derived & d, ValueType&& x) { d.emplace_front((ValueType&&)x); };

        template<typename Derived, typename ValueType>
        BOOST_STL_INTERFACES_CONCEPT empl_back =
            requires (Derived & d, ValueType&& x) { d.emplace_back((ValueType&&)x); };

        template<typename Derived>
        BOOST_STL_INTERFACES_CONCEPT erase =
          requires (Derived & d, ranges::iterator_t<const Derived> pos) {
            d.erase(pos, pos);
          };

        template<typename Derived>
        BOOST_STL_INTERFACES_CONCEPT resz_n_x =
          requires (Derived & d,
                    typename Derived::size_type n,
                    const typename Derived::value_type& x) {
            d.resize(n, x);
          };

        template<typename Derived, typename ValueType>
        BOOST_STL_INTERFACES_CONCEPT empl =
          requires (Derived & d,
                    ranges::iterator_t<const Derived> pos,
                    ValueType&& x) {
            d.emplace(pos, (ValueType&&)x);
          };

        template<typename Derived>
        BOOST_STL_INTERFACES_CONCEPT erase_ =
          requires (Derived & d, ranges::iterator_t<const Derived> pos) {
            d.erase(pos, pos);
          };

        template<typename Derived, typename Iter>
        BOOST_STL_INTERFACES_CONCEPT insert_ =
          ranges::input_iterator<Iter> &&
            requires (Derived & d,
                      ranges::iterator_t<const Derived> pos,
                      Iter it) {
              d.insert(pos, it, it);
            };

        template<typename Derived>
        using n_iter_t = detail::n_iter<typename Derived::value_type,
                                        typename Derived::size_type>;

        template<typename Derived>
        BOOST_STL_INTERFACES_CONCEPT swap =
            requires (Derived & d) { d.swap(d); };

        template<typename Derived>
        BOOST_STL_INTERFACES_CONCEPT szd_sent_fwd_rng =
            ranges::forward_range<Derived> &&
            ranges::sized_sentinel_for<ranges::sentinel_t<Derived>,
                                       ranges::iterator_t<Derived>>;

        template<typename Derived, typename Iter>
        BOOST_STL_INTERFACES_CONCEPT erase_insert =
            v2_dtl::erase_<Derived> && v2_dtl::insert_<Derived, Iter>;

        // This needs to become an exposition-only snake-case template alias
        // when standardized.
        template<typename Derived>
        using container_size_t = typename Derived::size_type;

#if 201711L <= __cpp_lib_three_way_comparison
        // This *may* need to become an exposition-only snake-case template
        // when standardized.
        struct three_way {
          template<class T, class U>
            requires three_way_comparable_with<T, U>// || BUILTIN-PTR-CMP(T, <=>, U)
          constexpr bool operator()(T&& t, U&& u) const {
            return std::forward<T>(t) <=> std::forward<U>(u);
          }

          using is_transparent = std::true_type; // = unspecified;
        };
#endif
    }

    /** A CRTP template that one may derive from to make it easier to define
        container types. */
    template<typename Derived>
      requires std::is_class_v<Derived> &&
               ranges::same_as<Derived, std::remove_cv_t<Derived>>
    struct container_interface {
    private:
      constexpr Derived& derived() noexcept {
        return static_cast<Derived&>(*this);
      }
      constexpr const Derived& derived() const noexcept {
        return static_cast<const Derived&>(*this);
      }
      constexpr Derived & mutable_derived() const noexcept {
        return const_cast<Derived&>(static_cast<const Derived&>(*this));
      }
      static constexpr void clear_impl(Derived& d) noexcept {}
      static constexpr void clear_impl(Derived& d) noexcept
        requires v2_dtl::erase_<Derived> {
          d.clear();
        }

    public:
      ~container_interface() { clear_impl(derived()); }

      constexpr bool empty() requires ranges::forward_range<Derived> {
        return ranges::begin(derived()) == ranges::end(derived());
      }
      constexpr bool empty() const requires ranges::forward_range<const Derived> {
        return ranges::begin(derived()) == ranges::end(derived());
      }

#if 0 // TODO: Needs tests.
      constexpr auto data() requires ranges::contiguous_iterator<ranges::iterator_t<Derived>> {
        return &*ranges::begin(derived());
      }
      template<ranges::range Container = const Derived>
        constexpr auto data() const
          requires ranges::contiguous_iterator<ranges::iterator_t<Container>> {
              return &*ranges::begin(derived());
            }
#endif

      constexpr auto size() requires v2_dtl::szd_sent_fwd_rng<Derived> {
        return typename Derived::size_type(
          ranges::end(derived()) - ranges::begin(derived()));
      }
      constexpr auto size() const requires v2_dtl::szd_sent_fwd_rng<Derived> {
        return typename Derived::size_type(
          ranges::end(derived()) - ranges::begin(derived()));
      }

      constexpr decltype(auto) front() requires ranges::forward_range<Derived> {
        BOOST_ASSERT(!empty());
        return *ranges::begin(derived());
      }
      constexpr decltype(auto) front() const
        requires ranges::forward_range<const Derived> {
          BOOST_ASSERT(!empty());
          return *ranges::begin(derived());
        }

      template<ranges::forward_range Container = Derived>
        constexpr void push_front(const ranges::ext::range_value_t<Container>& x)
          requires v2_dtl::empl_frnt<Container, const ranges::ext::range_value_t<Container>&> {
              derived().emplace_front(x);
            }
      template<ranges::forward_range Container = Derived>
        constexpr void push_front(ranges::ext::range_value_t<Container>&& x)
          requires v2_dtl::empl_frnt<Container, ranges::ext::range_value_t<Container>&&> {
              derived().emplace_front(std::move(x));
            }
      template<ranges::forward_range Container = Derived>
        constexpr void pop_front() noexcept
          requires v2_dtl::empl_frnt<Container, const ranges::ext::range_value_t<Container>&> &&
            v2_dtl::erase_<Container> {
              derived().erase(ranges::begin(derived()));
            }

      constexpr decltype(auto) back()
        requires ranges::bidirectional_range<Derived> &&
          ranges::common_range<Derived> {
            BOOST_ASSERT(!empty());
            return *ranges::prev(ranges::end(derived()));
          }
      constexpr decltype(auto) back() const
        requires ranges::bidirectional_range<const Derived> &&
          ranges::common_range<const Derived> {
            BOOST_ASSERT(!empty());
            return *ranges::prev(ranges::end(derived()));
          }

      template<ranges::bidirectional_range Container = Derived>
        constexpr void push_back(const ranges::ext::range_value_t<Container>& x)
          requires ranges::common_range<Container> &&
            v2_dtl::empl_back<Container, const ranges::ext::range_value_t<Container>&> {
              derived().emplace_back(x);
            }
      template<ranges::bidirectional_range Container = Derived>
        constexpr void push_back(ranges::ext::range_value_t<Container>&& x)
          requires ranges::common_range<Container> &&
            v2_dtl::empl_back<Container, ranges::ext::range_value_t<Container>&&> {
              derived().emplace_back(std::move(x));
            }
      template<ranges::bidirectional_range Container = Derived>
        constexpr void pop_back() noexcept
          requires ranges::common_range<Container> &&
            v2_dtl::empl_back<Container, const ranges::ext::range_value_t<Container>&> &&
            v2_dtl::erase_<Container> {
              derived().erase(ranges::prev(ranges::end(derived())));
            }

      template<ranges::random_access_range Container = Derived>
        constexpr decltype(auto) operator[](v2_dtl::container_size_t<Container> n) {
          return ranges::begin(derived())[n];
        }
      template<ranges::random_access_range Container = const Derived>
        constexpr decltype(auto) operator[](v2_dtl::container_size_t<Container> n) const {
          return ranges::begin(derived())[n];
        }

      template<ranges::random_access_range Container = Derived>
        constexpr decltype(auto) at(v2_dtl::container_size_t<Container> n) {
          if (derived().size() <= n)
            throw std::out_of_range("Bounds check failed in container_interface::at()");
          return ranges::begin(derived())[n];
        }
      template<ranges::random_access_range Container = const Derived>
        constexpr decltype(auto) at(v2_dtl::container_size_t<Container> n) const {
          if (derived().size() <= n)
            throw std::out_of_range("Bounds check failed in container_interface::at()");
          return ranges::begin(derived())[n];
        }

      template<ranges::forward_range Container = Derived>
        requires v2_dtl::resz_n_x<Container>
        constexpr void resize(v2_dtl::container_size_t<Container> n) {
          derived().resize(n, ranges::ext::range_value_t<Container>());
        }

      constexpr auto begin() const {
        return typename Derived::const_iterator(ranges::begin(mutable_derived()));
      }
      constexpr auto end() const {
        return typename Derived::const_iterator(ranges::end(mutable_derived()));
      }

      constexpr auto cbegin() const requires ranges::forward_range<const Derived> {
        return ranges::begin(derived());
      }
      constexpr auto cend() const requires ranges::forward_range<const Derived> {
        return ranges::end(derived());
      }

      constexpr auto rbegin()
        requires ranges::bidirectional_range<Derived> &&
          ranges::common_range<Derived> {
            return stl_interfaces::reverse_iterator(ranges::end(derived()));
          }
      constexpr auto rend()
        requires ranges::bidirectional_range<Derived> &&
          ranges::common_range<Derived> {
            return stl_interfaces::reverse_iterator(ranges::begin(derived()));
          }

      constexpr auto rbegin() const
        requires ranges::bidirectional_range<const Derived> &&
          ranges::common_range<Derived> {
            return stl_interfaces::reverse_iterator(ranges::iterator_t<const Derived>(
              ranges::end(mutable_derived())));
          }
      constexpr auto rend() const
        requires ranges::bidirectional_range<const Derived> &&
          ranges::common_range<Derived> {
            return stl_interfaces::reverse_iterator(ranges::iterator_t<const Derived>(
              ranges::begin(mutable_derived())));
          }

      constexpr auto crbegin() const
        requires ranges::bidirectional_range<const Derived> &&
          ranges::common_range<const Derived> {
            return stl_interfaces::reverse_iterator(ranges::iterator_t<const Derived>(
              ranges::end(mutable_derived())));
          }
      constexpr auto crend() const
        requires ranges::bidirectional_range<const Derived> &&
          ranges::common_range<const Derived> {
            return stl_interfaces::reverse_iterator(ranges::iterator_t<const Derived>(
              ranges::begin(mutable_derived())));
          }

      template<ranges::forward_range Container = Derived>
        requires v2_dtl::empl<Container, const ranges::ext::range_value_t<Container>&>
      constexpr auto insert(ranges::iterator_t<const Container> position,
                            const ranges::ext::range_value_t<Container>& x) {
        return derived().emplace(position, x);
      }
      template<ranges::forward_range Container = Derived>
        requires v2_dtl::empl<Container, ranges::ext::range_value_t<Container>&&>
      constexpr auto insert(ranges::iterator_t<const Container> position,
                            ranges::ext::range_value_t<Container>&& x) {
        return derived().emplace(position, std::move(x));
      }

      template<ranges::forward_range Container = Derived>
        constexpr auto insert(ranges::iterator_t<const Container> position,
                              v2_dtl::container_size_t<Container> n,
                              const ranges::ext::range_value_t<Container>& x)
#if 0 // TODO: This ICEs GCC 8 and 9.
          requires v2_dtl::insert_<Container, v2_dtl::n_iter_t<Container>>
#endif
          {
            return derived().insert(position, detail::make_n_iter(x, n),
                                    detail::make_n_iter_end(x, n));
          }
      template<ranges::forward_range Container = Derived>
        constexpr auto insert(ranges::iterator_t<const Container> position,
                              std::initializer_list<ranges::ext::range_value_t<Container>> il)
          requires v2_dtl::insert_<Container, decltype(il.begin())> {
            return derived().insert(position, il.begin(), il.end());
          }

      template<ranges::forward_range Container = Derived>
        requires v2_dtl::erase_<Container>
      constexpr auto erase(ranges::iterator_t<const Container> position) {
        return derived().erase(position, ranges::next(position));
      }

      template<ranges::input_iterator Iter, ranges::forward_range Container = Derived>
        constexpr void assign(Iter first, Iter last)
          requires v2_dtl::erase_insert<Container, Iter> {
            derived().clear();
            derived().insert(ranges::begin(derived()), first, last);
          }
      template<ranges::forward_range Container = Derived>
        requires v2_dtl::erase_insert<Container, v2_dtl::n_iter_t<Container>>
          constexpr void assign(v2_dtl::container_size_t<Container> n,
                                const ranges::ext::range_value_t<Container>& x) {
            derived().clear();
            derived().insert(ranges::begin(derived()),
                             detail::make_n_iter(x, n),
                             detail::make_n_iter_end(x, n));
          }
      template<ranges::forward_range Container = Derived>
        constexpr void assign(std::initializer_list<ranges::ext::range_value_t<Container>> il)
          requires v2_dtl::erase_insert<Container, decltype(il.begin())> {
            derived().clear();
            derived().insert(ranges::begin(derived()), il.begin(), il.end());
          }

      constexpr void clear() noexcept
        requires ranges::forward_range<Derived> && v2_dtl::erase_<Derived> {
          derived().erase(ranges::begin(derived()), ranges::end(derived()));
        }

      template<ranges::forward_range Container = Derived>
      constexpr decltype(auto) operator=(
        std::initializer_list<ranges::ext::range_value_t<Container>> il)
          requires v2_dtl::erase_<Container> &&
            v2_dtl::insert_<Container, decltype(il.begin())> {
              derived().assign(il.begin(), il.end());
            }

      friend constexpr void swap(Derived& lhs, Derived& rhs)
        requires v2_dtl::swap<Derived> {
          return lhs.swap(rhs);
        }

#if 201711L <= __cpp_lib_three_way_comparison
      friend constexpr bool operator==(const Derived& lhs, const Derived& rhs)
        requires ranges::sized_range<const Derived> &&
          ranges::indirect_relation<ranges::equal_to, ranges::iterator_t<const Derived>> {
            return lhs.size() == rhs.size() && ranges::equal(lhs, rhs);
          }
      friend constexpr std::strong_ordering operator<=>(const Derived& lhs,
                                                        const Derived& rhs)
        requires ranges::indirect_relation<v2_dtl::three_way, ranges::iterator_t<const Derived>> {
          return std::lexicographical_compare_three_way(lhs.begin(), lhs.end(),
                                                        rhs.begin(), rhs.end());
        }
#else
      friend constexpr bool operator==(const Derived& lhs, const Derived& rhs)
        requires ranges::sized_range<const Derived> &&
          ranges::indirect_relation<ranges::equal_to, ranges::iterator_t<const Derived>> {
            return lhs.size() == rhs.size() &&
                   std::equal(lhs.begin(), lhs.end(), rhs.begin(), rhs.end());
          }
      friend constexpr bool operator!=(const Derived& lhs, const Derived& rhs)
        requires ranges::sized_range<const Derived> &&
          ranges::indirect_relation<ranges::equal_to, ranges::iterator_t<const Derived>> {
            return !(lhs == rhs);
          }
      friend constexpr bool operator<(const Derived& lhs, const Derived& rhs)
        requires ranges::indirect_relation<ranges::less, ranges::iterator_t<const Derived>> {
          auto it1 = lhs.begin();
          auto const last1 = lhs.end();
          auto it2 = rhs.begin();
          auto const last2 = rhs.end();
          for (; it1 != last1 && it2 != last2; ++it1, ++it2) {
            if (*it1 < *it2)
              return true;
            if (*it2 < *it1)
              return false;
          }
          return it1 == last1 && it2 != last2;
        }
      friend constexpr bool operator<=(const Derived& lhs, const Derived& rhs)
        requires ranges::indirect_relation<ranges::less, ranges::iterator_t<const Derived>> {
          return !(rhs < lhs);
        }
      friend constexpr bool operator>(const Derived& lhs, const Derived& rhs)
        requires ranges::indirect_relation<ranges::less, ranges::iterator_t<const Derived>> {
          return rhs < lhs;
        }
      friend constexpr bool operator>=(const Derived& lhs, const Derived& rhs)
        requires ranges::indirect_relation<ranges::less, ranges::iterator_t<const Derived>> {
          return !(lhs < rhs);
        }
#endif
    };

#endif

    // clang-format on

}}}

#endif
