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

struct empty
{};
struct reference_only_1
{
    using reference = int &;
    using foo = double;
};
struct reference_only_2
{
    using reference = std::tuple<int &, int &>;
    using foo = double;
};
struct value_type_only
{
    using value_type = int const;
    using foo = double;
};
struct pointer_only
{
    using pointer = int *;
    using foo = double;
};
struct difference_type_only
{
    using difference_type = std::ptrdiff_t;
    using foo = double;
};

struct wrong_ops
{
    // binary operator*
    int operator*(wrong_ops) { return 42; }
    friend int operator*(wrong_ops, wrong_ops) { return 42; }
    // unary operator*, but unrelated type
    friend int operator*(difference_type_only) { return 42; }

    // unary operator-
    wrong_ops operator-() { return {}; }
    friend wrong_ops operator-(wrong_ops) { return {}; }

    // unary operator+
    wrong_ops operator+() { return {}; }
    friend wrong_ops operator+(wrong_ops) { return {}; }

    // unary operator++, but unrelated type
    friend difference_type_only & operator++(difference_type_only & x)
    {
        return x;
    }
    friend difference_type_only & operator++(difference_type_only & x, int)
    {
        return x;
    }
    // unary operator--, but unrelated type
    friend difference_type_only & operator--(difference_type_only & x)
    {
        return x;
    }
    friend difference_type_only & operator--(difference_type_only & x, int)
    {
        return x;
    }
};

struct member_binary_plus_only
{
    int operator+(member_binary_plus_only) { return 42; }
};
struct nonmember_binary_plus_only
{
    friend int operator+(int, member_binary_plus_only) { return 42; }
};

struct nondifference_member_binary_minus
{
    int operator-(int) { return 42; }
};
struct nondifference_nonmember_binary_minus_1
{
    friend int operator-(int, nondifference_nonmember_binary_minus_1 const &)
    {
        return 42;
    }
};
struct nondifference_nonmember_binary_minus_2
{
    friend int operator-(nondifference_nonmember_binary_minus_2, int)
    {
        return 42;
    }
};

struct member_binary_minus_only
{
    int operator-(member_binary_minus_only) { return 42; }
};
struct member_binary_minus_only_auto_return
{
    auto operator-(member_binary_minus_only_auto_return) { return 42; }
};
struct nonmember_binary_minus_only
{
    friend int
    operator-(nonmember_binary_minus_only, nonmember_binary_minus_only const &)
    {
        return 42;
    }
};
struct nonmember_binary_minus_only_auto_return
{
    friend int operator-(
        nonmember_binary_minus_only_auto_return,
        nonmember_binary_minus_only_auto_return const &)
    {
        return 42;
    }
};

template<typename T>
struct template_member_binary_minus_only
{
    int operator-(template_member_binary_minus_only) { return 42; }
};

struct member_pre_post_incr
{
    member_pre_post_incr & operator++() { return *this; }
    member_pre_post_incr operator++(int) { return *this; }
};
struct nonmember_pre_post_incr
{
    friend nonmember_pre_post_incr & operator++(nonmember_pre_post_incr & x)
    {
        return x;
    }
    friend nonmember_pre_post_incr operator++(nonmember_pre_post_incr & x, int)
    {
        return x;
    }
};
struct member_pre_post_decr
{
    member_pre_post_decr & operator--() { return *this; }
    member_pre_post_decr operator--(int) { return *this; }
};
struct nonmember_pre_post_decr
{
    friend nonmember_pre_post_decr & operator--(nonmember_pre_post_decr & x)
    {
        return x;
    }
    friend nonmember_pre_post_decr operator--(nonmember_pre_post_decr & x, int)
    {
        return x;
    }
};

struct adapted_deref_subtract
{
    int *& base_reference() noexcept { return it_; }
    int * base_reference() const noexcept { return it_; }

    int * it_;
};
struct adapted_deref_subtract_defined_reference
{
    using reference = int const &;

    int *& base_reference() noexcept { return it_; }
    int * base_reference() const noexcept { return it_; }

    int * it_;
};
struct adapted_deref_subtract_defined_difference_type
{
    using difference_type = int;

    int *& base_reference() noexcept { return it_; }
    int * base_reference() const noexcept { return it_; }

    int * it_;
};

struct int_t
{
    int value_;

    bool operator==(int_t other) const { return value_ == other.value_; }
    bool operator!=(int_t other) const { return value_ != other.value_; }
    bool operator<(int_t other) const { return value_ < other.value_; }

    bool operator==(int other) const { return value_ == other; }
    bool operator!=(int other) const { return value_ != other; }
    bool operator<(int other) const { return value_ < other; }

    friend bool operator==(int lhs, int_t rhs) { return lhs == rhs.value_; }
    friend bool operator!=(int lhs, int_t rhs) { return lhs != rhs.value_; }
    friend bool operator<(int lhs, int_t rhs) { return lhs < rhs.value_; }
};
struct udt_zip_iter
{
    using reference = std::tuple<int_t &, int &>;
    using value_type = std::tuple<int_t, int>;
};

struct params_of_nullary
{
    params_of_nullary & operator--() { return *this; }
};

#define CONSTEVAL_TEST(e)                                                      \
    do {                                                                       \
        if (!(e))                                                              \
            throw std::runtime_error(#e);                                      \
    } while (false)

struct params_of_unary_1
{
    std::ptrdiff_t operator-(params_of_unary_1) { return 42; }
    friend params_of_unary_1 operator+(params_of_unary_1) { return {}; }
    params_of_unary_1 operator++(int) { return *this; }
    std::ptrdiff_t operator/(std::ptrdiff_t) { return 42; }
    std::ptrdiff_t operator%(std::size_t) { return 42; }

    consteval
    {
        auto t = ^^params_of_unary_1;
        for (std::meta::info f : hidden_friends_of(t)) {
            if (!is_operator_function(f))
                continue;
            auto op = operator_of(f);
            if (op != std::meta::operators::op_plus)
                continue;
            v4_dtl::op_params result =
                v4_dtl::params_of(t, op, f, v4_dtl::member::no);
            CONSTEVAL_TEST(result == v4_dtl::op_params::t);
        }
    }
};

struct params_of_unary_2
{
    params_of_unary_2 operator+(std::ptrdiff_t n) { return *this; }
};

struct params_of_binary
{
    friend std::ptrdiff_t operator-(params_of_binary, params_of_binary) { return 42; }
    friend params_of_binary operator--(params_of_binary, int) { return {}; }
    friend std::ptrdiff_t operator/(params_of_binary, std::ptrdiff_t) { return 42; }
    friend std::ptrdiff_t operator%(std::ptrdiff_t, params_of_binary) { return 42; }

    consteval
    {
        auto t = ^^params_of_unary_1;
        for (std::meta::info f : hidden_friends_of(t)) {
            if (!is_operator_function(f))
                continue;
            auto op = operator_of(f);
            if (op == std::meta::operators::op_minus) {
                v4_dtl::op_params result =
                    v4_dtl::params_of(t, op, f, v4_dtl::member::no);
                CONSTEVAL_TEST(result == v4_dtl::op_params::t_t);
            }
            if (op == std::meta::operators::op_minus_minus) {
                v4_dtl::op_params result =
                    v4_dtl::params_of(t, op, f, v4_dtl::member::no);
                CONSTEVAL_TEST(result == v4_dtl::op_params::post);
            }
            if (op == std::meta::operators::op_slash) {
                v4_dtl::op_params result =
                    v4_dtl::params_of(t, op, f, v4_dtl::member::no);
                CONSTEVAL_TEST(result == v4_dtl::op_params::t_n);
            }
            if (op == std::meta::operators::op_percent) {
                v4_dtl::op_params result =
                    v4_dtl::params_of(t, op, f, v4_dtl::member::no);
                CONSTEVAL_TEST(result == v4_dtl::op_params::n_t);
            }
        }
    }
};

int main()
{
    // params_of(), nullary
    consteval
    {
        constexpr v4_dtl::op_params result = v4_dtl::params_of(
            ^^params_of_nullary,
            std::meta::operators::op_minus_minus,
            ^^params_of_nullary::operator--,
            v4_dtl::member::yes);
        CONSTEVAL_TEST(result == v4_dtl::op_params::t);
    }
    // params_of(), unary
    consteval
    {
        constexpr v4_dtl::op_params result = v4_dtl::params_of(
            ^^params_of_unary_1,
            std::meta::operators::op_minus,
            ^^params_of_unary_1::operator-,
            v4_dtl::member::yes);
        CONSTEVAL_TEST(result == v4_dtl::op_params::t_t);
    }
    consteval
    {
        constexpr v4_dtl::op_params result = v4_dtl::params_of(
            ^^params_of_unary_1,
            std::meta::operators::op_plus_plus,
            ^^params_of_unary_1::operator++,
            v4_dtl::member::yes);
        CONSTEVAL_TEST(result == v4_dtl::op_params::post);
    }
    consteval
    {
        constexpr v4_dtl::op_params result = v4_dtl::params_of(
            ^^params_of_unary_1,
            std::meta::operators::op_slash,
            ^^params_of_unary_1::operator/,
            v4_dtl::member::yes);
        CONSTEVAL_TEST(result == v4_dtl::op_params::t_n);
    }
    consteval
    {
        constexpr v4_dtl::op_params result = v4_dtl::params_of(
            ^^params_of_unary_1,
            std::meta::operators::op_percent,
            ^^params_of_unary_1::operator%,
            v4_dtl::member::yes);
        CONSTEVAL_TEST(result == v4_dtl::op_params::other);
    }
    consteval
    {
        constexpr v4_dtl::op_params result = v4_dtl::params_of(
            ^^params_of_unary_2,
            std::meta::operators::op_plus,
            ^^params_of_unary_2::operator+,
            v4_dtl::member::yes);
        CONSTEVAL_TEST(result == v4_dtl::op_params::t_n);
    }

    // empty
    consteval
    {
        constexpr v4_dtl::has_op_t has_op(^^empty);
        constexpr auto aliases = has_op.aliases();
        CONSTEVAL_TEST(aliases.reference == v4_dtl::iterator_alias{});
        CONSTEVAL_TEST(aliases.value_type == v4_dtl::iterator_alias{});
        CONSTEVAL_TEST(aliases.pointer == v4_dtl::iterator_alias{});
        CONSTEVAL_TEST(aliases.difference_type == v4_dtl::iterator_alias{});
    }

    // type alias detection
    consteval
    {
        constexpr v4_dtl::has_op_t has_op(^^reference_only_1);
        constexpr auto aliases = has_op.aliases();
        CONSTEVAL_TEST(
            aliases.reference == (v4_dtl::iterator_alias{^^int &, true}));
        CONSTEVAL_TEST(aliases.value_type == v4_dtl::iterator_alias{});
        CONSTEVAL_TEST(aliases.pointer == v4_dtl::iterator_alias{});
        CONSTEVAL_TEST(aliases.difference_type == v4_dtl::iterator_alias{});
    }
    consteval
    {
        constexpr v4_dtl::has_op_t has_op(^^reference_only_2);
        constexpr auto aliases = has_op.aliases();
        CONSTEVAL_TEST(
            aliases.reference ==
            (v4_dtl::iterator_alias{^^std::tuple<int &, int &>, true}));
        CONSTEVAL_TEST(aliases.value_type == v4_dtl::iterator_alias{});
        CONSTEVAL_TEST(aliases.pointer == v4_dtl::iterator_alias{});
        CONSTEVAL_TEST(aliases.difference_type == v4_dtl::iterator_alias{});
    }
    consteval
    {
        constexpr v4_dtl::has_op_t has_op(^^value_type_only);
        constexpr auto aliases = has_op.aliases();
        CONSTEVAL_TEST(aliases.reference == v4_dtl::iterator_alias{});
        CONSTEVAL_TEST(
            aliases.value_type == (v4_dtl::iterator_alias{^^int const, true}));
        CONSTEVAL_TEST(aliases.pointer == v4_dtl::iterator_alias{});
        CONSTEVAL_TEST(aliases.difference_type == v4_dtl::iterator_alias{});
    }
    consteval
    {
        constexpr v4_dtl::has_op_t has_op(^^pointer_only);
        constexpr auto aliases = has_op.aliases();
        CONSTEVAL_TEST(aliases.reference == v4_dtl::iterator_alias{});
        CONSTEVAL_TEST(aliases.value_type == v4_dtl::iterator_alias{});
        CONSTEVAL_TEST(
            aliases.pointer == (v4_dtl::iterator_alias{^^int *, true}));
        CONSTEVAL_TEST(aliases.difference_type == v4_dtl::iterator_alias{});
    }
    consteval
    {
        constexpr v4_dtl::has_op_t has_op(^^difference_type_only);
        constexpr auto aliases = has_op.aliases();
        CONSTEVAL_TEST(aliases.reference == v4_dtl::iterator_alias{});
        CONSTEVAL_TEST(aliases.value_type == v4_dtl::iterator_alias{});
        CONSTEVAL_TEST(aliases.pointer == v4_dtl::iterator_alias{});
        CONSTEVAL_TEST(
            aliases.difference_type ==
            (v4_dtl::iterator_alias{dealias(^^std::ptrdiff_t), true}));
    }
    consteval
    {
        constexpr v4_dtl::has_op_t has_op(^^udt_zip_iter);
        constexpr auto aliases = has_op.aliases();
        CONSTEVAL_TEST(
            aliases.reference ==
            (v4_dtl::iterator_alias{^^std::tuple<int_t &, int &>, true}));
        CONSTEVAL_TEST(
            aliases.value_type ==
            (v4_dtl::iterator_alias{^^std::tuple<int_t, int>, true}));
        CONSTEVAL_TEST(aliases.pointer == v4_dtl::iterator_alias{});
        CONSTEVAL_TEST(aliases.difference_type == v4_dtl::iterator_alias{});
    }

    // wrong-arity op*, op-, and op+ ignored
    consteval
    {
        constexpr v4_dtl::has_op_t has_op(^^wrong_ops);
        constexpr auto aliases = has_op.aliases();
        CONSTEVAL_TEST(aliases.reference == v4_dtl::iterator_alias{});
        CONSTEVAL_TEST(aliases.value_type == v4_dtl::iterator_alias{});
        CONSTEVAL_TEST(aliases.pointer == v4_dtl::iterator_alias{});
        CONSTEVAL_TEST(aliases.difference_type == v4_dtl::iterator_alias{});
        CONSTEVAL_TEST(!has_op(std::meta::operators::op_star));
        CONSTEVAL_TEST(!has_op(std::meta::operators::op_minus));
        CONSTEVAL_TEST(!has_op(std::meta::operators::op_plus));
        CONSTEVAL_TEST(!has_op(std::meta::operators::op_plus_plus));
        CONSTEVAL_TEST(!has_op(
            std::meta::operators::op_plus_plus, v4_dtl::op_params::post));
        CONSTEVAL_TEST(!has_op(std::meta::operators::op_minus_minus));
        CONSTEVAL_TEST(!has_op(
            std::meta::operators::op_minus_minus, v4_dtl::op_params::post));
    }

    // binary op+ detected
    consteval
    {
        constexpr v4_dtl::has_op_t has_op(^^member_binary_plus_only);
        constexpr auto aliases = has_op.aliases();
        CONSTEVAL_TEST(aliases.reference == v4_dtl::iterator_alias{});
        CONSTEVAL_TEST(aliases.value_type == v4_dtl::iterator_alias{});
        CONSTEVAL_TEST(aliases.pointer == v4_dtl::iterator_alias{});
        CONSTEVAL_TEST(aliases.difference_type == v4_dtl::iterator_alias{});
        CONSTEVAL_TEST(has_op(std::meta::operators::op_plus));
    }
    consteval
    {
        constexpr v4_dtl::has_op_t has_op(^^nonmember_binary_plus_only);
        constexpr auto aliases = has_op.aliases();
        CONSTEVAL_TEST(aliases.reference == v4_dtl::iterator_alias{});
        CONSTEVAL_TEST(aliases.value_type == v4_dtl::iterator_alias{});
        CONSTEVAL_TEST(aliases.pointer == v4_dtl::iterator_alias{});
        CONSTEVAL_TEST(aliases.difference_type == v4_dtl::iterator_alias{});
        CONSTEVAL_TEST(has_op(std::meta::operators::op_plus));
    }

    // binary minus detected, but not the one that computes a difference
    consteval
    {
        constexpr v4_dtl::has_op_t has_op(^^nondifference_member_binary_minus);
        constexpr auto aliases = has_op.aliases();
        CONSTEVAL_TEST(aliases.reference == v4_dtl::iterator_alias{});
        CONSTEVAL_TEST(aliases.value_type == v4_dtl::iterator_alias{});
        CONSTEVAL_TEST(aliases.pointer == v4_dtl::iterator_alias{});
        CONSTEVAL_TEST(aliases.difference_type == v4_dtl::iterator_alias{});
        CONSTEVAL_TEST(has_op(std::meta::operators::op_minus));
    }
    consteval
    {
        constexpr v4_dtl::has_op_t has_op(
            ^^nondifference_nonmember_binary_minus_1);
        constexpr auto aliases = has_op.aliases();
        CONSTEVAL_TEST(aliases.reference == v4_dtl::iterator_alias{});
        CONSTEVAL_TEST(aliases.value_type == v4_dtl::iterator_alias{});
        CONSTEVAL_TEST(aliases.pointer == v4_dtl::iterator_alias{});
        CONSTEVAL_TEST(aliases.difference_type == v4_dtl::iterator_alias{});
        CONSTEVAL_TEST(has_op(std::meta::operators::op_minus));
    }
    consteval
    {
        constexpr v4_dtl::has_op_t has_op(
            ^^nondifference_nonmember_binary_minus_2);
        constexpr auto aliases = has_op.aliases();
        CONSTEVAL_TEST(aliases.reference == v4_dtl::iterator_alias{});
        CONSTEVAL_TEST(aliases.value_type == v4_dtl::iterator_alias{});
        CONSTEVAL_TEST(aliases.pointer == v4_dtl::iterator_alias{});
        CONSTEVAL_TEST(aliases.difference_type == v4_dtl::iterator_alias{});
        CONSTEVAL_TEST(has_op(std::meta::operators::op_minus));
    }

    // binary minus detected
    consteval
    {
        constexpr v4_dtl::has_op_t has_op(^^member_binary_minus_only);
        constexpr auto aliases = has_op.aliases();
        CONSTEVAL_TEST(aliases.reference == v4_dtl::iterator_alias{});
        CONSTEVAL_TEST(aliases.value_type == v4_dtl::iterator_alias{});
        CONSTEVAL_TEST(aliases.pointer == v4_dtl::iterator_alias{});
        CONSTEVAL_TEST(
            aliases.difference_type == (v4_dtl::iterator_alias{^^int, false}));
        CONSTEVAL_TEST(has_op(std::meta::operators::op_minus));
    }
    consteval
    {
        constexpr v4_dtl::has_op_t has_op(
            ^^member_binary_minus_only_auto_return);
        constexpr auto aliases = has_op.aliases();
        CONSTEVAL_TEST(aliases.reference == v4_dtl::iterator_alias{});
        CONSTEVAL_TEST(aliases.value_type == v4_dtl::iterator_alias{});
        CONSTEVAL_TEST(aliases.pointer == v4_dtl::iterator_alias{});
        CONSTEVAL_TEST(
            aliases.difference_type == (v4_dtl::iterator_alias{^^int, false}));
        CONSTEVAL_TEST(has_op(std::meta::operators::op_minus));
    }
    consteval
    {
        constexpr v4_dtl::has_op_t has_op(^^nonmember_binary_minus_only);
        constexpr auto aliases = has_op.aliases();
        CONSTEVAL_TEST(aliases.reference == v4_dtl::iterator_alias{});
        CONSTEVAL_TEST(aliases.value_type == v4_dtl::iterator_alias{});
        CONSTEVAL_TEST(aliases.pointer == v4_dtl::iterator_alias{});
        CONSTEVAL_TEST(
            aliases.difference_type == (v4_dtl::iterator_alias{^^int, false}));
        CONSTEVAL_TEST(has_op(std::meta::operators::op_minus));
    }
    consteval
    {
        constexpr v4_dtl::has_op_t has_op(
            ^^nonmember_binary_minus_only_auto_return);
        constexpr auto aliases = has_op.aliases();
        CONSTEVAL_TEST(aliases.reference == v4_dtl::iterator_alias{});
        CONSTEVAL_TEST(aliases.value_type == v4_dtl::iterator_alias{});
        CONSTEVAL_TEST(aliases.pointer == v4_dtl::iterator_alias{});
        CONSTEVAL_TEST(
            aliases.difference_type == (v4_dtl::iterator_alias{^^int, false}));
        CONSTEVAL_TEST(has_op(std::meta::operators::op_minus));
    }
    consteval
    {
        constexpr v4_dtl::has_op_t has_op(
            ^^template_member_binary_minus_only<int>);
        constexpr auto aliases = has_op.aliases();
        CONSTEVAL_TEST(aliases.reference == v4_dtl::iterator_alias{});
        CONSTEVAL_TEST(aliases.value_type == v4_dtl::iterator_alias{});
        CONSTEVAL_TEST(aliases.pointer == v4_dtl::iterator_alias{});
        CONSTEVAL_TEST(
            aliases.difference_type == (v4_dtl::iterator_alias{^^int, false}));
        CONSTEVAL_TEST(has_op(std::meta::operators::op_minus));
    }

    // pre-/post-incr-/decrement detected
    consteval
    {
        constexpr v4_dtl::has_op_t has_op(^^member_pre_post_incr);
        constexpr auto aliases = has_op.aliases();
        CONSTEVAL_TEST(aliases.reference == v4_dtl::iterator_alias{});
        CONSTEVAL_TEST(aliases.value_type == v4_dtl::iterator_alias{});
        CONSTEVAL_TEST(aliases.pointer == v4_dtl::iterator_alias{});
        CONSTEVAL_TEST(aliases.difference_type == v4_dtl::iterator_alias{});
        CONSTEVAL_TEST(
            has_op(std::meta::operators::op_plus_plus, v4_dtl::op_params::pre));
        CONSTEVAL_TEST(has_op(
            std::meta::operators::op_plus_plus, v4_dtl::op_params::post));
    }
    consteval
    {
        constexpr v4_dtl::has_op_t has_op(^^nonmember_pre_post_incr);
        constexpr auto aliases = has_op.aliases();
        CONSTEVAL_TEST(aliases.reference == v4_dtl::iterator_alias{});
        CONSTEVAL_TEST(aliases.value_type == v4_dtl::iterator_alias{});
        CONSTEVAL_TEST(aliases.pointer == v4_dtl::iterator_alias{});
        CONSTEVAL_TEST(aliases.difference_type == v4_dtl::iterator_alias{});
        CONSTEVAL_TEST(has_op(std::meta::operators::op_plus_plus));
        CONSTEVAL_TEST(has_op(
            std::meta::operators::op_plus_plus, v4_dtl::op_params::post));
    }
    consteval
    {
        constexpr v4_dtl::has_op_t has_op(^^member_pre_post_decr);
        constexpr auto aliases = has_op.aliases();
        CONSTEVAL_TEST(aliases.reference == v4_dtl::iterator_alias{});
        CONSTEVAL_TEST(aliases.value_type == v4_dtl::iterator_alias{});
        CONSTEVAL_TEST(aliases.pointer == v4_dtl::iterator_alias{});
        CONSTEVAL_TEST(aliases.difference_type == v4_dtl::iterator_alias{});
        CONSTEVAL_TEST(has_op(std::meta::operators::op_minus_minus));
        CONSTEVAL_TEST(has_op(
            std::meta::operators::op_minus_minus, v4_dtl::op_params::post));
    }
    consteval
    {
        constexpr v4_dtl::has_op_t has_op(^^nonmember_pre_post_decr);
        constexpr auto aliases = has_op.aliases();
        CONSTEVAL_TEST(aliases.reference == v4_dtl::iterator_alias{});
        CONSTEVAL_TEST(aliases.value_type == v4_dtl::iterator_alias{});
        CONSTEVAL_TEST(aliases.pointer == v4_dtl::iterator_alias{});
        CONSTEVAL_TEST(aliases.difference_type == v4_dtl::iterator_alias{});
        CONSTEVAL_TEST(has_op(std::meta::operators::op_minus_minus));
        CONSTEVAL_TEST(has_op(
            std::meta::operators::op_minus_minus, v4_dtl::op_params::post));
    }

    // this tests parts of detect_adapted_ops()
    consteval
    {
        v4_dtl::has_op_t has_op(^^adapted_deref_subtract);
        constexpr auto adapted = ^^adapted_deref_subtract::it_;
        constexpr auto adapted_t = type_of(adapted);

        v4_dtl::detect_adapted_ops(adapted, has_op);

        constexpr bool adapted_deref = extract<bool>(substitute(
            ^^v4_dtl::adapted_iter_can_deref,
            {
                adapted_t}));
        if (!adapted_deref)
            throw std::runtime_error("!adapted_deref");
        constexpr bool adapted_subtract = extract<bool>(substitute(
            ^^v4_dtl::adapted_iter_can_subtract_unknown_result,
            {
                adapted_t}));
        if (!adapted_subtract)
            throw std::runtime_error("!adapted_subtract");

        constexpr auto reference_alias = dealias(substitute(
            ^^v4_dtl::adapted_iter_deref_result,
            {
                adapted_t}));
        CONSTEVAL_TEST(reference_alias == ^^int &);

        constexpr auto difference_type_alias = dealias(substitute(
            ^^v4_dtl::adapted_iter_subtract_result,
            {
                adapted_t}));
        CONSTEVAL_TEST(difference_type_alias == dealias(^^std::ptrdiff_t));
    }

    // detect_adapted_ops()
    consteval
    {
        v4_dtl::has_op_t has_op(^^adapted_deref_subtract);
        v4_dtl::detect_adapted_ops(^^adapted_deref_subtract::it_, has_op);
        auto aliases = has_op.aliases();
        CONSTEVAL_TEST(
            aliases.reference == (v4_dtl::iterator_alias{^^int &, false}));
        CONSTEVAL_TEST(aliases.value_type == v4_dtl::iterator_alias{});
        CONSTEVAL_TEST(aliases.pointer == v4_dtl::iterator_alias{});
        CONSTEVAL_TEST(
            aliases.difference_type ==
            (v4_dtl::iterator_alias{dealias(^^std::ptrdiff_t), false}));
    }
    consteval
    {
        v4_dtl::has_op_t has_op(^^adapted_deref_subtract_defined_reference);
        v4_dtl::detect_adapted_ops(
            ^^adapted_deref_subtract_defined_reference::it_, has_op);
        auto aliases = has_op.aliases();
        CONSTEVAL_TEST(
            aliases.reference == (v4_dtl::iterator_alias{^^int const &, true}));
        CONSTEVAL_TEST(aliases.value_type == v4_dtl::iterator_alias{});
        CONSTEVAL_TEST(aliases.pointer == v4_dtl::iterator_alias{});
        CONSTEVAL_TEST(
            aliases.difference_type ==
            (v4_dtl::iterator_alias{dealias(^^std::ptrdiff_t), false}));
    }
    consteval
    {
        v4_dtl::has_op_t has_op(
            ^^adapted_deref_subtract_defined_difference_type);
        v4_dtl::detect_adapted_ops(
            ^^adapted_deref_subtract_defined_difference_type::it_, has_op);
        auto aliases = has_op.aliases();
        CONSTEVAL_TEST(
            aliases.reference == (v4_dtl::iterator_alias{^^int &, false}));
        CONSTEVAL_TEST(aliases.value_type == v4_dtl::iterator_alias{});
        CONSTEVAL_TEST(aliases.pointer == v4_dtl::iterator_alias{});
        CONSTEVAL_TEST(
            aliases.difference_type == (v4_dtl::iterator_alias{^^int, true}));
    }

    return boost::report_errors();
}

#endif
