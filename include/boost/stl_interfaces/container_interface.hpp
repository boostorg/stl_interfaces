// Copyright (C) 2019 T. Zachary Laine
//
// Distributed under the Boost Software License, Version 1.0. (See
// accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
#ifndef BOOST_STL_INTERFACES_CONTAINER_INTERFACE_HPP
#define BOOST_STL_INTERFACES_CONTAINER_INTERFACE_HPP

#include <boost/stl_interfaces/view_interface.hpp>
#include <boost/stl_interfaces/reverse_iterator.hpp>


namespace boost { namespace stl_interfaces {

    namespace detail {
        template<typename T, typename = void>
        struct has_reserve : std::false_type
        {
        };
        template<typename T>
        struct has_reserve<T, void_t<decltype(std::declval<T &>().reserve())>>
            : std::true_type
        {
        };

        template<typename T, typename SizeType>
        struct n_iter : boost::stl_interfaces::iterator_interface<
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
            friend boost::stl_interfaces::access;
            constexpr T const *& base_reference() noexcept { return x_; }
            constexpr T const * base_reference() const noexcept { return x_; }

            T const * x_;
            SizeType n_;
        };

        template<typename T, typename SizeType>
        constexpr auto make_n_iter(T const & x, SizeType n) noexcept(
            noexcept(n_iter<T, SizeType>(x, n)))
        {
            return n_iter<T, SizeType>(x, SizeType(0));
        }
        template<typename T, typename SizeType>
        constexpr auto make_n_iter_end(T const & x, SizeType n) noexcept(
            noexcept(n_iter<T, SizeType>(x, n)))
        {
            return n_iter<T, SizeType>(x, n);
        }
    }

    // TODO:

    // Container requirements:
    // mutable begin/end -> const begin/end,cbegin/cend
    // c1 == c2 -> c1 != c2
    // c1 < c2 -> c1 <= c2,c1 > c2,c1 >= c2
    // random access begin/end -> size
    // equality-comparable begin/end -> empty
    // c1.swap(c2) -> swap(c1, c2)

    // Reversible container requirements:
    // mutable begin/end -> mutable rbegin/rend (requires detection of proxies for pre-C++20 code, because reverse_iterator does not handle proxies properly before that)
    // mutable rbegin/rend -> const rbegin/rendc,rbegin/crend

    // Sequence container requirements:
    // C(il) -> c = il
    // emplace,insert(p,i,j) -> insert(p,t),insert(p,rv),insert(p,n,t),insert(p,il)
    // clear,insert(p,i,j) -> assign(i,j),assign(n,t),assign(il)
    // emplace_front -> push_front(t),push_front(rv)
    // emplace_front,erase -> pop_front
    // emplace_back -> push_back(t),push_back(rv)
    // emplace_back,erase -> pop_back
    // random access begin/end -> operator[]
    // operator[] -> at

    // Associative container requirements (only those not covered above):
    // try_emplace -> all unique emplaces and inserts, except emplace_hint, hinted insert, and node handle inserts
    // emplace (in absence of try_emplace) -> all non-unique emplaces and inserts, except emplace_hint, hinted insert, and node handle inserts
    // emplace_hint -> hinted insert
    // lower_bound,upper_bound -> equal_range,find(tran?),count(tran?),contains(tran?)
    // erase(q),equal_range -> erase(k),erase(r),erase(q1,q2)

    // TODO: Document that the reverse_iterator type of Derived should be
    // stl_interfaces::reverse_iterator<iterator> if you want the reverse
    // iterator API to be defined by container_interface.

    // TODO: Document that iterator is expected to be implicitly convertible
    // to const_iterator.

    /** A CRTP template that one may derive from to make it easier to define
        container types. */
    template<typename Derived, bool Contiguous = discontiguous>
    struct container_interface : view_interface<Derived, Contiguous, true>
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
        // TODO: size() from view_interface is actually returning a
        // difference_type, nto a size_type.

        template<typename D = Derived>
        constexpr auto begin() const
            noexcept(noexcept(std::declval<D &>().begin()))
                -> decltype(std::declval<D &>().begin())
        {
            return derived().begin();
        }
        template<typename D = Derived>
        constexpr auto end() const
            noexcept(noexcept(std::declval<D &>().end()))
                -> decltype(std::declval<D &>().end())
        {
            return derived().end();
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
            -> decltype(
                --std::declval<D &>().end(),
                stl_interfaces::make_reverse_iterator(
                    std::declval<D &>().end()))
        {
            return stl_interfaces::make_reverse_iterator(derived().end());
        }
        template<
            typename D = Derived,
            typename Enable = std::enable_if_t<detail::common_range<D>::value>>
        constexpr auto rend() noexcept(noexcept(
            stl_interfaces::make_reverse_iterator(std::declval<D &>().begin())))
            -> decltype(
                --std::declval<D &>().end(),
                stl_interfaces::make_reverse_iterator(
                    std::declval<D &>().begin()))
        {
            return stl_interfaces::make_reverse_iterator(derived().begin());
        }

        template<typename D = Derived>
        constexpr auto rbegin() const
            noexcept(noexcept(std::declval<D &>().rbegin()))
                -> decltype(std::declval<D &>().rbegin())
        {
            return derived().rbegin();
        }
        template<typename D = Derived>
        constexpr auto rend() const
            noexcept(noexcept(std::declval<D &>().rend()))
                -> decltype(std::declval<D &>().rend())
        {
            return derived().rend();
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
            -> decltype(std::declval<D &>().insert(
                pos, detail::make_n_iter(x, n), detail::make_n_iter_end(x, n)))
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

        template<class InputIterator, typename D = Derived>
        constexpr auto
        assign(InputIterator first, InputIterator last) noexcept(noexcept(
            std::declval<D &>().clear(),
            std::declval<D &>().insert(first, last)))
            -> decltype(std::declval<D &>().insert(first, last))
        {
            derived().clear();
            return derived().insert(derived().begin(), first, last);
        }

        template<typename D = Derived>
        constexpr auto
        assign(typename D::size_type n, typename D::value_type const & x) noexcept(
            noexcept(
                std::declval<D &>().clear(),
                std::declval<D &>().insert(
                    std::declval<D &>().begin(),
                    detail::make_n_iter(x, n),
                    detail::make_n_iter_end(x, n))))
            -> decltype((void)std::declval<D &>().insert(
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
            -> decltype(std::declval<D &>().assign(il.begin(), il.end()))
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
            return derived().emplace_front(x);
        }

        template<typename D = Derived>
        constexpr auto push_front(typename D::value_type && x) noexcept(
            noexcept(std::declval<D &>().emplace_front(std::move(x))))
            -> decltype((void)std::declval<D &>().emplace_front(std::move(x)))
        {
            return derived().emplace_front(std::move(x));
        }

        template<typename D = Derived>
        constexpr auto pop_front(typename D::value_type && x) noexcept(
            noexcept(std::declval<D &>().erase(std::declval<D &>().begin())))
            -> decltype(
                std::declval<D &>().emplace_front(
                    std::declval<typename D::value_type &>()),
                (void)std::declval<D &>().erase(std::declval<D &>().begin()))
        {
            return derived().erase(derived().begin());
        }

        template<typename D = Derived>
        constexpr auto push_back(typename D::value_type const & x) noexcept(
            noexcept(std::declval<D &>().emplace_back(x)))
            -> decltype((void)std::declval<D &>().emplace_back(x))
        {
            return derived().emplace_back(x);
        }

        template<typename D = Derived>
        constexpr auto push_back(typename D::value_type && x) noexcept(
            noexcept(std::declval<D &>().emplace_back(std::move(x))))
            -> decltype((void)std::declval<D &>().emplace_back(std::move(x)))
        {
            return derived().emplace_back(std::move(x));
        }

        template<typename D = Derived>
        constexpr auto pop_back(typename D::value_type && x) noexcept(
            noexcept(std::declval<D &>().erase(--std::declval<D &>().end())))
            -> decltype(
                std::declval<D &>().emplace_back(
                    std::declval<typename D::value_type &>()),
                (void)std::declval<D &>().erase(--std::declval<D &>().end()))
        {
            return derived().erase(--derived().end());
        }

        template<typename D = Derived>
        constexpr auto at(typename D::size_type i) noexcept(
            noexcept(std::declval<D &>().size(), std::declval<D &>()[i]))
            -> decltype(std::declval<D &>().size(), std::declval<D &>()[i])
        {
            if (derived().size() <= i) {
                throw std::out_of_range(
                    "Bounds check failed in static_vector::at()");
            }
            return derived()[i];
        }

        template<typename D = Derived>
        constexpr auto clear(typename D::size_type i) noexcept(
            noexcept(std::declval<D &>().erase(
                std::declval<D &>().begin(), std::declval<D &>().end())))
            -> decltype((void)std::declval<D &>().erase(
                std::declval<D &>().begin(), std::declval<D &>().end()))
        {
            return derived().erase(derived().begin(), derived().end());
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
    constexpr auto operator<=(ContainerInterface lhs, ContainerInterface rhs) noexcept(
        noexcept(lhs < rhs))
        -> decltype(
            detail::dummy<typename ContainerInterface::derived_container_type>(),
            lhs < rhs)
    {
        return !(rhs < lhs);
    }

    /** Implementation of `operator>()` for all containers derived from
        `container_interface`.  */
    template<typename ContainerInterface>
    constexpr auto operator>(ContainerInterface lhs, ContainerInterface rhs) noexcept(
        noexcept(lhs < rhs))
        -> decltype(
            detail::dummy<typename ContainerInterface::derived_container_type>(),
            lhs < rhs)
    {
        return rhs < lhs;
    }

    /** Implementation of `operator>=()` for all containers derived from
        `container_interface`.  */
    template<typename ContainerInterface>
    constexpr auto operator>=(ContainerInterface lhs, ContainerInterface rhs) noexcept(
        noexcept(lhs < rhs))
        -> decltype(
            detail::dummy<typename ContainerInterface::derived_container_type>(),
            lhs < rhs)
    {
        return !(lhs < rhs);
    }

}}

#endif
