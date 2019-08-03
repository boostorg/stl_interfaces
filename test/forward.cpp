#include <boost/iterator_facade/iterator_facade.hpp>

#include <gtest/gtest.h>

#include <array>
#include <numeric>
#include <type_traits>


struct basic_forward_iter
    : boost::iterator_facade::
          iterator_facade<basic_forward_iter, std::forward_iterator_tag, int>
{
    basic_forward_iter() : it_(nullptr) {}
    basic_forward_iter(int * it) : it_(it) {}

private:
    friend boost::iterator_facade::access;
    int & dereference() const { return *it_; }
    void next() { ++it_; }
    bool equals(basic_forward_iter other) const { return it_ == other.it_; }

    int * it_;
};

template<typename ValueType>
struct forward_iter : boost::iterator_facade::iterator_facade<
                          forward_iter<ValueType>,
                          std::forward_iterator_tag,
                          ValueType>
{
    forward_iter(ValueType * it) : it_(it) {}
    template<
        typename ValueType2,
        typename E = std::enable_if_t<std::is_convertible<
            typename ValueType2::value_type *,
            ValueType *>::value>>
    forward_iter(ValueType2 it) : it_(it.it_)
    {}

private:
    friend boost::iterator_facade::access;
    ValueType & dereference() const { return *it_; }
    void next() { ++it_; }
    bool equals(forward_iter other) const { return it_ == other.it_; }

    ValueType * it_;

    template<typename ValueType2>
    friend struct forward_iter;
};

using forward = forward_iter<int>;
using const_forward = forward_iter<int const>;

#if 0 // TODO: Call ranges algorithms with this.
struct basic_proxy_forward_iter : boost::iterator_facade::iterator_facade<
                                      basic_proxy_forward_iter,
                                      std::forward_iterator_tag,
                                      int,
                                      int>
{
    basic_proxy_forward_iter() : it_(nullptr) {}
    basic_proxy_forward_iter(int * it) : it_(it) {}

private:
    friend boost::iterator_facade::access;
    int dereference() const { return *it_; }
    void next() { ++it_; }
    bool equals(basic_proxy_forward_iter other) const
    {
        return it_ == other.it_;
    }

    int * it_;
};
#endif

std::array<int, 10> ints = {{0, 1, 2, 3, 4, 5, 6, 7, 8, 9}};


TEST(forward, basic_std_copy)
{
    basic_forward_iter first(ints.data());
    basic_forward_iter last(ints.data() + ints.size());

    {
        std::array<int, 10> ints_copy;
        std::copy(first, last, ints_copy.begin());
        EXPECT_EQ(ints_copy, ints);
    }

    {
        std::array<int, 10> iota_ints;
        basic_forward_iter first(iota_ints.data());
        basic_forward_iter last(iota_ints.data() + iota_ints.size());
        std::iota(first, last, 0);
        EXPECT_EQ(iota_ints, ints);
    }
}


TEST(forward, mutable_to_const_conversions)
{
    forward first(ints.data());
    forward last(ints.data() + ints.size());
    const_forward first_copy(first);
    const_forward last_copy(last);
    std::equal(first, last, first_copy, last_copy);
}

TEST(forward, std_copy)
{
    forward first(ints.data());
    forward last(ints.data() + ints.size());

    {
        std::array<int, 10> ints_copy;
        std::copy(first, last, ints_copy.begin());
        EXPECT_EQ(ints_copy, ints);
    }

    {
        std::array<int, 10> iota_ints;
        forward first(iota_ints.data());
        forward last(iota_ints.data() + iota_ints.size());
        std::iota(first, last, 0);
        EXPECT_EQ(iota_ints, ints);
    }
}

TEST(forward, const_std_copy)
{
    const_forward first(ints.data());
    const_forward last(ints.data() + ints.size());

    {
        std::array<int, 10> ints_copy;
        std::copy(first, last, ints_copy.begin());
        EXPECT_EQ(ints_copy, ints);
    }

    {
        EXPECT_TRUE(std::binary_search(first, last, 3));
    }
}
