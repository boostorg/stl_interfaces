#include <boost/iterator_facade/iterator_facade.hpp>

#include <gtest/gtest.h>

#include <array>
#include <numeric>
#include <type_traits>


struct basic_bidirectional_iter : boost::iterator_facade::iterator_facade<
                                      basic_bidirectional_iter,
                                      std::bidirectional_iterator_tag,
                                      int>
{
    basic_bidirectional_iter() : it_(nullptr) {}
    basic_bidirectional_iter(int * it) : it_(it) {}

private:
    friend boost::iterator_facade::access;
    int & dereference() const { return *it_; }
    void prev() { --it_; }
    void next() { ++it_; }
    bool equals(basic_bidirectional_iter other) const
    {
        return it_ == other.it_;
    }

    int * it_;
};

template<typename ValueType>
struct bidirectional_iter : boost::iterator_facade::iterator_facade<
                                bidirectional_iter<ValueType>,
                                std::bidirectional_iterator_tag,
                                ValueType>
{
    bidirectional_iter(ValueType * it) : it_(it) {}
    template<
        typename ValueType2,
        typename E = std::enable_if_t<std::is_convertible<
            typename ValueType2::value_type *,
            ValueType *>::value>>
    bidirectional_iter(ValueType2 it) : it_(it.it_)
    {}

private:
    friend boost::iterator_facade::access;
    ValueType & dereference() const { return *it_; }
    void prev() { --it_; }
    void next() { ++it_; }
    bool equals(bidirectional_iter other) const { return it_ == other.it_; }

    ValueType * it_;

    template<typename ValueType2>
    friend struct bidirectional_iter;
};

using bidirectional = bidirectional_iter<int>;
using const_bidirectional = bidirectional_iter<int const>;

#if 0 // TODO: Call ranges algorithms with this.
struct basic_proxy_bidirectional_iter : boost::iterator_facade::iterator_facade<
                                            basic_proxy_bidirectional_iter,
                                            std::bidirectional_iterator_tag,
                                            int,
                                            int>
{
    basic_proxy_bidirectional_iter() : it_(nullptr) {}
    basic_proxy_bidirectional_iter(int * it) : it_(it) {}

private:
    friend boost::iterator_facade::access;
    int dereference() const { return *it_; }
    void prev() { --it_; }
    void next() { ++it_; }
    bool equals(basic_proxy_bidirectional_iter other) const
    {
        return it_ == other.it_;
    }

    int * it_;
};
#endif

std::array<int, 10> ints = {{0, 1, 2, 3, 4, 5, 6, 7, 8, 9}};


TEST(bidirectional, basic_std_copy)
{
    basic_bidirectional_iter first(ints.data());
    basic_bidirectional_iter last(ints.data() + ints.size());

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
        basic_bidirectional_iter first(iota_ints.data());
        basic_bidirectional_iter last(iota_ints.data() + iota_ints.size());
        std::iota(first, last, 0);
        EXPECT_EQ(iota_ints, ints);
    }

    {
        std::array<int, 10> iota_ints;
        basic_bidirectional_iter first(iota_ints.data());
        basic_bidirectional_iter last(iota_ints.data() + iota_ints.size());
        std::iota(
            std::make_reverse_iterator(last),
            std::make_reverse_iterator(first),
            0);
        std::reverse(iota_ints.begin(), iota_ints.end());
        EXPECT_EQ(iota_ints, ints);
    }
}


TEST(bidirectional, mutable_to_const_conversions)
{
    bidirectional first(ints.data());
    bidirectional last(ints.data() + ints.size());
    const_bidirectional first_copy(first);
    const_bidirectional last_copy(last);
    std::equal(first, last, first_copy, last_copy);
}

TEST(bidirectional, std_copy)
{
    bidirectional first(ints.data());
    bidirectional last(ints.data() + ints.size());

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
        bidirectional first(iota_ints.data());
        bidirectional last(iota_ints.data() + iota_ints.size());
        std::iota(first, last, 0);
        EXPECT_EQ(iota_ints, ints);
    }

    {
        std::array<int, 10> iota_ints;
        bidirectional first(iota_ints.data());
        bidirectional last(iota_ints.data() + iota_ints.size());
        std::iota(
            std::make_reverse_iterator(last),
            std::make_reverse_iterator(first),
            0);
        std::reverse(iota_ints.begin(), iota_ints.end());
        EXPECT_EQ(iota_ints, ints);
    }
}

TEST(bidirectional, const_std_copy)
{
    const_bidirectional first(ints.data());
    const_bidirectional last(ints.data() + ints.size());

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
