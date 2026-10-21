// Copyright (C) 2026 T. Zachary Laine
//
// Distributed under the Boost Software License, Version 1.0. (See
// accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
#include "ill_formed.hpp"

#include <boost/stl_interfaces/iterator_interface.hpp>

#include <boost/core/lightweight_test.hpp>

#include <array>
#include <list>
#include <vector>


#if !BOOST_STL_INTERFACES_USE_TOKEN_SEQUENCE_INJECTION

int main() { return boost::report_errors(); }

#else

#include <debugging>


namespace v4_dtl = boost::stl_interfaces::v4::v4_dtl;

namespace v4_explicit {
}

namespace v4_deduced {
    struct basic_input_iter
    {
        basic_input_iter() : it_(nullptr) {}
        basic_input_iter(int * it) : it_(it) {}

        int & operator*() const noexcept { return *it_; }
        basic_input_iter & operator++() noexcept
        {
            ++it_;
            return *this;
        }
        bool operator==(basic_input_iter rhs) const noexcept
        {
            return it_ == rhs.it_;
        }

#if BOOST_STL_INTERFACES_USE_TOKEN_SEQUENCE_INJECTION
        consteval
        {
            boost::stl_interfaces::iterator_interface(
                std::input_iterator_tag{}, ^^basic_input_iter);
        }
#endif

    private:
        int * it_;
    };
    BOOST_STL_INTERFACES_STATIC_ASSERT_CONCEPT(
        basic_input_iter, std::input_iterator)

    template<typename ValueType>
    struct input_iter
    {
        input_iter() : it_(nullptr) {}
        input_iter(ValueType * it) : it_(it) {}
        template<
            typename ValueType2,
            typename E = std::enable_if_t<
                std::is_convertible<ValueType2 *, ValueType *>::value>>
        input_iter(input_iter<ValueType2> it) : it_(it.it_)
        {}

        ValueType & operator*() const noexcept { return *it_; }
        input_iter & operator++() noexcept
        {
            ++it_;
            return *this;
        }
        friend bool operator==(input_iter lhs, input_iter rhs) noexcept
        {
            return lhs.it_ == rhs.it_;
        }

#if BOOST_STL_INTERFACES_USE_TOKEN_SEQUENCE_INJECTION
        consteval
        {
            boost::stl_interfaces::iterator_interface(
                std::input_iterator_tag{}, ^^input_iter);
        }
#endif

    private:
        ValueType * it_;

        template<typename ValueType2>
        friend struct input_iter;
    };
    BOOST_STL_INTERFACES_STATIC_ASSERT_CONCEPT(
        input_iter<int>, std::input_iterator)

    template<typename ValueType>
    struct proxy_input_iter
    {
        proxy_input_iter() : it_(nullptr) {}
        proxy_input_iter(ValueType * it) : it_(it) {}
        template<
            typename ValueType2,
            typename E = std::enable_if_t<
                std::is_convertible<ValueType2 *, ValueType *>::value>>
        proxy_input_iter(proxy_input_iter<ValueType2> it) : it_(it.it_)
        {}

        ValueType operator*() const noexcept { return *it_; }
        proxy_input_iter & operator++() noexcept
        {
            ++it_;
            return *this;
        }
        friend bool
        operator==(proxy_input_iter lhs, proxy_input_iter rhs) noexcept
        {
            return lhs.it_ == rhs.it_;
        }

#if BOOST_STL_INTERFACES_USE_TOKEN_SEQUENCE_INJECTION
        consteval
        {
            boost::stl_interfaces::proxy_iterator_interface(
                std::input_iterator_tag{}, ^^proxy_input_iter);
        }
#endif

    private:
        ValueType * it_;

        template<typename ValueType2>
        friend struct proxy_input_iter;
    };
    using int_pair = std::pair<int, int>;
    BOOST_STL_INTERFACES_STATIC_ASSERT_CONCEPT(
        proxy_input_iter<int_pair>, std::input_iterator)
}

#define CONSTEVAL_TEST(e)                                                      \
    do {                                                                       \
        if (!(e))                                                              \
            throw std::runtime_error(#e);                                      \
    } while (false)

int main() { return boost::report_errors(); }

#endif
