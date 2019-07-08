#ifndef BOOST_ITERATOR_FACADE_ITERATOR_FACADE_HPP
#define BOOST_ITERATOR_FACADE_ITERATOR_FACADE_HPP

#include <utility>
#include <iterator>


namespace boost { namespace iterator_facade {

    template<
        typename Derived,
        typename IteratorCategory,
        typename ValueType,
        typename Reference = ValueType &,
        typename Pointer = ValueType *,
        typename DifferenceType = std::ptrdiff_t>
    struct iterator_facade;

    struct access
    {
        template<
            typename Derived,
            typename IteratorCategory,
            typename ValueType,
            typename Reference,
            typename Pointer,
            typename DifferenceType>
        constexpr static auto
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
        constexpr static auto
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
    };

    // TODO: Do we need to support the *it++ semantics from the C++17 input
    // iterator table?
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
#if 0 // TODO
        using iterator_concept = std::input_iterator_tag;
#endif
        using iterator_category = std::input_iterator_tag;
        using value_type = ValueType;
        using reference = Reference;
        using pointer = void;
        using difference_type = DifferenceType;

        constexpr value_type operator*() const
            noexcept(noexcept(access::dereference(*this)))
        {
            return access::dereference(*this);
        }

        constexpr Derived & operator++() noexcept(noexcept(access::next(*this)))
        {
            access::next(*this);
            return access::derived(*this);
        }
        constexpr Derived
        operator++(int)noexcept(noexcept(access::next(*this)))
        {
            return ++*this;
        }
    };

    template<
        typename Derived,
        typename IteratorCategory,
        typename ValueType,
        typename Reference,
        typename Pointer,
        typename DifferenceType>
    auto operator==(
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
            DifferenceType> const & rhs)
    {
        return access::equals(lhs, rhs);
    }
    template<
        typename Derived,
        typename IteratorCategory,
        typename ValueType,
        typename Reference,
        typename Pointer,
        typename DifferenceType>
    auto operator!=(
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
            DifferenceType> const & rhs)
    {
        return !(lhs == rhs);
    }

}}

#endif
