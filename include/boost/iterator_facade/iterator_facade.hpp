// Copyright (C) 2019 T. Zachary Laine
//
// Distributed under the Boost Software License, Version 1.0. (See
// accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
#ifndef BOOST_ITERATOR_FACADE_ITERATOR_FACADE_HPP
#define BOOST_ITERATOR_FACADE_ITERATOR_FACADE_HPP

#include <utility>
#include <iterator>
#include <type_traits>


#ifndef BOOST_ITERATOR_FACADE_DOXYGEN

#if defined(_MSC_VER) || defined(__GNUC__) && __GNUC__ < 8
#define BOOST_ITERATOR_FACADE_HIDDEN_FRIEND_CONSTEXPR
#else
#define BOOST_ITERATOR_FACADE_HIDDEN_FRIEND_CONSTEXPR constexpr
#endif

#endif


namespace boost { namespace iterator_facade {

    /** The return type of `operator->()` in a proxy iterator.

        This template is used as the default `Pointer` template parameter in
        the `proxy_iterator_facade` template alias.  Note that the use of this
        template implies a copy or move of the underlying object of type
        `T`. */
    template<typename T>
    struct proxy_arrow_result
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
        template<typename Pointer, typename T>
        struct ptr_and_ref : std::is_same<
                                 std::add_pointer_t<std::remove_reference_t<T>>,
                                 Pointer>
        {
        };

        template<typename Pointer, typename T>
        auto make_pointer(
            T && value,
            std::enable_if_t<ptr_and_ref<Pointer, T>::value, int> = 0)
        {
            return std::addressof(value);
        }

        template<typename Pointer, typename T>
        auto make_pointer(
            T && value,
            std::enable_if_t<!ptr_and_ref<Pointer, T>::value, int> = 0)
        {
            return Pointer((T &&) value);
        }

        template<typename IteratorConcept>
        struct concept_category
        {
            using type = IteratorConcept;
        };
#if 201703L < __cplusplus && defined(__cpp_lib_ranges)
        template<>
        struct concept_category<std::contiguous_iterator_tag>
        {
            using type = std::random_access_iterator_tag;
        };
#endif
        template<typename IteratorConcept>
        using concept_category_t =
            typename concept_category<IteratorConcept>::type;

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

        template<typename... T>
        using void_t = void;
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
    }

    // TODO: Compile-fail tests and associated static_asserts to help catch
    // common errors.

    /** A CRTP template that one may derive from to make defining iterators
        easier. */
    template<
        typename Derived,
        typename IteratorConcept,
        typename ValueType,
        typename Reference = ValueType &,
        typename Pointer = ValueType *,
        typename DifferenceType = std::ptrdiff_t>
    struct iterator_interface
    {
    private:
        constexpr Derived & derived() noexcept
        {
            return static_cast<Derived &>(*this);
        }
        constexpr Derived const & derived() const noexcept
        {
            return static_cast<Derived const &>(*this);
        }

        constexpr static DifferenceType one = DifferenceType(1);

    public:
        using iterator_concept = IteratorConcept;
        using iterator_category = detail::concept_category_t<iterator_concept>;
        using value_type = ValueType;
        using reference = Reference;
        using pointer = detail::pointer_t<Pointer, iterator_concept>;
        using difference_type = DifferenceType;

        using derived_iterator_type = Derived;

        template<typename D = Derived>
        constexpr pointer operator->() const noexcept(
            noexcept(detail::make_pointer<pointer>(*std::declval<D &>())))
        {
            return detail::make_pointer<pointer>(*derived());
        }

        template<typename D = Derived>
        constexpr reference operator[](difference_type i) const
            noexcept(noexcept(
                D(std::declval<D &>()),
                std::declval<D &>() += i,
                *std::declval<D &>()))
        {
            D copy = derived();
            copy += i;
            return *copy;
        }

        template<typename D = Derived>
        constexpr auto
        operator++() noexcept(noexcept(std::declval<D &>().next()))
            -> decltype(std::declval<D &>().next(), std::declval<D &>())
        {
            derived().next();
            return derived();
        }
        template<typename D = Derived>
        constexpr auto
        operator++() noexcept(noexcept(std::declval<D &>() += one))
            -> decltype(std::declval<D &>() += one, std::declval<D &>())
        {
            derived() += one;
            return derived();
        }

        template<typename D = Derived>
        constexpr auto
        operator++(int)noexcept(noexcept(std::declval<D &>().next()))
            -> std::remove_reference_t<decltype(
                D(std::declval<D &>()),
                std::declval<D &>().next(),
                std::declval<D &>())>
        {
            D retval = derived();
            derived().next();
            return retval;
        }
        template<typename D = Derived>
        constexpr auto
        operator++(int)noexcept(noexcept(std::declval<D &>() += one))
            -> std::remove_reference_t<decltype(
                D(std::declval<D &>()),
                std::declval<D &>() += one,
                std::declval<D &>())>
        {
            D retval = derived();
            derived() += one;
            return retval;
        }

        template<typename D = Derived>
        constexpr D operator+(difference_type i) noexcept(
            noexcept(D(std::declval<D &>()), std::declval<D &>() += i))
        {
            D copy = derived();
            copy += i;
            return copy;
        }
        friend BOOST_ITERATOR_FACADE_HIDDEN_FRIEND_CONSTEXPR Derived
        operator+(difference_type i, Derived it) noexcept(noexcept(it + i))
        {
            return it + i;
        }

        template<typename D = Derived>
        constexpr auto
        operator--() noexcept(noexcept(std::declval<D &>().prev()))
            -> decltype(std::declval<D &>().prev(), std::declval<D &>())
        {
            derived().prev();
            return derived();
        }
        template<typename D = Derived>
        constexpr auto
        operator--() noexcept(noexcept(std::declval<D &>() += -one))
            -> decltype(std::declval<D &>() += -one, std::declval<D &>())
        {
            derived() += -one;
            return derived();
        }

        template<typename D = Derived>
        constexpr auto
        operator--(int)noexcept(noexcept(std::declval<D &>().prev()))
            -> std::remove_reference_t<decltype(
                D(std::declval<D &>()),
                std::declval<D &>().prev(),
                std::declval<D &>())>
        {
            D retval = derived();
            derived().prev();
            return retval;
        }
        template<typename D = Derived>
        constexpr auto
        operator--(int)noexcept(noexcept(std::declval<D &>() += -one))
            -> std::remove_reference_t<decltype(
                D(std::declval<D &>()),
                std::declval<D &>() += -one,
                std::declval<D &>())>
        {
            D retval = derived();
            derived() += -one;
            return retval;
        }

        template<typename D = Derived>
        constexpr D & operator-=(difference_type i) noexcept(
            noexcept(std::declval<D &>() += -i))
        {
            derived() += -i;
            return derived();
        }

        friend BOOST_ITERATOR_FACADE_HIDDEN_FRIEND_CONSTEXPR Derived operator-(
            Derived it,
            difference_type i) noexcept(noexcept(Derived(it), it += -i))
        {
            Derived copy = it;
            copy += -i;
            return copy;
        }
    };

    /** Implementation of `operator!=()` for all iterators derived from
        `iterator_interface`, except those with an iterator category derived
        from `std::random_access_iterator_tag`.  */
    template<
        typename IteratorInterface,
        typename Enable =
            std::enable_if_t<!detail::ra_iter<IteratorInterface>::value>>
    constexpr auto operator!=(
        IteratorInterface lhs,
        IteratorInterface rhs) noexcept(noexcept(lhs == rhs))
        -> decltype(
            typename IteratorInterface::derived_iterator_type(), lhs == rhs)
    {
        return !(lhs == rhs);
    }

    /** Implementation of `operator==()` for all iterators derived from
        `iterator_interface` that have an iterator category derived from
        `std::random_access_iterator_tag`.  */
    template<typename IteratorInterface>
    constexpr auto operator==(
        IteratorInterface lhs,
        IteratorInterface rhs) noexcept(noexcept(lhs - rhs))
        -> decltype(
            typename IteratorInterface::derived_iterator_type(), lhs - rhs)
    {
        return (lhs - rhs) == 0;
    }

    /** Implementation of `operator!=()` for all iterators derived from
        `iterator_interface` that have an iterator category derived from
        `std::random_access_iterator_tag`.  */
    template<
        typename IteratorInterface,
        typename Enable =
            std::enable_if_t<detail::ra_iter<IteratorInterface>::value>>
    constexpr auto operator!=(
        IteratorInterface lhs,
        IteratorInterface rhs) noexcept(noexcept(lhs - rhs))
        -> decltype(
            typename IteratorInterface::derived_iterator_type(), lhs - rhs)
    {
        return (lhs - rhs) != 0;
    }

    /** Implementation of `operator<()` for all iterators derived from
        `iterator_interface` that have an iterator category derived from
        `std::random_access_iterator_tag`.  */
    template<typename IteratorInterface>
    constexpr auto operator<(
        IteratorInterface lhs,
        IteratorInterface rhs) noexcept(noexcept(lhs - rhs))
        -> decltype(
            typename IteratorInterface::derived_iterator_type(), lhs - rhs)
    {
        return (lhs - rhs) < 0;
    }

    /** Implementation of `operator<=()` for all iterators derived from
        `iterator_interface` that have an iterator category derived from
        `std::random_access_iterator_tag`.  */
    template<typename IteratorInterface>
    constexpr auto operator<=(
        IteratorInterface lhs,
        IteratorInterface rhs) noexcept(noexcept(lhs - rhs))
        -> decltype(
            typename IteratorInterface::derived_iterator_type(), lhs - rhs)
    {
        return (lhs - rhs) <= 0;
    }

    /** Implementation of `operator>()` for all iterators derived from
        `iterator_interface` that have an iterator category derived from
        `std::random_access_iterator_tag`.  */
    template<typename IteratorInterface>
    constexpr auto operator>(
        IteratorInterface lhs,
        IteratorInterface rhs) noexcept(noexcept(lhs - rhs))
        -> decltype(
            typename IteratorInterface::derived_iterator_type(), lhs - rhs)
    {
        return (lhs - rhs) > 0;
    }

    /** Implementation of `operator>=()` for all iterators derived from
        `iterator_interface` that have an iterator category derived from
        `std::random_access_iterator_tag`.  */
    template<typename IteratorInterface>
    constexpr auto operator>=(
        IteratorInterface lhs,
        IteratorInterface rhs) noexcept(noexcept(lhs - rhs))
        -> decltype(
            typename IteratorInterface::derived_iterator_type(), lhs - rhs)
    {
        return (lhs - rhs) >= 0;
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

}}


#ifdef BOOST_ITERATOR_FACADE_DOXYGEN

/** `static_asserts` that iterator type `iter` models concept `concept_name`.
    This is useful for checking that an iterator you write using
    `iterator_facade` models the right C++ concept.

    For example: `BOOST_ITERATOR_FACADE_STATIC_ASSERT_CONCEPT(my_iter,
    std::input_iterator)`.

    \note This macro exapnds to nothing when `__cpp_lib_concepts` is not
    defined. */
#define BOOST_ITERATOR_FACADE_STATIC_ASSERT_CONCEPT(iter, concept_name)

/** `static_asserts` that the types of all typedefs in
    `std::iterator_traits<iter>` match the remaining macro parameters.  This
    is useful for checking that an iterator you write using `iterator_facade`
    has the correct iterator traits.

    For example: `BOOST_ITERATOR_FACADE_STATIC_ASSERT_ITERATOR_TRAITS(my_iter,
    std::input_iterator_tag, std::input_iterator_tag, int, int &, int *, std::ptrdiff_t)`.

    \note This macro ignores the `concept` parameter when `__cpp_lib_concepts`
    is not defined. */
#define BOOST_ITERATOR_FACADE_STATIC_ASSERT_ITERATOR_TRAITS(                   \
    iter, category, concept, value_type, reference, pointer, difference_type)

#else

#define BOOST_ITERATOR_FACADE_STATIC_ASSERT_ITERATOR_CONCEPT_IMPL(             \
    iter, concept_name)                                                        \
    static_assert(concept_name<iter>, "");

#if 201703L < __cplusplus && defined(__cpp_lib_concepts)
#define BOOST_ITERATOR_FACADE_STATIC_ASSERT_CONCEPT(iter, concept_name)        \
    BOOST_ITERATOR_FACADE_STATIC_ASSERT_ITERATOR_CONCEPT_IMPL(                 \
        iter, concept_name)
#else
#define BOOST_ITERATOR_FACADE_STATIC_ASSERT_CONCEPT(iter, concept_name)
#endif

#define BOOST_ITERATOR_FACADE_STATIC_ASSERT_ITERATOR_TRAITS_IMPL(              \
    iter, category, value_t, ref, ptr, diff_t)                                 \
    static_assert(                                                             \
        std::is_same<                                                          \
            typename std::iterator_traits<iter>::iterator_category,            \
            category>::value,                                                  \
        "");                                                                   \
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

#if 201703L < __cplusplus && defined(__cpp_lib_ranges)
#define BOOST_ITERATOR_FACADE_STATIC_ASSERT_ITERATOR_TRAITS(                   \
    iter, category, concept, value_type, reference, pointer, difference_type)  \
    static_assert(                                                             \
        std::is_same<                                                          \
            typename std::iterator_traits<iter>::iterator_concept,             \
            concept>::value,                                                   \
        "");                                                                   \
    BOOST_ITERATOR_FACADE_STATIC_ASSERT_ITERATOR_TRAITS_IMPL(                  \
        iter, category, value_type, reference, pointer, difference_type)
#else
#define BOOST_ITERATOR_FACADE_STATIC_ASSERT_ITERATOR_TRAITS(                   \
    iter, category, concept, value_type, reference, pointer, difference_type)  \
    BOOST_ITERATOR_FACADE_STATIC_ASSERT_ITERATOR_TRAITS_IMPL(                  \
        iter, category, value_type, reference, pointer, difference_type)
#endif

#endif

#endif
