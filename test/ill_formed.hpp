// Copyright (C) 2019 T. Zachary Laine
//
// Distributed under the Boost Software License, Version 1.0. (See
// accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
#ifndef BOOST_STL_INTERFACES_ILL_FORMED_HPP
#define BOOST_STL_INTERFACES_ILL_FORMED_HPP


namespace test_detail {
    template<typename... T>
    using void_t = void;

    template<
        typename AlwaysVoid,
        template<class...> class Template,
        typename... Args>
    struct detector : std::true_type
    {
    };

    template<template<class...> class Template, typename... Args>
    struct detector<void_t<Template<Args...>>, Template, Args...>
        : std::false_type
    {
    };
}

template<template<class...> class Template, typename... Args>
using ill_formed = typename test_detail::detector<void, Template, Args...>;

#endif
