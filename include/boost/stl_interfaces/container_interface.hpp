// Copyright (C) 2019 T. Zachary Laine
//
// Distributed under the Boost Software License, Version 1.0. (See
// accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
#ifndef BOOST_STL_INTERFACES_CONTAINER_INTERFACE_HPP
#define BOOST_STL_INTERFACES_CONTAINER_INTERFACE_HPP

#include <boost/stl_interfaces/reverse_iterator.hpp>

#include <stdexcept>


namespace boost { namespace stl_interfaces {

    namespace detail {
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
            BOOST_STL_INTERFACES_STATIC_ASSERT_CONCEPT(
                result_type, std::random_access_iterator)
            return result_type(x, SizeType(0));
        }
        template<typename T, typename SizeType>
        constexpr auto make_n_iter_end(T const & x, SizeType n) noexcept(
            noexcept(n_iter<T, SizeType>(x, n)))
        {
            return n_iter<T, SizeType>(x, n);
        }

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
    }

    // TODO (maybe one day...)
    // Associative container requirements (only those not covered by the earlier tables):
    // try_emplace -> all unique emplaces and inserts, except emplace_hint,
    // hinted insert, and node handle inserts emplace (in absence of
    // try_emplace) -> all non-unique emplaces and inserts, except emplace_hint,
    // hinted insert, and node handle inserts emplace_hint -> hinted insert
    // lower_bound,upper_bound ->
    // equal_range,find(tran?),count(tran?),contains(tran?) erase(q),equal_range
    // -> erase(k),erase(r),erase(q1,q2)

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
        using derived_container_type = Derived;

        ~container_interface() { detail::clear_impl<Derived>::call(derived()); }

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

        template<
            typename D = Derived,
            typename Enable = std::enable_if_t<
                detail::decrementable_sentinel<D>::value &&
                detail::common_range<D>::value>>
        constexpr auto
        back() noexcept(noexcept(*std::prev(std::declval<D &>().end())))
            -> decltype(*std::prev(std::declval<D &>().end()))
        {
            return *std::prev(derived().end());
        }
        template<
            typename D = Derived,
            typename Enable = std::enable_if_t<
                detail::decrementable_sentinel<D>::value &&
                detail::common_range<D>::value>>
        constexpr auto back() const
            noexcept(noexcept(*std::prev(std::declval<D const &>().end())))
                -> decltype(*std::prev(std::declval<D const &>().end()))
        {
            return *std::prev(derived().end());
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
            typename Enable = std::enable_if_t<detail::common_range<D>::value>>
        constexpr auto rbegin() noexcept(noexcept(
            stl_interfaces::make_reverse_iterator(std::declval<D &>().end())))
        {
            return stl_interfaces::make_reverse_iterator(derived().end());
        }
        template<
            typename D = Derived,
            typename Enable = std::enable_if_t<detail::common_range<D>::value>>
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
            typename D::value_type const &
                x) noexcept(noexcept(std::declval<D &>()
                                         .insert(
                                             pos,
                                             detail::make_n_iter(x, n),
                                             detail::make_n_iter_end(x, n))))
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
                std::enable_if_t<detail::in_iter<InputIterator>::value>>
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
        constexpr auto at(typename D::size_type i)
            -> decltype(std::declval<D &>().size(), std::declval<D &>()[i])
        {
            if (derived().size() <= i) {
                throw std::out_of_range(
                    "Bounds check failed in static_vector::at()");
            }
            return derived()[i];
        }

        template<typename D = Derived>
        constexpr auto at(typename D::size_type i) const -> decltype(
            std::declval<D const &>().size(), std::declval<D const &>()[i])
        {
            if (derived().size() <= i) {
                throw std::out_of_range(
                    "Bounds check failed in static_vector::at()");
            }
            return derived()[i];
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
        -> decltype(
            detail::dummy<
                typename ContainerInterface::derived_container_type>(),
            lhs.swap(rhs))
    {
        return lhs.swap(rhs);
    }

    /** Implementation of `operator<=()` for all containers derived from
        `container_interface`.  */
    template<typename ContainerInterface>
    constexpr auto operator<=(
        ContainerInterface lhs,
        ContainerInterface rhs) noexcept(noexcept(lhs < rhs))
        -> decltype(
            detail::dummy<
                typename ContainerInterface::derived_container_type>(),
            lhs < rhs)
    {
        return !(rhs < lhs);
    }

    /** Implementation of `operator>()` for all containers derived from
        `container_interface`.  */
    template<typename ContainerInterface>
    constexpr auto operator>(
        ContainerInterface lhs,
        ContainerInterface rhs) noexcept(noexcept(lhs < rhs))
        -> decltype(
            detail::dummy<
                typename ContainerInterface::derived_container_type>(),
            lhs < rhs)
    {
        return rhs < lhs;
    }

    /** Implementation of `operator>=()` for all containers derived from
        `container_interface`.  */
    template<typename ContainerInterface>
    constexpr auto operator>=(
        ContainerInterface lhs,
        ContainerInterface rhs) noexcept(noexcept(lhs < rhs))
        -> decltype(
            detail::dummy<
                typename ContainerInterface::derived_container_type>(),
            lhs < rhs)
    {
        return !(lhs < rhs);
    }

}}

#endif
