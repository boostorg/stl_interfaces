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

#if defined(_MSC_VER)
#define BOOST_ITERATOR_FACADE_HIDDEN_FRIEND_CONSTEXPR
#else
#define BOOST_ITERATOR_FACADE_HIDDEN_FRIEND_CONSTEXPR constexpr
#endif

#endif


namespace boost { namespace iterator_facade {

    /** A CRTP template that one may derive from to make defining iterators
        easier. */
    template<
        typename Derived,
        typename IteratorConcept,
        typename ValueType,
        typename Reference = ValueType &,
        typename Pointer = ValueType *,
        typename DifferenceType = std::ptrdiff_t>
    struct iterator_facade;

    /** A type for granting access to the private members of an iterator
        derived from `iterator_facade`. */
    struct access
    {
        template<
            typename Derived,
            typename IteratorConcept,
            typename ValueType,
            typename Reference,
            typename Pointer,
            typename DifferenceType>
        constexpr static Derived & derived(iterator_facade<
                                           Derived,
                                           IteratorConcept,
                                           ValueType,
                                           Reference,
                                           Pointer,
                                           DifferenceType> & if_) noexcept
        {
            return static_cast<Derived &>(if_);
        }
        template<
            typename Derived,
            typename IteratorConcept,
            typename ValueType,
            typename Reference,
            typename Pointer,
            typename DifferenceType>
        constexpr static Derived const &
        derived(iterator_facade<
                Derived,
                IteratorConcept,
                ValueType,
                Reference,
                Pointer,
                DifferenceType> const & if_) noexcept
        {
            return static_cast<Derived const &>(if_);
        }


        template<
            typename Derived,
            typename IteratorConcept,
            typename ValueType,
            typename Reference,
            typename Pointer,
            typename DifferenceType>
        constexpr static decltype(auto)
        dereference(iterator_facade<
                    Derived,
                    IteratorConcept,
                    ValueType,
                    Reference,
                    Pointer,
                    DifferenceType> const &
                        it) noexcept(noexcept(derived(it).dereference()))
        {
            return derived(it).dereference();
        }

        // This overload takes a non-`const` facade parameter, to support
        // output iterators like `std::back_insert_iterator`.
        template<
            typename Derived,
            typename IteratorConcept,
            typename ValueType,
            typename Reference,
            typename Pointer,
            typename DifferenceType>
        constexpr static decltype(auto)
        dereference(iterator_facade<
                    Derived,
                    IteratorConcept,
                    ValueType,
                    Reference,
                    Pointer,
                    DifferenceType> & it) noexcept(noexcept(derived(it)
                                                                .dereference()))
        {
            return derived(it).dereference();
        }

        template<
            typename Derived,
            typename IteratorConcept,
            typename ValueType,
            typename Reference,
            typename Pointer,
            typename DifferenceType>
        constexpr static auto equals(
            iterator_facade<
                Derived,
                IteratorConcept,
                ValueType,
                Reference,
                Pointer,
                DifferenceType> const & lhs,
            iterator_facade<
                Derived,
                IteratorConcept,
                ValueType,
                Reference,
                Pointer,
                DifferenceType> const &
                rhs) noexcept(noexcept(derived(lhs).equals(derived(rhs))))
        {
            return derived(lhs).equals(derived(rhs));
        }

        template<
            typename Derived,
            typename IteratorConcept,
            typename ValueType,
            typename Reference,
            typename Pointer,
            typename DifferenceType>
        constexpr static auto compare(
            iterator_facade<
                Derived,
                IteratorConcept,
                ValueType,
                Reference,
                Pointer,
                DifferenceType> const & lhs,
            iterator_facade<
                Derived,
                IteratorConcept,
                ValueType,
                Reference,
                Pointer,
                DifferenceType> const &
                rhs) noexcept(noexcept(derived(lhs).compare(derived(rhs))))
        {
            return derived(lhs).compare(derived(rhs));
        }

        template<
            typename Derived,
            typename IteratorConcept,
            typename ValueType,
            typename Reference,
            typename Pointer,
            typename DifferenceType>
        constexpr static void
        next(iterator_facade<
             Derived,
             IteratorConcept,
             ValueType,
             Reference,
             Pointer,
             DifferenceType> & it) noexcept(noexcept(derived(it).next()))
        {
            derived(it).next();
        }

        template<
            typename Derived,
            typename IteratorConcept,
            typename ValueType,
            typename Reference,
            typename Pointer,
            typename DifferenceType>
        constexpr static void
        prev(iterator_facade<
             Derived,
             IteratorConcept,
             ValueType,
             Reference,
             Pointer,
             DifferenceType> & it) noexcept(noexcept(derived(it).prev()))
        {
            derived(it).prev();
        }

        template<
            typename Derived,
            typename IteratorConcept,
            typename ValueType,
            typename Reference,
            typename Pointer,
            typename DifferenceType>
        constexpr static void advance(
            iterator_facade<
                Derived,
                IteratorConcept,
                ValueType,
                Reference,
                Pointer,
                DifferenceType> & it,
            DifferenceType diff) noexcept(noexcept(derived(it).advance(diff)))
        {
            derived(it).advance(diff);
        }
    };

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
    }

    template<
        typename Derived,
        typename IteratorConcept,
        typename ValueType,
        typename Reference,
        typename Pointer,
        typename DifferenceType>
    struct iterator_facade
    {
        using iterator_concept = IteratorConcept;
        using iterator_category = std::random_access_iterator_tag;
        using value_type = ValueType;
        using reference = Reference;
        using pointer = Pointer;
        using difference_type = DifferenceType;

        constexpr reference operator*() const noexcept(
            noexcept(access::dereference(std::declval<iterator_facade &>())))
        {
            return access::dereference(*this);
        }
        constexpr pointer operator->() const
            noexcept(noexcept(detail::make_pointer<pointer>(
                access::dereference(std::declval<iterator_facade &>()))))
        {
            return detail::make_pointer<pointer>(access::dereference(*this));
        }
        constexpr reference operator[](difference_type i) const
            noexcept(noexcept(
                Derived(access::derived(std::declval<iterator_facade &>())),
                access::advance(std::declval<Derived &>(), i),
                access::dereference(std::declval<iterator_facade &>())))
        {
            Derived copy = access::derived(*this);
            access::advance(copy, i);
            return access::dereference(copy);
        }

        constexpr Derived & operator++() noexcept(noexcept(access::advance(
            std::declval<iterator_facade &>(), difference_type(1))))
        {
            access::advance(*this, difference_type(1));
            return access::derived(*this);
        }
        constexpr Derived operator++(int)noexcept(noexcept(
            Derived(access::derived(std::declval<iterator_facade &>())),
            access::advance(
                std::declval<iterator_facade &>(), difference_type(1))))
        {
            Derived retval = access::derived(*this);
            access::advance(*this, difference_type(1));
            return retval;
        }
        constexpr Derived & operator+=(difference_type i) noexcept(
            noexcept(access::advance(std::declval<iterator_facade &>(), i)))
        {
            access::advance(*this, i);
            return access::derived(*this);
        }

        constexpr Derived operator+(difference_type i) noexcept(noexcept(
            Derived(access::derived(std::declval<iterator_facade &>())),
            access::advance(std::declval<iterator_facade &>(), i)))
        {
            Derived copy = access::derived(*this);
            access::advance(copy, i);
            return copy;
        }
        friend BOOST_ITERATOR_FACADE_HIDDEN_FRIEND_CONSTEXPR Derived
        operator+(difference_type i, Derived it) noexcept(noexcept(it + i))
        {
            return it + i;
        }

        constexpr Derived & operator--() noexcept(noexcept(access::advance(
            std::declval<iterator_facade &>(), difference_type(-1))))
        {
            access::advance(*this, difference_type(-1));
            return access::derived(*this);
        }
        constexpr Derived operator--(int)noexcept(noexcept(
            Derived(access::derived(std::declval<iterator_facade &>())),
            access::advance(
                std::declval<iterator_facade &>(), difference_type(-1))))
        {
            Derived retval = access::derived(*this);
            access::advance(*this, difference_type(-1));
            return retval;
        }
        constexpr Derived & operator-=(difference_type i) noexcept(
            noexcept(access::advance(std::declval<iterator_facade &>(), -i)))
        {
            access::advance(*this, -i);
            return access::derived(*this);
        }

        constexpr Derived operator-(difference_type i) noexcept(noexcept(
            Derived(access::derived(std::declval<iterator_facade &>())),
            access::advance(std::declval<iterator_facade &>(), -i)))
        {
            Derived copy = access::derived(*this);
            access::advance(copy, -i);
            return copy;
        }

        friend BOOST_ITERATOR_FACADE_HIDDEN_FRIEND_CONSTEXPR difference_type
        operator-(Derived it1, Derived it2) noexcept(noexcept(it1.comp(it2)))
        {
            return it1.comp(it2);
        }

        friend BOOST_ITERATOR_FACADE_HIDDEN_FRIEND_CONSTEXPR auto operator==(
            Derived lhs,
            Derived rhs) noexcept(noexcept(lhs.comp(rhs) == difference_type(0)))
        {
            return lhs.comp(rhs) == difference_type(0);
        }
        friend BOOST_ITERATOR_FACADE_HIDDEN_FRIEND_CONSTEXPR auto operator!=(
            Derived lhs,
            Derived rhs) noexcept(noexcept(lhs.comp(rhs) != difference_type(0)))
        {
            return lhs.comp(rhs) != difference_type(0);
        }
        friend BOOST_ITERATOR_FACADE_HIDDEN_FRIEND_CONSTEXPR auto operator<(
            Derived lhs,
            Derived rhs) noexcept(noexcept(lhs.comp(rhs) < difference_type(0)))
        {
            return lhs.comp(rhs) < difference_type(0);
        }
        friend BOOST_ITERATOR_FACADE_HIDDEN_FRIEND_CONSTEXPR auto operator<=(
            Derived lhs,
            Derived rhs) noexcept(noexcept(lhs.comp(rhs) <= difference_type(0)))
        {
            return lhs.comp(rhs) <= difference_type(0);
        }
        friend BOOST_ITERATOR_FACADE_HIDDEN_FRIEND_CONSTEXPR auto operator>(
            Derived lhs,
            Derived rhs) noexcept(noexcept(lhs.comp(rhs) > difference_type(0)))
        {
            return lhs.comp(rhs) > difference_type(0);
        }
        friend BOOST_ITERATOR_FACADE_HIDDEN_FRIEND_CONSTEXPR auto operator>=(
            Derived lhs,
            Derived rhs) noexcept(noexcept(lhs.comp(rhs) >= difference_type(0)))
        {
            return lhs.comp(rhs) >= difference_type(0);
        }

#ifndef BOOST_ITERATOR_FACADE_DOXYGEN
    private:
        constexpr auto comp(Derived it2) const noexcept(
            noexcept(access::compare(std::declval<iterator_facade &>(), it2)))
        {
            return access::compare(*this, it2);
        }

        // This primary template implements random access and contiguous
        // iterators.
#if 201703L < __cplusplus && defined(__cpp_lib_ranges)
        static_assert(
            std::is_same<IteratorConcept, std::random_access_iterator_tag>::
                value ||
            std::is_same<IteratorConcept, std::contiguous_iterator_tag>::value);
#else
        static_assert(
            std::is_same<IteratorConcept, std::random_access_iterator_tag>::
                value,
            "");
#endif

#endif
    };

    /** A specialization of `iterator_facade` specific to input iterators. */
    template<
        typename Derived,
        typename ValueType,
        typename Reference,
        typename Pointer,
        typename DifferenceType>
    struct iterator_facade<
        Derived,
        std::input_iterator_tag,
        ValueType,
        Reference,
        Pointer,
        DifferenceType>
    {
        using iterator_concept = std::input_iterator_tag;
        using iterator_category = std::input_iterator_tag;
        using value_type = ValueType;
        using reference = Reference;
        using pointer = Pointer;
        using difference_type = DifferenceType;

        constexpr reference operator*() const noexcept(
            noexcept(access::dereference(std::declval<iterator_facade &>())))
        {
            return access::dereference(*this);
        }
        constexpr pointer operator->() const
            noexcept(noexcept(detail::make_pointer<pointer>(
                access::dereference(std::declval<iterator_facade &>()))))
        {
            return detail::make_pointer<pointer>(access::dereference(*this));
        }

        constexpr Derived & operator++() noexcept(
            noexcept(access::next(std::declval<iterator_facade &>())))
        {
            access::next(*this);
            return access::derived(*this);
        }
        constexpr Derived operator++(int)noexcept(noexcept(
            Derived(access::derived(std::declval<iterator_facade &>())),
            access::next(std::declval<iterator_facade &>())))
        {
            Derived retval = access::derived(*this);
            access::next(*this);
            return retval;
        }

        friend BOOST_ITERATOR_FACADE_HIDDEN_FRIEND_CONSTEXPR auto operator==(
            Derived lhs,
            Derived rhs) noexcept(noexcept(access::equals(lhs, rhs)))
        {
            return access::equals(lhs, rhs);
        }
        friend BOOST_ITERATOR_FACADE_HIDDEN_FRIEND_CONSTEXPR auto
        operator!=(Derived lhs, Derived rhs) noexcept(noexcept(lhs == rhs))
        {
            return !(lhs == rhs);
        }
    };

    // TODO: Compile-fail tests and associated static_asserts in these
    // specializations to help catch common errors.

    /** A specialization of `iterator_facade` specific to output iterators. */
    template<
        typename Derived,
        typename ValueType,
        typename Reference,
        typename Pointer,
        typename DifferenceType>
    struct iterator_facade<
        Derived,
        std::output_iterator_tag,
        ValueType,
        Reference,
        Pointer,
        DifferenceType>
    {
        using iterator_concept = std::output_iterator_tag;
        using iterator_category = std::output_iterator_tag;
        using value_type = ValueType;
        using reference = Reference;
        using pointer = void;
        using difference_type = DifferenceType;

        constexpr reference operator*() const noexcept(
            noexcept(access::dereference(std::declval<iterator_facade &>())))
        {
            return access::dereference(*this);
        }

        // This non-const overload exists to support output iterators like
        // std::back_insert_iterator.
        constexpr reference operator*() noexcept(
            noexcept(access::dereference(std::declval<iterator_facade &>())))
        {
            return access::dereference(*this);
        }

        constexpr Derived & operator++() noexcept(
            noexcept(access::next(std::declval<iterator_facade &>())))
        {
            access::next(*this);
            return access::derived(*this);
        }
        constexpr Derived operator++(int)noexcept(noexcept(
            Derived(access::derived(std::declval<iterator_facade &>())),
            access::next(std::declval<iterator_facade &>())))
        {
            Derived retval = access::derived(*this);
            access::next(*this);
            return retval;
        }
    };

    /** A specialization of `iterator_facade` specific to forward
        iterators. */
    template<
        typename Derived,
        typename ValueType,
        typename Reference,
        typename Pointer,
        typename DifferenceType>
    struct iterator_facade<
        Derived,
        std::forward_iterator_tag,
        ValueType,
        Reference,
        Pointer,
        DifferenceType>
    {
        using iterator_concept = std::forward_iterator_tag;
        using iterator_category = std::forward_iterator_tag;
        using value_type = ValueType;
        using reference = Reference;
        using pointer = Pointer;
        using difference_type = DifferenceType;

        constexpr reference operator*() const noexcept(
            noexcept(access::dereference(std::declval<iterator_facade &>())))
        {
            return access::dereference(*this);
        }
        constexpr pointer operator->() const
            noexcept(noexcept(detail::make_pointer<pointer>(
                access::dereference(std::declval<iterator_facade &>()))))
        {
            return detail::make_pointer<pointer>(access::dereference(*this));
        }

        constexpr Derived & operator++() noexcept(
            noexcept(access::next(std::declval<iterator_facade &>())))
        {
            access::next(*this);
            return access::derived(*this);
        }
        constexpr Derived operator++(int)noexcept(noexcept(
            Derived(access::derived(std::declval<iterator_facade &>())),
            access::next(std::declval<iterator_facade &>())))
        {
            Derived retval = access::derived(*this);
            access::next(*this);
            return retval;
        }

        friend BOOST_ITERATOR_FACADE_HIDDEN_FRIEND_CONSTEXPR auto operator==(
            Derived lhs,
            Derived rhs) noexcept(noexcept(access::equals(lhs, rhs)))
        {
            return access::equals(lhs, rhs);
        }
        friend BOOST_ITERATOR_FACADE_HIDDEN_FRIEND_CONSTEXPR auto
        operator!=(Derived lhs, Derived rhs) noexcept(noexcept(lhs == rhs))
        {
            return !(lhs == rhs);
        }
    };

    /** A specialization of `iterator_facade` specific to bidirectional
        iterators. */
    template<
        typename Derived,
        typename ValueType,
        typename Reference,
        typename Pointer,
        typename DifferenceType>
    struct iterator_facade<
        Derived,
        std::bidirectional_iterator_tag,
        ValueType,
        Reference,
        Pointer,
        DifferenceType>
    {
        using iterator_concept = std::bidirectional_iterator_tag;
        using iterator_category = std::bidirectional_iterator_tag;
        using value_type = ValueType;
        using reference = Reference;
        using pointer = Pointer;
        using difference_type = DifferenceType;

        constexpr reference operator*() const noexcept(
            noexcept(access::dereference(std::declval<iterator_facade &>())))
        {
            return access::dereference(*this);
        }
        constexpr pointer operator->() const
            noexcept(noexcept(detail::make_pointer<pointer>(
                access::dereference(std::declval<iterator_facade &>()))))
        {
            return detail::make_pointer<pointer>(access::dereference(*this));
        }

        constexpr Derived & operator++() noexcept(
            noexcept(access::next(std::declval<iterator_facade &>())))
        {
            access::next(*this);
            return access::derived(*this);
        }
        constexpr Derived operator++(int)noexcept(noexcept(
            Derived(access::derived(std::declval<iterator_facade &>())),
            access::next(std::declval<iterator_facade &>())))
        {
            Derived retval = access::derived(*this);
            access::next(*this);
            return retval;
        }

        constexpr Derived & operator--() noexcept(
            noexcept(access::prev(std::declval<iterator_facade &>())))
        {
            access::prev(*this);
            return access::derived(*this);
        }
        constexpr Derived operator--(int)noexcept(noexcept(
            Derived(access::derived(std::declval<iterator_facade &>())),
            access::prev(std::declval<iterator_facade &>())))
        {
            Derived retval = access::derived(*this);
            access::prev(*this);
            return retval;
        }

        friend BOOST_ITERATOR_FACADE_HIDDEN_FRIEND_CONSTEXPR auto operator==(
            Derived lhs,
            Derived rhs) noexcept(noexcept(access::equals(lhs, rhs)))
        {
            return access::equals(lhs, rhs);
        }
        friend BOOST_ITERATOR_FACADE_HIDDEN_FRIEND_CONSTEXPR auto
        operator!=(Derived lhs, Derived rhs) noexcept(noexcept(lhs == rhs))
        {
            return !(lhs == rhs);
        }
    };

    /** A template alias useful for defining proxy iterators.  \see
        `iterator_facade`. */
    template<
        typename Derived,
        typename IteratorConcept,
        typename ValueType,
        typename Reference = ValueType,
        typename DifferenceType = std::ptrdiff_t>
    using proxy_iterator_facade = iterator_facade<
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
