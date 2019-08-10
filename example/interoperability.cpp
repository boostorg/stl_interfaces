//[ interoperability
// Copyright (C) 2019 T. Zachary Laine
//
// Distributed under the Boost Software License, Version 1.0. (See
// accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
#include <boost/iterator_facade/iterator_facade.hpp>

#include <algorithm>
#include <array>

#include <cassert>


// This is a random access iterator templated on a value type.  The ValueType
// template parameter allows us easily to define const and non-const iterators
// from the same template.
template<typename ValueType>
struct random_access_iterator : boost::iterator_facade::iterator_facade<
                                    random_access_iterator<ValueType>,
                                    std::random_access_iterator_tag,
                                    ValueType>
{
    // Default constructor.
    constexpr random_access_iterator() noexcept {}

    // Construction from an underlying pointer.
    constexpr random_access_iterator(ValueType * it) noexcept : it_(it) {}

    // Implicit conversion from an existing random_access_iterator with a
    // possibly different value type.  The enable_if logic here just enforces
    // that this constructor only participates in overload resolution when the
    // expression it_ = other.it_ is well-formed.
    template<
        typename ValueType2,
        typename E = std::enable_if_t<std::is_convertible<
            typename ValueType2::value_type *,
            ValueType *>::value>>
    constexpr random_access_iterator(ValueType2 other) noexcept : it_(other.it_)
    {}

private:
    friend boost::iterator_facade::access;
    constexpr ValueType & dereference() const noexcept { return *it_; }
    constexpr void advance(std::ptrdiff_t i) noexcept { it_ += i; }
    constexpr auto compare(random_access_iterator other) const noexcept
    {
        return it_ - other.it_;
    }

    ValueType * it_;

    // This friendship is necessary to enable the implicit conversion
    // constructor to work aboev.
    template<typename ValueType2>
    friend struct random_access_iterator;
};

using iterator = random_access_iterator<int>;
using const_iterator = random_access_iterator<int const>;

int main()
{
    std::array<int, 10> ints = {{0, 2, 1, 3, 4, 5, 7, 6, 8, 9}};

    // Create and use two mutable iterators.
    iterator first(ints.data());
    iterator last(ints.data() + ints.size());
    std::sort(first, last);
    assert(ints == (std::array<int, 10>{{0, 1, 2, 3, 4, 5, 6, 7, 8, 9}}));

    // Create and use two constant iterators, one from an existing mutable one.
    std::array<int, 10> ints_copy;
    const_iterator cfirst(ints.data());
    const_iterator clast = last;
    std::copy(cfirst, clast, ints_copy.begin());
    assert(ints_copy == (std::array<int, 10>{{0, 1, 2, 3, 4, 5, 6, 7, 8, 9}}));
}
//]
