#include <boost/iterator_facade/iterator_facade.hpp>

#include <gtest/gtest.h>

#include <array>
#include <numeric>
#include <type_traits>


struct basic_random_access_iter : boost::iterator_facade::iterator_facade<
                                      basic_random_access_iter,
                                      std::random_access_iterator_tag,
                                      int>
{
    basic_random_access_iter() : it_(nullptr) {}
    basic_random_access_iter(int * it) : it_(it) {}

private:
    friend boost::iterator_facade::access;
    int & dereference() const { return *it_; }
    void advance(std::ptrdiff_t i) { it_ += i; }
    auto compare(basic_random_access_iter other) const
    {
        return it_ - other.it_;
    }

    int * it_;
};

template<typename ValueType>
struct random_access_iter : boost::iterator_facade::iterator_facade<
                                random_access_iter<ValueType>,
                                std::random_access_iterator_tag,
                                ValueType>
{
    random_access_iter(ValueType * it) : it_(it) {}
    template<
        typename ValueType2,
        typename E = std::enable_if_t<std::is_convertible<
            typename ValueType2::value_type *,
            ValueType *>::value>>
    random_access_iter(ValueType2 it) : it_(it.it_)
    {}

private:
    friend boost::iterator_facade::access;
    ValueType & dereference() const { return *it_; }
    void advance(std::ptrdiff_t i) { it_ += i; }
    auto compare(random_access_iter other) const
    {
        return it_ - other.it_;
    }

    ValueType * it_;

    template<typename ValueType2>
    friend struct random_access_iter;
};

using random_access = random_access_iter<int>;
using const_random_access = random_access_iter<int const>;

#if 0 // TODO: Call ranges algorithms with this.
struct basic_proxy_random_access_iter : boost::iterator_facade::iterator_facade<
                                            basic_proxy_random_access_iter,
                                            std::random_access_iterator_tag,
                                            int,
                                            int>
{
    basic_proxy_random_access_iter() : it_(nullptr) {}
    basic_proxy_random_access_iter(int * it) : it_(it) {}

private:
    friend boost::iterator_facade::access;
    int dereference() const { return *it_; }
    void advance(std::ptrdiff_t i) { it_ += i; }
    auto compare(basic_proxy_random_access_iter other) const
    {
        return it_ - other.it_;
    }

    int * it_;
};
#endif

std::array<int, 10> ints = {{0, 1, 2, 3, 4, 5, 6, 7, 8, 9}};


TEST(random_access, basic_coverage)
{
    basic_random_access_iter first(ints.data());
    basic_random_access_iter last(ints.data() + ints.size());

    EXPECT_EQ(*first, 0);
    EXPECT_EQ(*(first + 1), 1);
    EXPECT_EQ(*(first + 2), 2);
    EXPECT_EQ(*(1 + first), 1);
    EXPECT_EQ(*(2 + first), 2);

    EXPECT_EQ(first[0], 0);
    EXPECT_EQ(first[1], 1);
    EXPECT_EQ(first[2], 2);

    EXPECT_EQ(*(last - 1), 9);
    EXPECT_EQ(*(last - 2), 8);
    EXPECT_EQ(*(last - 3), 7);

    EXPECT_EQ(last[-1], 9);
    EXPECT_EQ(last[-2], 8);
    EXPECT_EQ(last[-3], 7);

    EXPECT_EQ(last - first, 10);
    EXPECT_EQ(first, first);
    EXPECT_NE(first, last);
    EXPECT_LT(first, last);
    EXPECT_LE(first, last);
    EXPECT_LE(first, first);
    EXPECT_GT(last, first);
    EXPECT_GE(last, first);
    EXPECT_GE(last, last);

    {
        auto first_copy = first;
        first_copy += 10;
        EXPECT_EQ(first_copy, last);
    }

    {
        auto last_copy = last;
        last_copy -= 10;
        EXPECT_EQ(last_copy, first);
    }
}

TEST(random_access, basic_std_copy)
{
    {
        std::array<int, 10> ints_copy;
        basic_random_access_iter first(ints.data());
        basic_random_access_iter last(ints.data() + ints.size());
        std::copy(first, last, ints_copy.begin());
        EXPECT_EQ(ints_copy, ints);
    }

    {
        std::array<int, 10> ints_copy;
        basic_random_access_iter first(ints.data());
        basic_random_access_iter last(ints.data() + ints.size());
        std::copy(
            std::make_reverse_iterator(last),
            std::make_reverse_iterator(first),
            ints_copy.begin());
        std::reverse(ints_copy.begin(), ints_copy.end());
        EXPECT_EQ(ints_copy, ints);
    }

    {
        std::array<int, 10> iota_ints;
        basic_random_access_iter first(iota_ints.data());
        basic_random_access_iter last(iota_ints.data() + iota_ints.size());
        std::iota(first, last, 0);
        EXPECT_EQ(iota_ints, ints);
    }

    {
        std::array<int, 10> iota_ints;
        basic_random_access_iter first(iota_ints.data());
        basic_random_access_iter last(iota_ints.data() + iota_ints.size());
        std::iota(
            std::make_reverse_iterator(last),
            std::make_reverse_iterator(first),
            0);
        std::reverse(iota_ints.begin(), iota_ints.end());
        EXPECT_EQ(iota_ints, ints);
    }

    {
        std::array<int, 10> iota_ints;
        basic_random_access_iter first(iota_ints.data());
        basic_random_access_iter last(iota_ints.data() + iota_ints.size());
        std::iota(
            std::make_reverse_iterator(last),
            std::make_reverse_iterator(first),
            0);
        std::sort(first, last);
        EXPECT_EQ(iota_ints, ints);
    }
}

TEST(random_access, mutable_to_const_conversions)
{
    random_access first(ints.data());
    random_access last(ints.data() + ints.size());
    const_random_access first_copy(first);
    const_random_access last_copy(last);
    std::equal(first, last, first_copy, last_copy);
}

TEST(random_access, coverage)
{
    random_access first(ints.data());
    random_access last(ints.data() + ints.size());

    EXPECT_EQ(*first, 0);
    EXPECT_EQ(*(first + 1), 1);
    EXPECT_EQ(*(first + 2), 2);
    EXPECT_EQ(*(1 + first), 1);
    EXPECT_EQ(*(2 + first), 2);

    EXPECT_EQ(first[0], 0);
    EXPECT_EQ(first[1], 1);
    EXPECT_EQ(first[2], 2);

    EXPECT_EQ(*(last - 1), 9);
    EXPECT_EQ(*(last - 2), 8);
    EXPECT_EQ(*(last - 3), 7);

    EXPECT_EQ(last[-1], 9);
    EXPECT_EQ(last[-2], 8);
    EXPECT_EQ(last[-3], 7);

    EXPECT_EQ(last - first, 10);
    EXPECT_EQ(first, first);
    EXPECT_NE(first, last);
    EXPECT_LT(first, last);
    EXPECT_LE(first, last);
    EXPECT_LE(first, first);
    EXPECT_GT(last, first);
    EXPECT_GE(last, first);
    EXPECT_GE(last, last);

    {
        auto first_copy = first;
        first_copy += 10;
        EXPECT_EQ(first_copy, last);
    }

    {
        auto last_copy = last;
        last_copy -= 10;
        EXPECT_EQ(last_copy, first);
    }
}

TEST(random_access, std_copy)
{
    random_access first(ints.data());
    random_access last(ints.data() + ints.size());

    {
        std::array<int, 10> ints_copy;
        std::copy(first, last, ints_copy.begin());
        EXPECT_EQ(ints_copy, ints);
    }

    {
        std::array<int, 10> ints_copy;
        std::copy(
            std::make_reverse_iterator(last),
            std::make_reverse_iterator(first),
            ints_copy.begin());
        std::reverse(ints_copy.begin(), ints_copy.end());
        EXPECT_EQ(ints_copy, ints);
    }

    {
        std::array<int, 10> iota_ints;
        random_access first(iota_ints.data());
        random_access last(iota_ints.data() + iota_ints.size());
        std::iota(first, last, 0);
        EXPECT_EQ(iota_ints, ints);
    }

    {
        std::array<int, 10> iota_ints;
        random_access first(iota_ints.data());
        random_access last(iota_ints.data() + iota_ints.size());
        std::iota(
            std::make_reverse_iterator(last),
            std::make_reverse_iterator(first),
            0);
        std::reverse(iota_ints.begin(), iota_ints.end());
        EXPECT_EQ(iota_ints, ints);
    }

    {
        std::array<int, 10> iota_ints;
        random_access first(iota_ints.data());
        random_access last(iota_ints.data() + iota_ints.size());
        std::iota(
            std::make_reverse_iterator(last),
            std::make_reverse_iterator(first),
            0);
        std::sort(first, last);
        EXPECT_EQ(iota_ints, ints);
    }
}

TEST(random_access, const_std_copy)
{
    const_random_access first(ints.data());
    const_random_access last(ints.data() + ints.size());

    {
        std::array<int, 10> ints_copy;
        std::copy(first, last, ints_copy.begin());
        EXPECT_EQ(ints_copy, ints);
    }

    {
        EXPECT_TRUE(std::binary_search(first, last, 3));
        EXPECT_TRUE(std::binary_search(
            std::make_reverse_iterator(last),
            std::make_reverse_iterator(first),
            3,
            std::greater<>{}));
    }
}
