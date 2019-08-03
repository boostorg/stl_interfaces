#include <boost/iterator_facade/iterator_facade.hpp>

#include <gtest/gtest.h>

#include <array>
#include <numeric>
#include <type_traits>


struct basic_proxy_input_iter
    : boost::iterator_facade::
          iterator_facade<basic_proxy_input_iter, std::input_iterator_tag, int>
{
    basic_proxy_input_iter(int * it) : it_(it) {}

private:
    friend boost::iterator_facade::access;
    int dereference() const { return *it_; }
    void next() { ++it_; }
    bool equals(basic_proxy_input_iter other) const { return it_ == other.it_; }

    int * it_;
};

template<typename ValueType>
struct proxy_input_iter : boost::iterator_facade::iterator_facade<
                              proxy_input_iter<ValueType>,
                              std::input_iterator_tag,
                              ValueType>
{
    proxy_input_iter(ValueType * it) : it_(it) {}
    template<
        typename ValueType2,
        typename E = std::enable_if_t<std::is_convertible<
            typename ValueType2::value_type *,
            ValueType *>::value>>
    proxy_input_iter(ValueType2 it) : it_(it.it_)
    {}

private:
    friend boost::iterator_facade::access;
    ValueType dereference() const { return *it_; }
    void next() { ++it_; }
    bool equals(proxy_input_iter other) const { return it_ == other.it_; }

    ValueType * it_;

    template<typename ValueType2>
    friend struct proxy_input_iter;
};

using proxy_input = proxy_input_iter<int>;
using const_proxy_input = proxy_input_iter<int const>;

std::array<int, 10> ints = {{0, 1, 2, 3, 4, 5, 6, 7, 8, 9}};


TEST(input, basic_proxy_std_copy)
{
    basic_proxy_input_iter first(ints.data());
    basic_proxy_input_iter last(ints.data() + ints.size());

    {
        std::array<int, 10> ints_copy;
        std::copy(first, last, ints_copy.begin());
        EXPECT_EQ(ints_copy, ints);
    }

    // TODO
}


TEST(input, proxy_mutable_to_const_conversions)
{
    proxy_input first(ints.data());
    proxy_input last(ints.data() + ints.size());
    const_proxy_input first_copy(first);
    const_proxy_input last_copy(last);
    std::equal(first, last, first_copy, last_copy);
}

TEST(input, proxy_std_copy)
{
    proxy_input first(ints.data());
    proxy_input last(ints.data() + ints.size());

    {
        std::array<int, 10> ints_copy;
        std::copy(first, last, ints_copy.begin());
        EXPECT_EQ(ints_copy, ints);
    }

    // TODO
}

TEST(input, const_proxy_std_copy)
{
    const_proxy_input first(ints.data());
    const_proxy_input last(ints.data() + ints.size());

    {
        std::array<int, 10> ints_copy;
        std::copy(first, last, ints_copy.begin());
        EXPECT_EQ(ints_copy, ints);
    }

    // TODO
}
