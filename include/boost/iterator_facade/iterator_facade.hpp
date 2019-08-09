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


namespace boost { namespace iterator_facade {

    /** A CRTP template that one may derive from to make defining iterators
        easier. */
    template<
        typename Derived,
        typename IteratorCategory,
        typename ValueType,
        typename Reference = ValueType &,
        typename Pointer = ValueType *,
        typename DifferenceType = std::ptrdiff_t>
    struct iterator_facade;

    /** A type for granting access to the private members of an iterator
        derived from iterator_facade. */
    struct access
    {
        template<
            typename Derived,
            typename IteratorCategory,
            typename ValueType,
            typename Reference,
            typename Pointer,
            typename DifferenceType>
        constexpr static Derived & derived(iterator_facade<
                                           Derived,
                                           IteratorCategory,
                                           ValueType,
                                           Reference,
                                           Pointer,
                                           DifferenceType> & if_) noexcept
        {
            return static_cast<Derived &>(if_);
        }
        template<
            typename Derived,
            typename IteratorCategory,
            typename ValueType,
            typename Reference,
            typename Pointer,
            typename DifferenceType>
        constexpr static Derived const &
        derived(iterator_facade<
                Derived,
                IteratorCategory,
                ValueType,
                Reference,
                Pointer,
                DifferenceType> const & if_) noexcept
        {
            return static_cast<Derived const &>(if_);
        }


        template<
            typename Derived,
            typename IteratorCategory,
            typename ValueType,
            typename Reference,
            typename Pointer,
            typename DifferenceType>
        constexpr static decltype(auto)
        dereference(iterator_facade<
                    Derived,
                    IteratorCategory,
                    ValueType,
                    Reference,
                    Pointer,
                    DifferenceType> const &
                        it) noexcept(noexcept(derived(it).dereference()))
        {
            return derived(it).dereference();
        }

        // This overload takes a non-const facade parameter, to support output
        // iterators like std::back_insert_iterator.
        template<
            typename Derived,
            typename IteratorCategory,
            typename ValueType,
            typename Reference,
            typename Pointer,
            typename DifferenceType>
        constexpr static decltype(auto)
        dereference(iterator_facade<
                    Derived,
                    IteratorCategory,
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
            typename IteratorCategory,
            typename ValueType,
            typename Reference,
            typename Pointer,
            typename DifferenceType>
        constexpr static auto equals(
            iterator_facade<
                Derived,
                IteratorCategory,
                ValueType,
                Reference,
                Pointer,
                DifferenceType> const & lhs,
            iterator_facade<
                Derived,
                IteratorCategory,
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
            typename IteratorCategory,
            typename ValueType,
            typename Reference,
            typename Pointer,
            typename DifferenceType>
        constexpr static auto compare(
            iterator_facade<
                Derived,
                IteratorCategory,
                ValueType,
                Reference,
                Pointer,
                DifferenceType> const & lhs,
            iterator_facade<
                Derived,
                IteratorCategory,
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
            typename IteratorCategory,
            typename ValueType,
            typename Reference,
            typename Pointer,
            typename DifferenceType>
        constexpr static void
        next(iterator_facade<
             Derived,
             IteratorCategory,
             ValueType,
             Reference,
             Pointer,
             DifferenceType> & it) noexcept(noexcept(derived(it).next()))
        {
            derived(it).next();
        }

        template<
            typename Derived,
            typename IteratorCategory,
            typename ValueType,
            typename Reference,
            typename Pointer,
            typename DifferenceType>
        constexpr static void
        prev(iterator_facade<
             Derived,
             IteratorCategory,
             ValueType,
             Reference,
             Pointer,
             DifferenceType> & it) noexcept(noexcept(derived(it).prev()))
        {
            derived(it).prev();
        }

        template<
            typename Derived,
            typename IteratorCategory,
            typename ValueType,
            typename Reference,
            typename Pointer,
            typename DifferenceType>
        constexpr static void advance(
            iterator_facade<
                Derived,
                IteratorCategory,
                ValueType,
                Reference,
                Pointer,
                DifferenceType> & it,
            DifferenceType diff) noexcept(noexcept(derived(it).advance(diff)))
        {
            derived(it).advance(diff);
        }
    };

    namespace detail {
        // The return type of operator->() in proxy iterators.
        template<typename T>
        struct arrow
        {
            arrow(T const & value) noexcept(noexcept(T(value))) : value_(value)
            {}
            arrow(T && value) noexcept(noexcept(T(std::move(value)))) :
                value_(std::move(value))
            {}

            T const * operator->() const noexcept { return &value_; }
            T * operator->() noexcept { return &value_; }

        private:
            T value_;
        };

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

        friend constexpr auto operator==(Derived lhs, Derived rhs) noexcept(
            noexcept(access::equals(lhs, rhs)))
        {
            return access::equals(lhs, rhs);
        }
        friend constexpr auto
        operator!=(Derived lhs, Derived rhs) noexcept(noexcept(lhs == rhs))
        {
            return !(lhs == rhs);
        }
    };

    // TODO: Compile-fail tests and associated static_asserts in these
    // specializations to help catch common errors.

    // TODO: Examples that implement certain common iterator patterns, like
    // back_inserter.

    // TODO: Tests that cover iterator_traits for uses of these facades.

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

        friend constexpr auto operator==(Derived lhs, Derived rhs) noexcept(
            noexcept(access::equals(lhs, rhs)))
        {
            return access::equals(lhs, rhs);
        }
        friend constexpr auto
        operator!=(Derived lhs, Derived rhs) noexcept(noexcept(lhs == rhs))
        {
            return !(lhs == rhs);
        }
    };

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

        friend constexpr auto operator==(Derived lhs, Derived rhs) noexcept(
            noexcept(access::equals(lhs, rhs)))
        {
            return access::equals(lhs, rhs);
        }
        friend constexpr auto
        operator!=(Derived lhs, Derived rhs) noexcept(noexcept(lhs == rhs))
        {
            return !(lhs == rhs);
        }
    };

    template<
        typename Derived,
        typename ValueType,
        typename Reference,
        typename Pointer,
        typename DifferenceType>
    struct iterator_facade<
        Derived,
        std::random_access_iterator_tag,
        ValueType,
        Reference,
        Pointer,
        DifferenceType>
    {
        using iterator_concept = std::random_access_iterator_tag;
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
        friend constexpr Derived
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

        friend constexpr difference_type
        operator-(Derived it1, Derived it2) noexcept(noexcept(it1.comp(it2)))
        {
            return it1.comp(it2);
        }

        friend constexpr auto operator==(Derived lhs, Derived rhs) noexcept(
            noexcept(lhs.comp(rhs) == difference_type(0)))
        {
            return lhs.comp(rhs) == difference_type(0);
        }
        friend constexpr auto operator!=(Derived lhs, Derived rhs) noexcept(
            noexcept(lhs.comp(rhs) != difference_type(0)))
        {
            return lhs.comp(rhs) != difference_type(0);
        }
        friend constexpr auto operator<(Derived lhs, Derived rhs) noexcept(
            noexcept(lhs.comp(rhs) < difference_type(0)))
        {
            return lhs.comp(rhs) < difference_type(0);
        }
        friend constexpr auto operator<=(Derived lhs, Derived rhs) noexcept(
            noexcept(lhs.comp(rhs) <= difference_type(0)))
        {
            return lhs.comp(rhs) <= difference_type(0);
        }
        friend constexpr auto operator>(Derived lhs, Derived rhs) noexcept(
            noexcept(lhs.comp(rhs) > difference_type(0)))
        {
            return lhs.comp(rhs) > difference_type(0);
        }
        friend constexpr auto operator>=(Derived lhs, Derived rhs) noexcept(
            noexcept(lhs.comp(rhs) >= difference_type(0)))
        {
            return lhs.comp(rhs) >= difference_type(0);
        }

    private:
        constexpr auto comp(Derived it2) const noexcept(
            noexcept(access::compare(std::declval<iterator_facade &>(), it2)))
        {
            return access::compare(*this, it2);
        }
    };

#if 201703L < __cplusplus

    template<
        typename Derived,
        typename ValueType,
        typename Reference,
        typename Pointer,
        typename DifferenceType>
    struct iterator_facade<
        Derived,
        std::contiguous_iterator_tag,
        ValueType,
        Reference,
        Pointer,
        DifferenceType>
    {
        using iterator_concept = std::contiguous_iterator_tag;
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
        friend constexpr Derived
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

        friend constexpr difference_type
        operator-(Derived it1, Derived it2) noexcept(noexcept(it1.comp(it2)))
        {
            return it1.comp(it2);
        }

        friend constexpr auto operator==(Derived lhs, Derived rhs) noexcept(
            noexcept(lhs.comp(rhs) == difference_type(0)))
        {
            return lhs.comp(rhs) == difference_type(0);
        }
        friend constexpr auto operator!=(Derived lhs, Derived rhs) noexcept(
            noexcept(lhs.comp(rhs) != difference_type(0)))
        {
            return lhs.comp(rhs) != difference_type(0);
        }
        friend constexpr auto operator<(Derived lhs, Derived rhs) noexcept(
            noexcept(lhs.comp(rhs) < difference_type(0)))
        {
            return lhs.comp(rhs) < difference_type(0);
        }
        friend constexpr auto operator<=(Derived lhs, Derived rhs) noexcept(
            noexcept(lhs.comp(rhs) <= difference_type(0)))
        {
            return lhs.comp(rhs) <= difference_type(0);
        }
        friend constexpr auto operator>(Derived lhs, Derived rhs) noexcept(
            noexcept(lhs.comp(rhs) > difference_type(0)))
        {
            return lhs.comp(rhs) > difference_type(0);
        }
        friend constexpr auto operator>=(Derived lhs, Derived rhs) noexcept(
            noexcept(lhs.comp(rhs) >= difference_type(0)))
        {
            return lhs.comp(rhs) >= difference_type(0);
        }

    private:
        constexpr auto comp(Derived it2) const noexcept(
            noexcept(access::compare(std::declval<iterator_facade &>(), it2)))
        {
            return access::compare(*this, it2);
        }
    };

#endif

    /** A template alias useful for defining proxy iterators.  \see
        iterator_facade. */
    template<
        typename Derived,
        typename IteratorCategory,
        typename ValueType,
        typename Reference = ValueType,
        typename DifferenceType = std::ptrdiff_t>
    using proxy_iterator_facade = iterator_facade<
        Derived,
        IteratorCategory,
        ValueType,
        Reference,
        detail::arrow<Reference>,
        DifferenceType>;

}}

#endif
