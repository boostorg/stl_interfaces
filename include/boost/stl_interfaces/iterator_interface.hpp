// Copyright (C) 2019 T. Zachary Laine
//
// Distributed under the Boost Software License, Version 1.0. (See
// accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
#ifndef BOOST_STL_INTERFACES_ITERATOR_INTERFACE_HPP
#define BOOST_STL_INTERFACES_ITERATOR_INTERFACE_HPP

#include <boost/stl_interfaces/fwd.hpp>

#include <utility>
#include <type_traits>
#if defined(__cpp_lib_three_way_comparison)
#include <compare>
#endif


namespace boost { namespace stl_interfaces {

    /** A type for granting access to the private members of an iterator
        derived from `iterator_interface`. */
    struct access
    {
#ifndef BOOST_STL_INTERFACES_DOXYGEN

        template<typename D>
        static constexpr auto base(D & d) noexcept
            -> decltype(d.base_reference())
        {
            return d.base_reference();
        }
        template<typename D>
        static constexpr auto base(D const & d) noexcept
            -> decltype(d.base_reference())
        {
            return d.base_reference();
        }

#endif
    };

    /** The return type of `operator->()` in a proxy iterator.

        This template is used as the default `Pointer` template parameter in
        the `proxy_iterator_interface` template alias.  Note that the use of
        this template implies a copy or move of the underlying object of type
        `T`. */
    template<typename T>
#if defined(BOOST_STL_INTERFACES_DOXYGEN) || BOOST_STL_INTERFACES_USE_CONCEPTS
    // clang-format off
        requires std::is_object_v<T>
#endif
    struct proxy_arrow_result
    // clang-format on
    {
        constexpr proxy_arrow_result(T const & value) noexcept(
            noexcept(T(value))) :
            value_(value)
        {}
        constexpr proxy_arrow_result(T && value) noexcept(
            noexcept(T(std::move(value)))) :
            value_(std::move(value))
        {}

        constexpr T const * operator->() const noexcept { return &value_; }
        constexpr T * operator->() noexcept { return &value_; }

    private:
        T value_;
    };

    namespace detail {
        template<typename Pointer, typename Reference, typename T>
        auto make_pointer(
            T && value,
            std::enable_if_t<
                std::is_pointer<Pointer>::value &&
                    std::is_reference<Reference>::value,
                int> = 0) -> decltype(std::addressof(value))
        {
            return std::addressof(value);
        }

        template<typename Pointer, typename Reference, typename T>
        auto make_pointer(
            T && value,
            std::enable_if_t<
                !std::is_pointer<Pointer>::value &&
                    !std::is_same<Pointer, void>::value &&
                    std::is_reference<Reference>::value,
                int> = 0)
        {
            return Pointer(std::forward<T>(value));
        }

        template<typename Pointer, typename IteratorConcept>
        struct pointer
        {
            using type = Pointer;
        };
        template<typename Pointer>
        struct pointer<Pointer, std::output_iterator_tag>
        {
            using type = void;
        };
        template<typename Pointer, typename IteratorConcept>
        using pointer_t = typename pointer<Pointer, IteratorConcept>::type;

        template<typename T, typename U>
        using interoperable = std::integral_constant<
            bool,
            (std::is_convertible<T, U>::value ||
             std::is_convertible<U, T>::value)>;

        template<typename T, typename U>
        using common_t =
            std::conditional_t<std::is_convertible<T, U>::value, U, T>;

        template<typename T>
        using use_base = decltype(access::base(std::declval<T &>()));

        template<typename... T>
        using void_t = void;

        template<
            typename AlwaysVoid,
            template<class...> class Template,
            typename... Args>
        struct detector : std::false_type
        {
        };

        template<template<class...> class Template, typename... Args>
        struct detector<void_t<Template<Args...>>, Template, Args...>
            : std::true_type
        {
        };

        template<
            typename T,
            typename U,
            bool UseBase = detector<void, use_base, T>::value>
        struct common_eq
        {
            static constexpr auto call(T lhs, U rhs)
            {
                return static_cast<common_t<T, U>>(lhs).derived() ==
                       static_cast<common_t<T, U>>(rhs).derived();
            }
        };
        template<typename T, typename U>
        struct common_eq<T, U, true>
        {
            static constexpr auto call(T lhs, U rhs)
            {
                return access::base(lhs) == access::base(rhs);
            }
        };

        template<typename T, typename U>
        constexpr auto common_diff(T lhs, U rhs) noexcept(noexcept(
            static_cast<common_t<T, U>>(lhs) -
            static_cast<common_t<T, U>>(rhs)))
            -> decltype(
                static_cast<common_t<T, U>>(lhs) -
                static_cast<common_t<T, U>>(rhs))
        {
            return static_cast<common_t<T, U>>(lhs) -
                   static_cast<common_t<T, U>>(rhs);
        }
    }

}}

namespace boost { namespace stl_interfaces { BOOST_STL_INTERFACES_NAMESPACE_V1 {

    /** A CRTP template that one may derive from to make defining iterators
        easier.

        The template parameter `D` for `iterator_interface` may be an
        incomplete type.  Before any member of the resulting specialization of
        `iterator_interface` other than special member functions is
        referenced, `D` shall be complete, and model
        `std::derived_from<iterator_interface<D>>`. */
    template<
        typename Derived,
        typename IteratorConcept,
        typename ValueType,
        typename Reference = ValueType &,
        typename Pointer = ValueType *,
        typename DifferenceType = std::ptrdiff_t
#ifndef BOOST_STL_INTERFACES_DOXYGEN
        ,
        typename E = std::enable_if_t<
            std::is_class<Derived>::value &&
            std::is_same<Derived, std::remove_cv_t<Derived>>::value>
#endif
        >
    struct iterator_interface;

    namespace v1_dtl {
        template<typename Iterator, typename = void>
        struct ra_iter : std::false_type
        {
        };
        template<typename Iterator>
        struct ra_iter<Iterator, void_t<typename Iterator::iterator_concept>>
            : std::integral_constant<
                  bool,
                  std::is_base_of<
                      std::random_access_iterator_tag,
                      typename Iterator::iterator_concept>::value>
        {
        };

        template<typename Iterator, typename DifferenceType, typename = void>
        struct plus_eq : std::false_type
        {
        };
        template<typename Iterator, typename DifferenceType>
        struct plus_eq<
            Iterator,
            DifferenceType,
            void_t<decltype(
                std::declval<Iterator &>() += std::declval<DifferenceType>())>>
            : std::true_type
        {
        };

        template<
            typename D,
            typename IteratorConcept,
            typename ValueType,
            typename Reference,
            typename Pointer,
            typename DifferenceType>
        void derived_iterator(iterator_interface<
                              D,
                              IteratorConcept,
                              ValueType,
                              Reference,
                              Pointer,
                              DifferenceType> const &);
    }

    template<
        typename Derived,
        typename IteratorConcept,
        typename ValueType,
        typename Reference,
        typename Pointer,
        typename DifferenceType
#ifndef BOOST_STL_INTERFACES_DOXYGEN
        ,
        typename E
#endif
        >
    struct iterator_interface
    {
#ifndef BOOST_STL_INTERFACES_DOXYGEN
    private:
        constexpr Derived & derived() noexcept
        {
            return static_cast<Derived &>(*this);
        }
        constexpr Derived const & derived() const noexcept
        {
            return static_cast<Derived const &>(*this);
        }

        template<typename T, typename U, bool UseBase>
        friend struct detail::common_eq;
#endif

    public:
        using iterator_concept = IteratorConcept;
        using iterator_category = iterator_concept;
        using value_type = std::remove_const_t<ValueType>;
        using reference = Reference;
        using pointer = detail::pointer_t<Pointer, iterator_concept>;
        using difference_type = DifferenceType;

        template<typename D = Derived>
        constexpr auto operator*()
            noexcept(noexcept(*access::base(std::declval<D &>())))
                -> decltype(*access::base(std::declval<D &>()))
        {
            return *access::base(derived());
        }
        template<typename D = Derived>
        constexpr auto operator*() const
            noexcept(noexcept(*access::base(std::declval<D const &>())))
                -> decltype(*access::base(std::declval<D const &>()))
        {
            return *access::base(derived());
        }

        template<typename D = Derived>
        constexpr auto operator->() noexcept(noexcept(
            detail::make_pointer<pointer, reference>(*std::declval<D &>())))
            -> decltype(
                detail::make_pointer<pointer, reference>(*std::declval<D &>()))
        {
            return detail::make_pointer<pointer, reference>(*derived());
        }
        template<typename D = Derived>
        constexpr auto operator->() const noexcept(noexcept(
            detail::make_pointer<pointer, reference>(
                *std::declval<D const &>())))
            -> decltype(
                detail::make_pointer<pointer, reference>(
                    *std::declval<D const &>()))
        {
            return detail::make_pointer<pointer, reference>(*derived());
        }

        template<typename D = Derived>
        constexpr auto operator[](difference_type i) const noexcept(noexcept(
            D(std::declval<D const &>()),
            std::declval<D &>() += i,
            *std::declval<D &>()))
            -> decltype(std::declval<D &>() += i, *std::declval<D &>())
        {
            D retval = derived();
            retval += i;
            return *retval;
        }

        template<
            typename D = Derived,
            typename Enable =
                std::enable_if_t<!v1_dtl::plus_eq<D, difference_type>::value>>
        constexpr auto
        operator++() noexcept(noexcept(++access::base(std::declval<D &>())))
            -> decltype(
                ++access::base(std::declval<D &>()), std::declval<D &>())
        {
            ++access::base(derived());
            return derived();
        }

        template<typename D = Derived>
        constexpr auto operator++() noexcept(
            noexcept(std::declval<D &>() += difference_type(1)))
            -> decltype(
                std::declval<D &>() += difference_type(1), std::declval<D &>())
        {
            derived() += difference_type(1);
            return derived();
        }
        template<typename D = Derived>
        constexpr auto operator++(int)noexcept(
            noexcept(D(std::declval<D &>()), ++std::declval<D &>()))
            -> std::remove_reference_t<decltype(
                D(std::declval<D &>()),
                ++std::declval<D &>(),
                std::declval<D &>())>
        {
            D retval = derived();
            ++derived();
            return retval;
        }

        template<typename D = Derived>
        constexpr auto operator+=(difference_type n) noexcept(
            noexcept(access::base(std::declval<D &>()) += n))
            -> decltype(
                access::base(std::declval<D &>()) += n, std::declval<D &>())
        {
            access::base(derived()) += n;
            return derived();
        }

        template<typename D = Derived>
        constexpr auto operator+(difference_type i) const
            noexcept(noexcept(D(std::declval<D &>()), std::declval<D &>() += i))
                -> std::remove_reference_t<decltype(
                    D(std::declval<D &>()),
                    std::declval<D &>() += i,
                    std::declval<D &>())>
        {
            D retval = derived();
            retval += i;
            return retval;
        }
        friend BOOST_STL_INTERFACES_HIDDEN_FRIEND_CONSTEXPR Derived
        operator+(difference_type i, Derived it) noexcept
        {
            return it + i;
        }

        template<
            typename D = Derived,
            typename Enable =
                std::enable_if_t<!v1_dtl::plus_eq<D, difference_type>::value>>
        constexpr auto
        operator--() noexcept(noexcept(--access::base(std::declval<D &>())))
            -> decltype(--access::base(std::declval<D &>()), std::declval<D &>())
        {
            --access::base(derived());
            return derived();
        }

        template<typename D = Derived>
        constexpr auto operator--() noexcept(noexcept(
            D(std::declval<D &>()), std::declval<D &>() += -difference_type(1)))
            -> decltype(
                std::declval<D &>() += -difference_type(1), std::declval<D &>())
        {
            derived() += -difference_type(1);
            return derived();
        }
        template<typename D = Derived>
        constexpr auto operator--(int)noexcept(
            noexcept(D(std::declval<D &>()), --std::declval<D &>()))
            -> std::remove_reference_t<decltype(
                D(std::declval<D &>()),
                --std::declval<D &>(),
                std::declval<D &>())>
        {
            D retval = derived();
            --derived();
            return retval;
        }

        template<typename D = Derived>
        constexpr D & operator-=(difference_type i) noexcept
        {
            derived() += -i;
            return derived();
        }

        template<typename D = Derived>
        constexpr auto operator-(D other) const noexcept(noexcept(
            access::base(std::declval<D const &>()) - access::base(other)))
            -> decltype(
                access::base(std::declval<D const &>()) - access::base(other))
        {
            return access::base(derived()) - access::base(other);
        }

        friend BOOST_STL_INTERFACES_HIDDEN_FRIEND_CONSTEXPR Derived
        operator-(Derived it, difference_type i) noexcept
        {
            Derived retval = it;
            retval += -i;
            return retval;
        }
    };

    /** Implementation of `operator==()`, implemented in terms of the iterator
        underlying IteratorInterface, for all iterators derived from
        `iterator_interface`, except those with an iterator category derived
        from `std::random_access_iterator_tag`.  */
    template<
        typename IteratorInterface1,
        typename IteratorInterface2,
        typename Enable =
            std::enable_if_t<!v1_dtl::ra_iter<IteratorInterface1>::value>>
    constexpr auto
    operator==(IteratorInterface1 lhs, IteratorInterface2 rhs) noexcept
        -> decltype(
            access::base(std::declval<IteratorInterface1 &>()) ==
            access::base(std::declval<IteratorInterface2 &>()))
    {
        return access::base(lhs) == access::base(rhs);
    }

    /** Implementation of `operator==()` for all iterators derived from
        `iterator_interface` that have an iterator category derived from
        `std::random_access_iterator_tag`.  */
    template<
        typename IteratorInterface1,
        typename IteratorInterface2,
        typename Enable =
            std::enable_if_t<v1_dtl::ra_iter<IteratorInterface1>::value>>
    constexpr auto
    operator==(IteratorInterface1 lhs, IteratorInterface2 rhs) noexcept(
        noexcept(detail::common_diff(lhs, rhs)))
        -> decltype(
            v1_dtl::derived_iterator(lhs), detail::common_diff(lhs, rhs) == 0)
    {
        return detail::common_diff(lhs, rhs) == 0;
    }

    /** Implementation of `operator!=()` for all iterators derived from
        `iterator_interface`.  */
    template<typename IteratorInterface1, typename IteratorInterface2>
    constexpr auto operator!=(
        IteratorInterface1 lhs,
        IteratorInterface2 rhs) noexcept(noexcept(!(lhs == rhs)))
        -> decltype(v1_dtl::derived_iterator(lhs), !(lhs == rhs))
    {
        return !(lhs == rhs);
    }

    /** Implementation of `operator<()` for all iterators derived from
        `iterator_interface` that have an iterator category derived from
        `std::random_access_iterator_tag`.  */
    template<typename IteratorInterface1, typename IteratorInterface2>
    constexpr auto
    operator<(IteratorInterface1 lhs, IteratorInterface2 rhs) noexcept(
        noexcept(detail::common_diff(lhs, rhs)))
        -> decltype(
            v1_dtl::derived_iterator(lhs), detail::common_diff(lhs, rhs) < 0)
    {
        return detail::common_diff(lhs, rhs) < 0;
    }

    /** Implementation of `operator<=()` for all iterators derived from
        `iterator_interface` that have an iterator category derived from
        `std::random_access_iterator_tag`.  */
    template<typename IteratorInterface1, typename IteratorInterface2>
    constexpr auto
    operator<=(IteratorInterface1 lhs, IteratorInterface2 rhs) noexcept(
        noexcept(detail::common_diff(lhs, rhs)))
        -> decltype(
            v1_dtl::derived_iterator(lhs), detail::common_diff(lhs, rhs) <= 0)
    {
        return detail::common_diff(lhs, rhs) <= 0;
    }

    /** Implementation of `operator>()` for all iterators derived from
        `iterator_interface` that have an iterator category derived from
        `std::random_access_iterator_tag`.  */
    template<typename IteratorInterface1, typename IteratorInterface2>
    constexpr auto
    operator>(IteratorInterface1 lhs, IteratorInterface2 rhs) noexcept(
        noexcept(detail::common_diff(lhs, rhs)))
        -> decltype(
            v1_dtl::derived_iterator(lhs), detail::common_diff(lhs, rhs) > 0)
    {
        return detail::common_diff(lhs, rhs) > 0;
    }

    /** Implementation of `operator>=()` for all iterators derived from
        `iterator_interface` that have an iterator category derived from
        `std::random_access_iterator_tag`.  */
    template<typename IteratorInterface1, typename IteratorInterface2>
    constexpr auto
    operator>=(IteratorInterface1 lhs, IteratorInterface2 rhs) noexcept(
        noexcept(detail::common_diff(lhs, rhs)))
        -> decltype(
            v1_dtl::derived_iterator(lhs), detail::common_diff(lhs, rhs) >= 0)
    {
        return detail::common_diff(lhs, rhs) >= 0;
    }


    /** A template alias useful for defining proxy iterators.  \see
        `iterator_interface`. */
    template<
        typename Derived,
        typename IteratorConcept,
        typename ValueType,
        typename Reference = ValueType,
        typename DifferenceType = std::ptrdiff_t>
    using proxy_iterator_interface = iterator_interface<
        Derived,
        IteratorConcept,
        ValueType,
        Reference,
        proxy_arrow_result<Reference>,
        DifferenceType>;

}}}

#if defined(BOOST_STL_INTERFACES_DOXYGEN) || BOOST_STL_INTERFACES_USE_CONCEPTS

namespace boost { namespace stl_interfaces { BOOST_STL_INTERFACES_NAMESPACE_V2 {

    namespace v2_dtl {
        template<typename Iterator>
        struct iter_concept;

        template<typename Iterator>
        requires requires
        {
            typename std::iterator_traits<Iterator>::iterator_concept;
        }
        struct iter_concept<Iterator>
        {
            using type =
                typename std::iterator_traits<Iterator>::iterator_concept;
        };

        template<typename Iterator>
        requires(
            !requires {
                typename std::iterator_traits<Iterator>::iterator_concept;
            } &&
            requires {
                typename std::iterator_traits<Iterator>::iterator_category;
            })
	struct iter_concept<Iterator>
        {
            using type =
                typename std::iterator_traits<Iterator>::iterator_category;
        };

        template<typename Iterator>
        requires(
            !requires {
                typename std::iterator_traits<Iterator>::iterator_concept;
            } &&
            !requires {
                typename std::iterator_traits<Iterator>::iterator_category;
            })
	struct iter_concept<Iterator>
        {
            using type = std::random_access_iterator_tag;
        };

        template<typename Iterator>
        struct iter_concept
        {};

        template<typename Iterator>
        using iter_concept_t = typename iter_concept<Iterator>::type;

        template<typename D, typename DifferenceType>
        // clang-format off
        concept plus_eq = requires (D d) { d += DifferenceType(1); };
        // clang-format on

        template<typename D, typename D2 = D>
        // clang-format off
        concept base_3way =
#if defined(__cpp_impl_three_way_comparison)
            requires (D d, D2 d2) { access::base(d) <=> access::base(d2); };
#else
            false;
#endif
        // clang-format on

        template<typename D1, typename D2 = D1>
        // clang-format off
        concept base_eq =
            requires (D1 d1, D2 d2) { access::base(d1) == access::base(d2); };
        // clang-format on

        template<typename D, typename D2 = D>
        // clang-format off
        concept iter_sub = requires (D d, D2 d2) {
            typename D::difference_type;
            {d - d2} -> std::convertible_to<typename D::difference_type>;
        };
        // clang-format on

        // This iterator concept -> category mapping scheme follows the one
        // from zip_transform_view; see
        // https://eel.is/c++draft/range.zip.transform.iterator#1.

        template<typename IteratorConcept, typename ReferenceType>
        constexpr auto category_tag()
        {
            if constexpr (std::is_base_of_v<
                              std::forward_iterator_tag,
                              IteratorConcept>) {
                if constexpr (!std::is_reference_v<ReferenceType>) {
                    return std::input_iterator_tag{};
                } else if constexpr (std::is_base_of_v<
                                         std::random_access_iterator_tag,
                                         IteratorConcept>) {
                    return std::random_access_iterator_tag{};
                } else if constexpr (std::is_base_of_v<
                                         std::bidirectional_iterator_tag,
                                         IteratorConcept>) {
                    return std::bidirectional_iterator_tag{};
                } else {
                    return std::forward_iterator_tag{};
                }
            } else {
                return 0; // int means "no tag"
            }
        }
        template<
            typename IteratorConcept,
            typename ReferenceType,
            typename IteratorCategory =
                decltype(v2_dtl::
                             category_tag<IteratorConcept, ReferenceType>())>
        struct iterator_category_base
        {
            using iterator_category = IteratorCategory;
        };

        template<typename IteratorConcept, typename ReferenceType>
        struct iterator_category_base<IteratorConcept, ReferenceType, int>
        {};

        template<typename IteratorConcept, typename ReferenceType>
        constexpr bool non_input_tag()
        {
            if (std::same_as<IteratorConcept, std::input_iterator_tag>)
                return false;
            using tag_t =
                decltype(v2_dtl::
                             category_tag<IteratorConcept, ReferenceType>());
            return !std::same_as<tag_t, std::input_iterator_tag>;
        }
    }

    // clang-format off

    /** A CRTP template that one may derive from to make defining iterators
        easier.

        The template parameter `D` for `iterator_interface` may be an
        incomplete type.  Before any member of the resulting specialization of
        `iterator_interface` other than special member functions is
        referenced, `D` shall be complete, and model
        `std::derived_from<iterator_interface<D>>`. */
    template<
      typename D,
      typename IteratorConcept,
      typename ValueType,
      typename Reference = ValueType &,
      typename Pointer = ValueType *,
      typename DifferenceType = std::ptrdiff_t>
      requires std::is_class_v<D> && std::same_as<D, std::remove_cv_t<D>>
    struct iterator_interface
        : v2_dtl::iterator_category_base<IteratorConcept, Reference>
    {
    private:
      constexpr D& derived() noexcept {
        return static_cast<D&>(*this);
      }
      constexpr const D& derived() const noexcept {
        return static_cast<const D&>(*this);
      }

    public:
      using iterator_concept = IteratorConcept;
      using value_type = std::remove_const_t<ValueType>;
      using reference = Reference;
      using pointer = detail::pointer_t<Pointer, iterator_concept>;
      using difference_type = DifferenceType;

      constexpr decltype(auto) operator*()
        requires requires (D d) { *access::base(d); } {
          return *access::base(derived());
        }
      constexpr decltype(auto) operator*() const
        requires requires (D const d) { *access::base(d); } {
          return *access::base(derived());
        }

      constexpr auto operator->()
        requires (!std::same_as<pointer, void> && std::is_reference_v<reference> &&
                  requires (D d) { *d; }) {
          return detail::make_pointer<pointer, reference>(*derived());
        }
      constexpr auto operator->() const
        requires (!std::same_as<pointer, void> && std::is_reference_v<reference> &&
                  requires (D const d) { *d; }) {
          return detail::make_pointer<pointer, reference>(*derived());
        }

      constexpr decltype(auto) operator[](difference_type n) const
        requires requires (D const d) { d + n; } {
        D retval = derived();
        retval += n;
        return *retval;
      }

      constexpr decltype(auto) operator++()
        requires requires (D d) { ++access::base(d); } &&
          (!v2_dtl::plus_eq<D, difference_type>) {
            ++access::base(derived());
            return derived();
          }
      constexpr decltype(auto) operator++()
        requires requires (D d) { d += difference_type(1); } {
          return derived() += difference_type(1);
        }
      constexpr auto operator++(int) requires requires (D d) { ++d; } {
        if constexpr (std::is_same_v<IteratorConcept, std::input_iterator_tag>){
          ++derived();
        } else {
          D retval = derived();
          ++derived();
          return retval;
        }
      }
      constexpr decltype(auto) operator+=(difference_type n)
        requires requires (D d) { access::base(d) += n; } {
          access::base(derived()) += n;
          return derived();
        }

      constexpr decltype(auto) operator--()
        requires requires (D d) { --access::base(d); } &&
          (!v2_dtl::plus_eq<D, difference_type>) {
            --access::base(derived());
            return derived();
          }
      constexpr decltype(auto) operator--()
        requires requires (D d) { d += -difference_type(1); } {
          return derived() += -difference_type(1);
        }
      constexpr auto operator--(int) requires requires (D d) { --d; } {
        D retval = derived();
        --derived();
        return retval;
      }
      constexpr decltype(auto) operator-=(difference_type n)
        requires requires (D d) { d += -n; } {
          return derived() += -n;
        }
    };

    namespace v2_dtl {
        template<
            typename D,
            typename IteratorConcept,
            typename ValueType,
            typename Reference,
            typename Pointer,
            typename DifferenceType>
        void derived_iterator(v2::iterator_interface<
                              D,
                              IteratorConcept,
                              ValueType,
                              Reference,
                              Pointer,
                              DifferenceType> const &);

        template<typename D>
        concept derived_iter = requires (D d) { v2_dtl::derived_iterator(d); };
    }

    template<typename D>
      constexpr auto operator+(D it, typename D::difference_type n)
        requires v2_dtl::derived_iter<D> && requires { it += n; }
          { return it += n; }
    template<typename D>
      constexpr auto operator+(typename D::difference_type n, D it)
        requires v2_dtl::derived_iter<D> && requires { it += n; }
          { return it += n; }

    template<typename D1, typename D2>
      constexpr auto operator-(D1 lhs, D2 rhs)
        requires v2_dtl::derived_iter<D1> && v2_dtl::derived_iter<D2> &&
                 requires { access::base(lhs) - access::base(rhs); }
          { return access::base(lhs) - access::base(rhs); }
    template<typename D>
      constexpr auto operator-(D it, typename D::difference_type n)
        requires v2_dtl::derived_iter<D> && requires { it += -n; }
          { return it += -n; }

#if defined(__cpp_lib_three_way_comparison)
    template<typename D1, typename D2>
      constexpr auto operator<=>(D1 lhs, D2 rhs)
        requires v2_dtl::derived_iter<D1> && v2_dtl::derived_iter<D2> &&
        (v2_dtl::base_3way<D1, D2> || v2_dtl::iter_sub<D1, D2>) {
        if constexpr (v2_dtl::base_3way<D1, D2>) {
            return access::base(lhs) <=> access::base(rhs);
          } else {
            using diff_type = typename D1::difference_type;
            diff_type const diff = lhs - rhs;
            return diff < diff_type(0) ? std::strong_ordering::less :
              diff_type(0) < diff ? std::strong_ordering::greater :
              std::strong_ordering::equal;
          }
        }
#endif
    template<typename D1, typename D2>
      constexpr bool operator<(D1 lhs, D2 rhs)
        requires v2_dtl::derived_iter<D1> && v2_dtl::derived_iter<D2> && v2_dtl::iter_sub<D1, D2>
          { return (lhs - rhs) < typename D1::difference_type(0); }
    template<typename D1, typename D2>
      constexpr bool operator<=(D1 lhs, D2 rhs)
        requires v2_dtl::derived_iter<D1> && v2_dtl::derived_iter<D2> && v2_dtl::iter_sub<D1, D2>
          { return (lhs - rhs) <= typename D1::difference_type(0); }
    template<typename D1, typename D2>
      constexpr bool operator>(D1 lhs, D2 rhs)
        requires v2_dtl::derived_iter<D1> && v2_dtl::derived_iter<D2> && v2_dtl::iter_sub<D1, D2>
          { return (lhs - rhs) > typename D1::difference_type(0); }
    template<typename D1, typename D2>
      constexpr bool operator>=(D1 lhs, D2 rhs)
        requires v2_dtl::derived_iter<D1> && v2_dtl::derived_iter<D2> && v2_dtl::iter_sub<D1, D2>
          { return (lhs - rhs) >= typename D1::difference_type(0); }

    template<typename D1, typename D2>
      constexpr bool operator==(D1 lhs, D2 rhs)
        requires v2_dtl::derived_iter<D1> && v2_dtl::derived_iter<D2> &&
                 detail::interoperable<D1, D2>::value &&
        (v2_dtl::base_eq<D1, D2> || v2_dtl::iter_sub<D1, D2>) {
        if constexpr (v2_dtl::base_eq<D1, D2>) {
          return (access::base(lhs) == access::base(rhs));
        } else if constexpr (v2_dtl::iter_sub<D1, D2>) {
          return (lhs - rhs) == typename D1::difference_type(0);
        }
      }

    template<typename D1, typename D2>
      constexpr auto operator!=(D1 lhs, D2 rhs) -> decltype(!(lhs == rhs))
        requires v2_dtl::derived_iter<D1> && v2_dtl::derived_iter<D2>
          { return !(lhs == rhs); }

    // clang-format on


    /** A template alias useful for defining proxy iterators.  \see
        `iterator_interface`. */
    template<
        typename Derived,
        typename IteratorConcept,
        typename ValueType,
        typename Reference = ValueType,
        typename DifferenceType = std::ptrdiff_t>
    using proxy_iterator_interface = iterator_interface<
        Derived,
        IteratorConcept,
        ValueType,
        Reference,
        proxy_arrow_result<Reference>,
        DifferenceType>;

}}}

#endif

#if defined(BOOST_STL_INTERFACES_DOXYGEN) || BOOST_STL_INTERFACES_USE_DEDUCED_THIS

namespace boost { namespace stl_interfaces { BOOST_STL_INTERFACES_NAMESPACE_V3 {

    // clang-format off

    /** A base template that one may derive from to make defining iterators
        easier. */
    template<
      typename IteratorConcept,
      typename ValueType,
      typename Reference = ValueType &,
      typename Pointer = ValueType *,
      typename DifferenceType = std::ptrdiff_t>
    struct iterator_interface
        : v2::v2_dtl::iterator_category_base<IteratorConcept, Reference>
    {
      using iterator_concept = IteratorConcept;
      using value_type = std::remove_const_t<ValueType>;
      using reference = Reference;
      using pointer = detail::pointer_t<Pointer, iterator_concept>;
      using difference_type = DifferenceType;

      constexpr decltype(auto) operator*(this auto&& self)
          requires requires { *access::base(self); } {
          return *access::base(self);
      }

      constexpr auto operator->(this auto&& self)
        requires (!std::same_as<pointer, void>) && std::is_reference_v<reference> && requires { *self; } {
          return detail::make_pointer<pointer, reference>(*self);
        }

      constexpr decltype(auto) operator[](this auto const& self, difference_type n)
        requires requires { self + n; } {
        auto retval = self;
        retval = retval + n;
        return *retval;
      }

      constexpr decltype(auto) operator++(this auto& self)
        requires requires { ++access::base(self); } && (!requires { self += difference_type(1); }) {
          ++access::base(self);
          return self;
        }
      constexpr decltype(auto) operator++(this auto& self)
        requires requires { self += difference_type(1); } {
          return self += difference_type(1);
        }
      constexpr auto operator++(this auto& self, int) requires requires { ++self; } {
        if constexpr (std::is_same_v<IteratorConcept, std::input_iterator_tag>){
          ++self;
        } else {
          auto retval = self;
          ++self;
          return retval;
        }
      }
      constexpr decltype(auto) operator+=(this auto& self, difference_type n)
        requires requires { access::base(self) += n; } {
          access::base(self) += n;
          return self;
        }

      constexpr decltype(auto) operator--(this auto& self)
          requires requires { --access::base(self); } && (!requires { self += difference_type(1); }) {
            --access::base(self);
            return self;
          }
      constexpr decltype(auto) operator--(this auto& self)
        requires requires { self += -difference_type(1); } {
          return self += -difference_type(1);
        }
      constexpr auto operator--(this auto& self, int) requires requires { --self; } {
        auto retval = self;
        --self;
        return retval;
      }
      constexpr decltype(auto) operator-=(this auto& self, difference_type n)
        requires requires { self += -n; } {
          return self += -n;
        }
    };

    namespace v3_dtl {
        template<
            typename IteratorConcept,
            typename ValueType,
            typename Reference,
            typename Pointer,
            typename DifferenceType>
        void derived_iterator(v3::iterator_interface<
                              IteratorConcept,
                              ValueType,
                              Reference,
                              Pointer,
                              DifferenceType> const &);

        template<typename D>
        concept derived_iter = requires (D d) { v3_dtl::derived_iterator(d); };
    }

    template<typename D>
      constexpr auto operator+(D it, typename D::difference_type n)
        requires v3_dtl::derived_iter<D> && requires { it += n; }
          { return it += n; }
    template<typename D>
      constexpr auto operator+(typename D::difference_type n, D it)
        requires v3_dtl::derived_iter<D> && requires { it += n; }
          { return it += n; }

    template<typename D1, typename D2>
      constexpr auto operator-(D1 lhs, D2 rhs)
        requires v3_dtl::derived_iter<D1> && v3_dtl::derived_iter<D2> &&
                 requires { access::base(lhs) - access::base(rhs); }
          { return access::base(lhs) - access::base(rhs); }
    template<typename D>
      constexpr auto operator-(D it, typename D::difference_type n)
        requires v3_dtl::derived_iter<D> && requires { it += -n; }
          { return it += -n; }

#if defined(__cpp_lib_three_way_comparison)
    template<typename D1, typename D2>
      constexpr auto operator<=>(D1 lhs, D2 rhs)
        requires v3_dtl::derived_iter<D1> && v3_dtl::derived_iter<D2> &&
        (v2::v2_dtl::base_3way<D1, D2> || v2::v2_dtl::iter_sub<D1, D2>) {
        if constexpr (v2::v2_dtl::base_3way<D1, D2>) {
            return access::base(lhs) <=> access::base(rhs);
          } else {
            using diff_type = typename D1::difference_type;
            diff_type const diff = lhs - rhs;
            return diff < diff_type(0) ? std::strong_ordering::less :
              diff_type(0) < diff ? std::strong_ordering::greater :
              std::strong_ordering::equal;
          }
        }
#endif
    template<typename D1, typename D2>
      constexpr bool operator<(D1 lhs, D2 rhs)
        requires v3_dtl::derived_iter<D1> && v3_dtl::derived_iter<D2> && v2::v2_dtl::iter_sub<D1, D2>
          { return (lhs - rhs) < typename D1::difference_type(0); }
    template<typename D1, typename D2>
      constexpr bool operator<=(D1 lhs, D2 rhs)
        requires v3_dtl::derived_iter<D1> && v3_dtl::derived_iter<D2> && v2::v2_dtl::iter_sub<D1, D2>
          { return (lhs - rhs) <= typename D1::difference_type(0); }
    template<typename D1, typename D2>
      constexpr bool operator>(D1 lhs, D2 rhs)
        requires v3_dtl::derived_iter<D1> && v3_dtl::derived_iter<D2> && v2::v2_dtl::iter_sub<D1, D2>
          { return (lhs - rhs) > typename D1::difference_type(0); }
    template<typename D1, typename D2>
      constexpr bool operator>=(D1 lhs, D2 rhs)
        requires v3_dtl::derived_iter<D1> && v3_dtl::derived_iter<D2> && v2::v2_dtl::iter_sub<D1, D2>
          { return (lhs - rhs) >= typename D1::difference_type(0); }

    template<typename D1, typename D2>
      constexpr bool operator==(D1 lhs, D2 rhs)
        requires v3_dtl::derived_iter<D1> && v3_dtl::derived_iter<D2> &&
                 detail::interoperable<D1, D2>::value &&
        (v2::v2_dtl::base_eq<D1, D2> || v2::v2_dtl::iter_sub<D1, D2>) {
        if constexpr (v2::v2_dtl::base_eq<D1, D2>) {
          return (access::base(lhs) == access::base(rhs));
        } else if constexpr (v2::v2_dtl::iter_sub<D1, D2>) {
          return (lhs - rhs) == typename D1::difference_type(0);
        }
      }

    template<typename D1, typename D2>
      constexpr auto operator!=(D1 lhs, D2 rhs) -> decltype(!(lhs == rhs))
        requires v3_dtl::derived_iter<D1> && v3_dtl::derived_iter<D2>
          { return !(lhs == rhs); }

    // clang-format on


    /** A template alias useful for defining proxy iterators.  \see
        `iterator_interface`. */
    template<
        typename IteratorConcept,
        typename ValueType,
        typename Reference = ValueType,
        typename DifferenceType = std::ptrdiff_t>
    using proxy_iterator_interface = iterator_interface<
        IteratorConcept,
        ValueType,
        Reference,
        proxy_arrow_result<Reference>,
        DifferenceType>;

}}}

#endif

#ifdef BOOST_STL_INTERFACES_DOXYGEN

/** `static_asserts` that type `type` models concept `concept_name`.  This is
    useful for checking that an iterator, view, etc. that you write using one
    of the *`_interface` templates models the right C++ concept.

    For example: `BOOST_STL_INTERFACES_STATIC_ASSERT_CONCEPT(my_iter,
    std::input_iterator)`.

    \note This macro expands to nothing when `__cpp_lib_concepts` is not
    defined. */
#define BOOST_STL_INTERFACES_STATIC_ASSERT_CONCEPT(type, concept_name)

/** `static_asserts` that the types of all typedefs in
    `std::iterator_traits<iter>` match the remaining macro parameters.  This
    is useful for checking that an iterator you write using
    `iterator_interface` has the correct iterator traits.

    For example: `BOOST_STL_INTERFACES_STATIC_ASSERT_ITERATOR_TRAITS(my_iter,
    std::input_iterator_tag, std::input_iterator, int, int &, int *, std::ptrdiff_t)`.

    \note This macro ignores the `concept` parameter when `__cpp_lib_concepts`
    is not defined. */
#define BOOST_STL_INTERFACES_STATIC_ASSERT_ITERATOR_TRAITS(                    \
    iter, category, concept, value_type, reference, pointer, difference_type)

#else

#define BOOST_STL_INTERFACES_STATIC_ASSERT_ITERATOR_CONCEPT_IMPL(              \
    type, concept_name)                                                        \
    static_assert(concept_name<type>, "");

#if BOOST_STL_INTERFACES_USE_CONCEPTS
#define BOOST_STL_INTERFACES_STATIC_ASSERT_CONCEPT(iter, concept_name)         \
    BOOST_STL_INTERFACES_STATIC_ASSERT_ITERATOR_CONCEPT_IMPL(iter, concept_name)
#else
#define BOOST_STL_INTERFACES_STATIC_ASSERT_CONCEPT(iter, concept_name)
#endif

#define BOOST_STL_INTERFACES_STATIC_ASSERT_ITERATOR_TRAITS_IMPL(               \
    iter, category, value_t, ref, ptr, diff_t)                                 \
    static_assert(                                                             \
        std::is_same<                                                          \
            typename std::iterator_traits<iter>::value_type,                   \
            value_t>::value,                                                   \
        "");                                                                   \
    static_assert(                                                             \
        std::is_same<typename std::iterator_traits<iter>::reference, ref>::    \
            value,                                                             \
        "");                                                                   \
    static_assert(                                                             \
        std::is_same<typename std::iterator_traits<iter>::pointer, ptr>::      \
            value,                                                             \
        "");                                                                   \
    static_assert(                                                             \
        std::is_same<                                                          \
            typename std::iterator_traits<iter>::difference_type,              \
            diff_t>::value,                                                    \
        "");

#define BOOST_STL_INTERFACES_STATIC_ASSERT_ITERATOR_TRAITS(                    \
    iter, category, concept, value_type, reference, pointer, difference_type)  \
    BOOST_STL_INTERFACES_STATIC_ASSERT_ITERATOR_TRAITS_IMPL(                   \
        iter, category, value_type, reference, pointer, difference_type)
#endif

#endif
