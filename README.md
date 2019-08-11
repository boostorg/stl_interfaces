# iterator_facade

An updated C++20-friendly version of the iterator_facade part of
Boost.Iterator, targeting standardization.  This library requires at least
C++14.

In short, if you need to write an iterator, iterator_facade turns this:

```c++
    struct repeated_chars_iterator
    {
        using value_type = char;
        using difference_type = std::ptrdiff_t;
        using pointer = char const *;
        using reference = char const;
        using iterator_category = std::random_access_iterator_tag;

        constexpr repeated_chars_iterator() noexcept :
            first_(nullptr),
            size_(0),
            n_(0)
        {}
        constexpr repeated_chars_iterator(
            char const * first,
            difference_type size,
            difference_type n) noexcept :
            first_(first),
            size_(size),
            n_(n)
        {}

        constexpr reference operator*() const noexcept
        {
            return first_[n_ % size_];
        }

        constexpr value_type operator[](difference_type n) const noexcept
        {
            return first_[(n_ + n) % size_];
        }

        constexpr repeated_chars_iterator & operator++() noexcept
        {
            ++n_;
            return *this;
        }
        constexpr repeated_chars_iterator operator++(int)noexcept
        {
            repeated_chars_iterator retval = *this;
            ++*this;
            return retval;
        }
        constexpr repeated_chars_iterator & operator+=(difference_type n) noexcept
        {
            n_ += n;
            return *this;
        }

        constexpr repeated_chars_iterator & operator--() noexcept
        {
            --n_;
            return *this;
        }
        constexpr repeated_chars_iterator operator--(int)noexcept
        {
            repeated_chars_iterator retval = *this;
            --*this;
            return retval;
        }
        constexpr repeated_chars_iterator & operator-=(difference_type n) noexcept
        {
            n_ -= n;
            return *this;
        }

        friend constexpr bool operator==(
            repeated_chars_iterator lhs, repeated_chars_iterator rhs) noexcept
        {
            return lhs.first_ == rhs.first_ && lhs.n_ == rhs.n_;
        }
        friend constexpr bool operator!=(
            repeated_chars_iterator lhs, repeated_chars_iterator rhs) noexcept
        {
            return !(lhs == rhs);
        }
        friend constexpr bool operator<(
            repeated_chars_iterator lhs, repeated_chars_iterator rhs) noexcept
        {
            return lhs.first_ == rhs.first_ && lhs.n_ < rhs.n_;
        }
        friend constexpr bool operator<=(
            repeated_chars_iterator lhs, repeated_chars_iterator rhs) noexcept
        {
            return lhs == rhs || lhs < rhs;
        }
        friend constexpr bool operator>(
            repeated_chars_iterator lhs, repeated_chars_iterator rhs) noexcept
        {
            return rhs < lhs;
        }
        friend constexpr bool operator>=(
            repeated_chars_iterator lhs, repeated_chars_iterator rhs) noexcept
        {
            return lhs <= rhs;
        }

        friend constexpr repeated_chars_iterator
        operator+(repeated_chars_iterator lhs, difference_type rhs) noexcept
        {
            return lhs += rhs;
        }
        friend constexpr repeated_chars_iterator
        operator+(difference_type lhs, repeated_chars_iterator rhs) noexcept
        {
            return rhs += lhs;
        }
        friend constexpr repeated_chars_iterator
        operator-(repeated_chars_iterator lhs, difference_type rhs) noexcept
        {
            return lhs -= rhs;
        }
        friend constexpr repeated_chars_iterator
        operator-(difference_type lhs, repeated_chars_iterator rhs) noexcept
        {
            return rhs -= lhs;
        }
        friend constexpr difference_type operator-(
            repeated_chars_iterator lhs, repeated_chars_iterator rhs) noexcept
        {
            return lhs.n_ - rhs.n_;
        }

    private:
        char const * first_;
        difference_type size_;
        difference_type n_;
    };
```

into this:

```c++
struct repeated_chars_iterator : boost::iterator_facade::iterator_facade<
                                     repeated_chars_iterator,
                                     std::random_access_iterator_tag,
                                     char,
                                     char const>
{
    constexpr repeated_chars_iterator() noexcept :
        first_(nullptr),
        size_(0),
        n_(0)
    {}
    constexpr repeated_chars_iterator(
        char const * first, difference_type size, difference_type n) noexcept :
        first_(first),
        size_(size),
        n_(n)
    {}

private:
    friend boost::iterator_facade::access;
    constexpr char const dereference() const noexcept
    {
        return first_[n_ % size_];
    }
    constexpr void advance(std::ptrdiff_t i) noexcept { n_ += i; }
    constexpr auto compare(repeated_chars_iterator other) const noexcept
    {
        return n_ - other.n_;
    }

    char const * first_;
    difference_type size_;
    difference_type n_;
};
```

If you don't ever write iterators, this is not for you.

Online docs: https://tzlaine.github.io/iterator_facade.

[![Build Status](https://travis-ci.org/tzlaine/iterator_facade.svg?branch=master)](https://travis-ci.org/tzlaine/iterator_facade)
[![Build Status](https://ci.appveyor.com/api/projects/status/github/tzlaine/iterator_facade?branch=master&svg=true)](https://ci.appveyor.com/project/tzlaine/iterator_facade)
[![License](https://img.shields.io/badge/license-boost-brightgreen.svg)](LICENSE_1_0.txt)
