// Copyright (C) 2019 T. Zachary Laine
//
// Distributed under the Boost Software License, Version 1.0. (See
// accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
#define USE_V2
#include "../example/static_vector.hpp"

#include "ill_formed.hpp"

#include <boost/test/minimal.hpp>

#include <array>


// Instantiate all the members we can.
template struct static_vector<int, 1024>;

using vec_type = static_vector<int, 10>;


int test_main(int, char * [])
{

{
    vec_type v;
    BOOST_CHECK(v.empty());
    BOOST_CHECK(v.size() == 0u);

    BOOST_CHECK(v.max_size() == 10u);
    BOOST_CHECK(v.capacity() == 10u);

    BOOST_CHECK(v == v);
    BOOST_CHECK(v <= v);
    BOOST_CHECK(v >= v);

    BOOST_CHECK_THROW(v.at(0), std::out_of_range);

    vec_type const & cv = v;
    BOOST_CHECK(cv.empty());
    BOOST_CHECK(cv.size() == 0u);

    BOOST_CHECK(cv.max_size() == 10u);
    BOOST_CHECK(cv.capacity() == 10u);

    BOOST_CHECK(cv == cv);
    BOOST_CHECK(cv <= cv);
    BOOST_CHECK(cv >= cv);

    BOOST_CHECK_THROW(cv.at(0), std::out_of_range);
}


{
    {
        vec_type v(3);
        BOOST_CHECK(!v.empty());
        BOOST_CHECK(v.size() == 3u);

        vec_type v2(std::initializer_list<int>{0, 0, 0});
        BOOST_CHECK(v == v2);
    }

    {
        std::initializer_list<int> il{3, 2, 1};
        vec_type v(il);
        BOOST_CHECK(!v.empty());
        BOOST_CHECK(v.size() == 3u);

        static_assert(std::is_same<decltype(v = il), vec_type &>::value, "");

        vec_type v2;
        v2 = il;
        BOOST_CHECK(v == v2);
    }

    {
        std::initializer_list<int> il{3, 2, 1};
        vec_type v;
        v.assign(il);
        BOOST_CHECK(!v.empty());
        BOOST_CHECK(v.size() == 3u);

        static_assert(std::is_same<decltype(v.assign(il)), void>::value, "");

        vec_type v2;
        v2 = il;
        BOOST_CHECK(v == v2);
    }

    {
        vec_type v(3, 4);
        BOOST_CHECK(!v.empty());
        BOOST_CHECK(v.size() == 3u);

        vec_type v2 = {4, 4, 4};
        BOOST_CHECK(v == v2);
    }

    {
        vec_type v;
        v.assign(3, 4);
        BOOST_CHECK(!v.empty());
        BOOST_CHECK(v.size() == 3u);

        static_assert(std::is_same<decltype(v.assign(3, 4)), void>::value, "");

        vec_type v2 = {4, 4, 4};
        BOOST_CHECK(v == v2);
    }

    {
        std::array<int, 3> a = {{1, 2, 3}};

        vec_type v(a.begin(), a.end());
        BOOST_CHECK(!v.empty());
        BOOST_CHECK(v.size() == 3u);

        vec_type v2 = {1, 2, 3};
        BOOST_CHECK(v == v2);
    }

    {
        std::array<int, 3> a = {{1, 2, 3}};

        vec_type v;
        v.assign(a.begin(), a.end());
        BOOST_CHECK(!v.empty());
        BOOST_CHECK(v.size() == 3u);

        static_assert(
            std::is_same<decltype(v.assign(a.begin(), a.end())), void>::value,
            "");

        vec_type v2 = {1, 2, 3};
        BOOST_CHECK(v == v2);
    }
}


{
    {
        vec_type v;

        static_assert(std::is_same<decltype(v.resize(1)), void>::value, "");

        v.resize(3);
        BOOST_CHECK(v == vec_type(3));

        v.resize(6);
        BOOST_CHECK(v == vec_type(6));
    }

    {
        vec_type v(6);

        v.resize(3);
        BOOST_CHECK(v == vec_type(3));

        v.resize(0);
        BOOST_CHECK(v == vec_type{});
    }
}


{
    {
        vec_type v2 = {4, 4, 4};

        vec_type v(v2);
        BOOST_CHECK(v == v2);
    }

    {
        vec_type v;
        vec_type v2 = {4, 4, 4};

        static_assert(std::is_same<decltype(v = v2), vec_type &>::value, "");
        static_assert(
            std::is_same<decltype(v = std::move(v2)), vec_type &>::value, "");

        v = v2;
        BOOST_CHECK(v == v2);
    }

    {
        vec_type v2 = {4, 4, 4};

        vec_type v(std::move(v2));
        BOOST_CHECK(v == (vec_type(3, 4)));
        BOOST_CHECK(v2.empty());
    }

    {
        vec_type v;
        vec_type v2 = {4, 4, 4};

        v = std::move(v2);
        BOOST_CHECK(v == (vec_type(3, 4)));
        BOOST_CHECK(v2.empty());
    }
}


{
    vec_type sm = {1, 2, 3};
    vec_type md = {1, 2, 3, 4};
    vec_type lg = {1, 2, 3, 4, 5};

    BOOST_CHECK(sm == sm);
    BOOST_CHECK(!(sm == md));
    BOOST_CHECK(!(sm == lg));

    BOOST_CHECK(!(sm != sm));
    BOOST_CHECK(sm != md);
    BOOST_CHECK(sm != lg);

    BOOST_CHECK(!(sm < sm));
    BOOST_CHECK(sm < md);
    BOOST_CHECK(sm < lg);

    BOOST_CHECK(sm <= sm);
    BOOST_CHECK(sm <= md);
    BOOST_CHECK(sm <= lg);

    BOOST_CHECK(!(sm > sm));
    BOOST_CHECK(!(sm > md));
    BOOST_CHECK(!(sm > lg));

    BOOST_CHECK(sm >= sm);
    BOOST_CHECK(!(sm >= md));
    BOOST_CHECK(!(sm >= lg));


    BOOST_CHECK(!(md == sm));
    BOOST_CHECK(md == md);
    BOOST_CHECK(!(md == lg));

    BOOST_CHECK(!(md < sm));
    BOOST_CHECK(!(md < md));
    BOOST_CHECK(md < lg);

    BOOST_CHECK(!(md <= sm));
    BOOST_CHECK(md <= md);
    BOOST_CHECK(md <= lg);

    BOOST_CHECK(md > sm);
    BOOST_CHECK(!(md > md));
    BOOST_CHECK(!(md > lg));

    BOOST_CHECK(md >= sm);
    BOOST_CHECK(md >= md);
    BOOST_CHECK(!(md >= lg));


    BOOST_CHECK(!(lg == sm));
    BOOST_CHECK(!(lg == md));
    BOOST_CHECK(lg == lg);

    BOOST_CHECK(!(lg < sm));
    BOOST_CHECK(!(lg < md));
    BOOST_CHECK(!(lg < lg));

    BOOST_CHECK(!(lg <= sm));
    BOOST_CHECK(!(lg <= md));
    BOOST_CHECK(lg <= lg);

    BOOST_CHECK(lg > sm);
    BOOST_CHECK(lg > md);
    BOOST_CHECK(!(lg > lg));

    BOOST_CHECK(lg >= sm);
    BOOST_CHECK(lg >= md);
    BOOST_CHECK(lg >= lg);
}


{
    {
        vec_type v1(3, 4);
        vec_type v2(4, 3);

        static_assert(std::is_same<decltype(v1.swap(v2)), void>::value, "");
        static_assert(std::is_same<decltype(swap(v1, v2)), void>::value, "");

        v1.swap(v2);

        BOOST_CHECK(v1.size() == 4u);
        BOOST_CHECK(v2.size() == 3u);

        BOOST_CHECK(v1 == vec_type(4, 3));
        BOOST_CHECK(v2 == vec_type(3, 4));
    }

    {
        vec_type v1(3, 4);
        vec_type v2(4, 3);

        swap(v1, v2);

        BOOST_CHECK(v1.size() == 4u);
        BOOST_CHECK(v2.size() == 3u);

        BOOST_CHECK(v1 == vec_type(4, 3));
        BOOST_CHECK(v2 == vec_type(3, 4));
    }
}

template<typename Iter>
using writable_iter_t = decltype(
    *std::declval<Iter>() =
        std::declval<typename std::iterator_traits<Iter>::value_type>());

static_assert(
    ill_formed<
        writable_iter_t,
        decltype(std::declval<vec_type const &>().begin())>::value,
    "");
static_assert(
    ill_formed<
        writable_iter_t,
        decltype(std::declval<vec_type const &>().end())>::value,
    "");
static_assert(
    ill_formed<writable_iter_t, decltype(std::declval<vec_type &>().cbegin())>::
        value,
    "");
static_assert(
    ill_formed<writable_iter_t, decltype(std::declval<vec_type &>().cend())>::
        value,
    "");

static_assert(
    ill_formed<
        writable_iter_t,
        decltype(std::declval<vec_type const &>().rbegin())>::value,
    "");
static_assert(
    ill_formed<
        writable_iter_t,
        decltype(std::declval<vec_type const &>().rend())>::value,
    "");
static_assert(
    ill_formed<
        writable_iter_t,
        decltype(std::declval<vec_type &>().crbegin())>::value,
    "");
static_assert(
    ill_formed<writable_iter_t, decltype(std::declval<vec_type &>().crend())>::
        value,
    "");


{
    {
        vec_type v = {3, 2, 1};

        static_assert(
            std::is_same<decltype(v.begin()), vec_type::iterator>::value, "");
        static_assert(
            std::is_same<decltype(v.end()), vec_type::iterator>::value, "");
        static_assert(
            std::is_same<decltype(v.cbegin()), vec_type::const_iterator>::value,
            "");
        static_assert(
            std::is_same<decltype(v.cend()), vec_type::const_iterator>::value,
            "");
        static_assert(
            std::is_same<decltype(v.rbegin()), vec_type::reverse_iterator>::
                value,
            "");
        static_assert(
            std::is_same<decltype(v.rbegin()), vec_type::reverse_iterator>::
                value,
            "");
        static_assert(
            std::is_same<
                decltype(v.crbegin()),
                vec_type::const_reverse_iterator>::value,
            "");
        static_assert(
            std::is_same<
                decltype(v.crbegin()),
                vec_type::const_reverse_iterator>::value,
            "");

        std::array<int, 3> const a = {{3, 2, 1}};
	std::array<int, 3> const ra = {{1, 2, 3}};

        BOOST_CHECK(std::equal(v.begin(), v.end(), a.begin(), a.end()));
        BOOST_CHECK(std::equal(v.cbegin(), v.cend(), a.begin(), a.end()));

        BOOST_CHECK(std::equal(v.rbegin(), v.rend(), ra.begin(), ra.end()));
        BOOST_CHECK(std::equal(v.crbegin(), v.crend(), ra.begin(), ra.end()));

        *v.begin() = 8;
        *v.rbegin() = 9;
        BOOST_CHECK(v == vec_type({8, 2, 9}));
    }

    {
        vec_type const v = {3, 2, 1};

        static_assert(
            std::is_same<decltype(v.begin()), vec_type::const_iterator>::value,
            "");
        static_assert(
            std::is_same<decltype(v.end()), vec_type::const_iterator>::value,
            "");
        static_assert(
            std::is_same<decltype(v.cbegin()), vec_type::const_iterator>::value,
            "");
        static_assert(
            std::is_same<decltype(v.cend()), vec_type::const_iterator>::value,
            "");
        static_assert(
            std::is_same<
                decltype(v.rbegin()),
                vec_type::const_reverse_iterator>::value,
            "");
        static_assert(
            std::is_same<
                decltype(v.rbegin()),
                vec_type::const_reverse_iterator>::value,
            "");
        static_assert(
            std::is_same<
                decltype(v.crbegin()),
                vec_type::const_reverse_iterator>::value,
            "");
        static_assert(
            std::is_same<
                decltype(v.crbegin()),
                vec_type::const_reverse_iterator>::value,
            "");

        std::array<int, 3> const a = {{3, 2, 1}};
        std::array<int, 3> const ra = {{1, 2, 3}};

        BOOST_CHECK(std::equal(v.begin(), v.end(), a.begin(), a.end()));
        BOOST_CHECK(std::equal(v.cbegin(), v.cend(), a.begin(), a.end()));

        BOOST_CHECK(std::equal(v.rbegin(), v.rend(), ra.begin(), ra.end()));
        BOOST_CHECK(std::equal(v.crbegin(), v.crend(), ra.begin(), ra.end()));
    }
}


{
    {
        vec_type v;

        int const i = 0;
        static_assert(
            std::is_same<decltype(v.emplace_back(0)), vec_type::reference>::
                value,
            "");
        static_assert(
            std::is_same<decltype(v.emplace_back(i)), vec_type::reference>::
                value,
            "");

        v.emplace_back(i);
        BOOST_CHECK(v.front() == i);
        BOOST_CHECK(v.back() == i);

        v.emplace_back(1);
        BOOST_CHECK(v.front() == i);
        BOOST_CHECK(v.back() == 1);

        v.emplace_back(2);
        BOOST_CHECK(v.front() == i);
        BOOST_CHECK(v.back() == 2);

        BOOST_CHECK(v == vec_type({0, 1, 2}));
    }

    {
        vec_type v = {1, 2};

        int const i = 0;
        static_assert(
            std::is_same<
                decltype(v.emplace(v.begin(), 0)),
                vec_type::iterator>::value,
            "");
        static_assert(
            std::is_same<
                decltype(v.emplace(v.begin(), i)),
                vec_type::iterator>::value,
            "");

        v.emplace(v.begin(), i);
        BOOST_CHECK(v == vec_type({0, 1, 2}));

        v.emplace(v.end(), 3);
        BOOST_CHECK(v == vec_type({0, 1, 2, 3}));

        v.emplace(v.begin() + 2, 9);
        BOOST_CHECK(v == vec_type({0, 1, 9, 2, 3}));
    }

    {
        vec_type v = {1, 2};

        std::array<int, 2> a1 = {{0, 0}};
        std::array<int, 1> a2 = {{3}};
        std::array<int, 3> a3 = {{9, 9, 9}};

        static_assert(
            std::is_same<
                decltype(v.insert(v.begin(), a1.begin(), a1.end())),
                vec_type::iterator>::value,
            "");

        auto const it0 = v.insert(v.begin(), a1.begin(), a1.end());
        BOOST_CHECK(v == vec_type({0, 0, 1, 2}));
        BOOST_CHECK(it0 == v.begin());

        auto const it1 = v.insert(v.end(), a2.begin(), a2.end());
        BOOST_CHECK(v == vec_type({0, 0, 1, 2, 3}));
        BOOST_CHECK(it1 == v.begin() + 4);

        auto const it2 = v.insert(v.begin() + 2, a3.begin(), a3.end());
        BOOST_CHECK(v == vec_type({0, 0, 9, 9, 9, 1, 2, 3}));
        BOOST_CHECK(it2 == v.begin() + 2);
    }

    {
        vec_type v = {1, 2};

        int const i = 0;
        static_assert(
            std::is_same<decltype(v.insert(v.begin(), 0)), vec_type::iterator>::
                value,
            "");
        static_assert(
            std::is_same<decltype(v.insert(v.begin(), i)), vec_type::iterator>::
                value,
            "");

        v.insert(v.begin(), i);
        BOOST_CHECK(v == vec_type({0, 1, 2}));

        v.insert(v.end(), 3);
        BOOST_CHECK(v == vec_type({0, 1, 2, 3}));

        v.insert(v.begin() + 2, 9);
        BOOST_CHECK(v == vec_type({0, 1, 9, 2, 3}));
    }

    {
        vec_type v = {1, 2};

        static_assert(
            std::is_same<
                decltype(v.insert(v.begin(), 3, 0)),
                vec_type::iterator>::value,
            "");

        v.insert(v.begin(), 2, 0);
        BOOST_CHECK(v == vec_type({0, 0, 1, 2}));

        v.insert(v.end(), 1, 3);
        BOOST_CHECK(v == vec_type({0, 0, 1, 2, 3}));

        v.insert(v.begin() + 2, 3, 9);
        BOOST_CHECK(v == vec_type({0, 0, 9, 9, 9, 1, 2, 3}));
    }

    {
        vec_type v = {1, 2};

        static_assert(
            std::is_same<
                decltype(v.insert(v.begin(), std::initializer_list<int>{0, 0})),
                vec_type::iterator>::value,
            "");

        v.insert(v.begin(), std::initializer_list<int>{0, 0});
        BOOST_CHECK(v == vec_type({0, 0, 1, 2}));

        v.insert(v.end(), std::initializer_list<int>{3});
        BOOST_CHECK(v == vec_type({0, 0, 1, 2, 3}));

        v.insert(v.begin() + 2, std::initializer_list<int>{9, 9, 9});
        BOOST_CHECK(v == vec_type({0, 0, 9, 9, 9, 1, 2, 3}));
    }
}


{
    {
        vec_type v = {3, 2, 1};

        static_assert(
            std::is_same<
                decltype(v.erase(v.begin(), v.end())),
                vec_type::iterator>::value,
            "");
        static_assert(
            std::is_same<decltype(v.erase(v.begin())), vec_type::iterator>::
                value,
            "");

        v.erase(v.begin(), v.end());
        BOOST_CHECK(v.empty());
        BOOST_CHECK(v.size() == 0u);
    }

    {
        vec_type v = {3, 2, 1};
        v.erase(v.begin() + 1, v.end());
        BOOST_CHECK(!v.empty());
        BOOST_CHECK(v.size() == 1u);
        BOOST_CHECK(v == vec_type(1, 3));
    }

    {
        vec_type v = {3, 2, 1};
        v.erase(v.begin(), v.end() - 1);
        BOOST_CHECK(!v.empty());
        BOOST_CHECK(v.size() == 1u);
        BOOST_CHECK(v == vec_type(1, 1));
    }

    {
        vec_type v = {3, 2, 1};
        v.erase(v.begin());
        BOOST_CHECK(!v.empty());
        BOOST_CHECK(v.size() == 2u);
        BOOST_CHECK(v == vec_type({2, 1}));
    }

    {
        vec_type v = {3, 2, 1};
        v.erase(v.begin() + 1);
        BOOST_CHECK(!v.empty());
        BOOST_CHECK(v.size() == 2u);
        BOOST_CHECK(v == vec_type({3, 1}));
    }

    {
        vec_type v = {3, 2, 1};
        v.erase(v.begin() + 2);
        BOOST_CHECK(!v.empty());
        BOOST_CHECK(v.size() == 2u);
        BOOST_CHECK(v == vec_type({3, 2}));
    }
}

template<
    typename Container,
    typename ValueType = typename Container::value_type>
using lvalue_push_front_t = decltype(
    std::declval<Container &>().push_front(std::declval<ValueType const &>()));
template<
    typename Container,
    typename ValueType = typename Container::value_type>
using rvalue_push_front_t = decltype(std::declval<Container &>().push_front(0));
template<typename Container>
using pop_front_t = decltype(std::declval<Container &>().pop_front());

static_assert(ill_formed<lvalue_push_front_t, vec_type>::value, "");
static_assert(ill_formed<rvalue_push_front_t, vec_type>::value, "");
static_assert(ill_formed<pop_front_t, vec_type>::value, "");


{
    {
        vec_type v;

        int const i = 0;
        static_assert(std::is_same<decltype(v.push_back(0)), void>::value, "");
        static_assert(std::is_same<decltype(v.push_back(i)), void>::value, "");
        static_assert(std::is_same<decltype(v.pop_back()), void>::value, "");

        v.push_back(i);
        BOOST_CHECK(v.front() == i);
        BOOST_CHECK(v.back() == i);

        v.push_back(1);
        BOOST_CHECK(v.front() == i);
        BOOST_CHECK(v.back() == 1);

        v.push_back(2);
        BOOST_CHECK(v.front() == i);
        BOOST_CHECK(v.back() == 2);

        static_assert(std::is_same<decltype(v.front()), int &>::value, "");
        static_assert(std::is_same<decltype(v.back()), int &>::value, "");

        v.front() = 9;
        v.back() = 8;
        BOOST_CHECK(v == vec_type({9, 1, 8}));

        v.pop_back();
        BOOST_CHECK(v == vec_type({9, 1}));
    }

    {
        vec_type const v = {3, 2, 1};
        BOOST_CHECK(v.front() == 3);
        BOOST_CHECK(v.back() == 1);

        static_assert(
            std::is_same<decltype(v.front()), int const &>::value, "");
        static_assert(std::is_same<decltype(v.back()), int const &>::value, "");
    }
}


{
    {
        vec_type v = {3, 2, 1};
        BOOST_CHECK(v.data()[0] == 3);
        BOOST_CHECK(v.data()[1] == 2);
        BOOST_CHECK(v.data()[2] == 1);
        BOOST_CHECK(v[0] == 3);
        BOOST_CHECK(v[1] == 2);
        BOOST_CHECK(v[2] == 1);
        BOOST_CHECK_NO_THROW(v.at(0));
        BOOST_CHECK_NO_THROW(v.at(1));
        BOOST_CHECK_NO_THROW(v.at(2));
        BOOST_CHECK_THROW(v.at(3), std::out_of_range);

        static_assert(std::is_same<decltype(v.data()), int *>::value, "");
        static_assert(std::is_same<decltype(v[0]), int &>::value, "");
        static_assert(std::is_same<decltype(v.at(0)), int &>::value, "");

        v[0] = 8;
        v.at(1) = 9;
        BOOST_CHECK(v == vec_type({8, 9, 1}));
    }

    {
        vec_type const v = {3, 2, 1};
        BOOST_CHECK(v.data()[0] == 3);
        BOOST_CHECK(v.data()[1] == 2);
        BOOST_CHECK(v.data()[2] == 1);
        BOOST_CHECK(v[0] == 3);
        BOOST_CHECK(v[1] == 2);
        BOOST_CHECK(v[2] == 1);
        BOOST_CHECK_NO_THROW(v.at(0));
        BOOST_CHECK_NO_THROW(v.at(1));
        BOOST_CHECK_NO_THROW(v.at(2));
        BOOST_CHECK_THROW(v.at(3), std::out_of_range);

        static_assert(std::is_same<decltype(v.data()), int const *>::value, "");
        static_assert(std::is_same<decltype(v[0]), int const &>::value, "");
        static_assert(std::is_same<decltype(v.at(0)), int const &>::value, "");
    }
}

    return 0;
}
