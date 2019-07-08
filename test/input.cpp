#include <boost/iterator_facade/iterator_facade.hpp>

#include <gtest/gtest.h>

#include <array>


using namespace boost;

struct basic_input_iter
    : iterator_facade::
          iterator_facade<basic_input_iter, std::input_iterator_tag>
{
    basic_input_iter() : it_(nullptr) {}
    basic_input_iter(int * it) : it_(it) {}

private:
    friend iterator_facade::access;
    int dereference() { return *it_; }
    void increment() { return ++it_; }

    int * it_;
};

TEST(input, TODO)
{
    std::array<int, 10> ints;
    std::iota(ints.begin(), ints.end(), 0);
    basic_input_iter first(ints.data());
    basic_input_iter last(ints.data() + ints.size());

    // TODO
}
