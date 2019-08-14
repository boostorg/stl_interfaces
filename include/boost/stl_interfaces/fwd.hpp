// Copyright (C) 2019 T. Zachary Laine
//
// Distributed under the Boost Software License, Version 1.0. (See
// accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
#ifndef BOOST_STL_INTERFACES_FWD_HPP
#define BOOST_STL_INTERFACES_FWD_HPP


#ifndef BOOST_STL_INTERFACES_DOXYGEN

#if defined(_MSC_VER) || defined(__GNUC__) && __GNUC__ < 8
#define BOOST_STL_INTERFACES_HIDDEN_FRIEND_CONSTEXPR
#else
#define BOOST_STL_INTERFACES_HIDDEN_FRIEND_CONSTEXPR constexpr
#endif

#endif


namespace boost { namespace stl_interfaces {

    /** An enumeration used to indicate whether the underlying data have a
        contiguous layout when instantiating `view_interface` and
        `container_interface`. */
    enum element_layout : bool { discontiguous, contiguous };

    namespace detail {
        template<typename... T>
        using void_t = void;

        template<typename T>
        struct dummy
        {
        };
    }

}}

#endif
