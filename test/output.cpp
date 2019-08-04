#include <boost/iterator_facade/iterator_facade.hpp>

#include <gtest/gtest.h>

#include <array>
#include <numeric>
#include <type_traits>


struct basic_output_iter
    : boost::iterator_facade::
          iterator_facade<basic_output_iter, std::input_iterator_tag, int>
{
    basic_output_iter(int * it) : it_(it) {}

private:
    friend boost::iterator_facade::access;
    int & dereference() const { return *it_; }
    void next() { ++it_; }

    int * it_;
};

using output = basic_output_iter;

template<typename Container>
struct back_insert_iter : boost::iterator_facade::iterator_facade<
                              back_insert_iter<Container>,
                              std::output_iterator_tag,
                              typename Container::value_type,
                              back_insert_iter<Container> &>
{
    back_insert_iter(Container & c) : c_(std::addressof(c)) { assert(c_); }

    back_insert_iter & operator=(typename Container::value_type const & v)
    {
        c_->push_back(v);
        return *this;
    }
    back_insert_iter & operator=(typename Container::value_type && v)
    {
        c_->push_back(std::move(v));
        return *this;
    }

private:
    friend boost::iterator_facade::access;
    back_insert_iter & dereference() { return *this; }
    void next() {}

    Container * c_;
};

using back_insert = back_insert_iter<std::vector<int>>;


std::vector<int> ints = {{0, 1, 2, 3, 4, 5, 6, 7, 8, 9}};


TEST(output, basic_std_copy)
{
    std::vector<int> ints_copy(ints.size());
    std::copy(ints.begin(), ints.end(), output(ints_copy.data()));
    EXPECT_EQ(ints_copy, ints);
}

TEST(output, back_insert_std_copy)
{
    std::vector<int> ints_copy;
    std::copy(ints.begin(), ints.end(), back_insert(ints_copy));
    EXPECT_EQ(ints_copy, ints);
}
