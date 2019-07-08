#include <boost/iterator_facade/iterator_facade.hpp>

#include <gtest/gtest.h>

#include <array>
#include <numeric>


struct basic_input_iter
    : boost::iterator_facade::
          iterator_facade<basic_input_iter, std::input_iterator_tag, int>
{
    basic_input_iter() : it_(nullptr) {}
    basic_input_iter(int * it) : it_(it) {}

    // TODO: const version, and conversion from const.

private:
    friend boost::iterator_facade::access;
    int dereference() const { return *it_; }
    void next() { ++it_; }
    bool equals(basic_input_iter other) const { return it_ == other.it_; }

    int * it_;
};

TEST(input, TODO)
{
    std::array<int, 10> ints;
    std::iota(ints.begin(), ints.end(), 0);
    basic_input_iter first(ints.data());
    basic_input_iter last(ints.data() + ints.size());

    {
        std::array<int, 10> ints_copy;
        std::copy(first, last, ints_copy.begin());
        EXPECT_EQ(ints_copy, ints);
    }

    // TODO
}
