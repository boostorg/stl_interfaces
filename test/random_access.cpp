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
    auto compare(random_access_iter other) const { return it_ - other.it_; }

    ValueType * it_;

    template<typename ValueType2>
    friend struct random_access_iter;
};

using random_access = random_access_iter<int>;
using const_random_access = random_access_iter<int const>;

// TODO: Call ranges algorithms with this.
struct zip_iter : boost::iterator_facade::proxy_iterator_facade<
                      zip_iter,
                      std::random_access_iterator_tag,
                      std::pair<int, int>,
                      std::pair<int &, int &>>
{
    zip_iter() : it1_(nullptr), it2_(nullptr) {}
    zip_iter(int * it1, int * it2) : it1_(it1), it2_(it2) {}

private:
    friend boost::iterator_facade::access;
    std::pair<int &, int &> dereference() const
    {
        return std::pair<int &, int &>{*it1_, *it2_};
    }
    void advance(std::ptrdiff_t i)
    {
        it1_ += i;
        it2_ += i;
    }
    auto compare(zip_iter other) const
    {
        return it1_ - other.it1_;
    }

    int * it1_;
    int * it2_;
};

struct int_t
{
    int value_;

    bool operator==(int_t other) const { return value_ == other.value_; }
    bool operator!=(int_t other) const { return value_ != other.value_; }
    bool operator<(int_t other) const { return value_ < other.value_; }

    bool operator==(int other) const { return value_ == other; }
    bool operator!=(int other) const { return value_ != other; }
    bool operator<(int other) const { return value_ < other; }

    friend bool operator==(int lhs, int_t rhs) { return lhs == rhs.value_; }
    friend bool operator!=(int lhs, int_t rhs) { return lhs != rhs.value_; }
    friend bool operator<(int lhs, int_t rhs) { return lhs < rhs.value_; }
};

struct udt_zip_iter : boost::iterator_facade::proxy_iterator_facade<
                          udt_zip_iter,
                          std::random_access_iterator_tag,
                          std::pair<int_t, int>,
                          std::pair<int_t &, int &>>
{
    udt_zip_iter() : it1_(nullptr), it2_(nullptr) {}
    udt_zip_iter(int_t * it1, int * it2) : it1_(it1), it2_(it2) {}

private:
    friend boost::iterator_facade::access;
    std::pair<int_t &, int &> dereference() const
    {
        return std::pair<int_t &, int &>{*it1_, *it2_};
    }
    void advance(std::ptrdiff_t i)
    {
        it1_ += i;
        it2_ += i;
    }
    auto compare(udt_zip_iter other) const
    {
        return it1_ - other.it1_;
    }

    int_t * it1_;
    int * it2_;
};

// TODO: Document the necessity of writing this overload, and in which cases
// it needs to go in std.
namespace std {
    // Required for std::sort to work with zip_iter.  If zip_iter::reference
    // were not a std::pair with builtin types as its template parameters, we
    // could have put this in another namespace.
    void swap(zip_iter::reference && lhs, zip_iter::reference && rhs)
    {
        using std::swap;
        swap(lhs.first, rhs.first);
        swap(lhs.second, rhs.second);
    }
}

void swap(udt_zip_iter::reference && lhs, udt_zip_iter::reference && rhs)
{
    using std::swap;
    swap(lhs.first, rhs.first);
    swap(lhs.second, rhs.second);
}

std::array<int, 10> ints = {{0, 1, 2, 3, 4, 5, 6, 7, 8, 9}};
std::array<int, 10> ones = {{1, 1, 1, 1, 1, 1, 1, 1, 1, 1}};
std::array<std::pair<int, int>, 10> pairs = {{
    {0, 1},
    {1, 1},
    {2, 1},
    {3, 1},
    {4, 1},
    {5, 1},
    {6, 1},
    {7, 1},
    {8, 1},
    {9, 1},
}};

std::array<int_t, 10> udts = {
    {{0}, {1}, {2}, {3}, {4}, {5}, {6}, {7}, {8}, {9}}};
std::array<std::pair<int_t, int>, 10> udt_pairs = {{
    {{0}, 1},
    {{1}, 1},
    {{2}, 1},
    {{3}, 1},
    {{4}, 1},
    {{5}, 1},
    {{6}, 1},
    {{7}, 1},
    {{8}, 1},
    {{9}, 1},
}};

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

TEST(random_access, zip)
{
    {
        zip_iter first(ints.data(), ones.data());
        zip_iter last(ints.data() + ints.size(), ones.data() + ones.size());
        EXPECT_TRUE(std::equal(first, last, pairs.begin(), pairs.end()));
    }

    {
        auto ints_copy = ints;
        std::reverse(ints_copy.begin(), ints_copy.end());
        auto ones_copy = ones;
        zip_iter first(ints_copy.data(), ones_copy.data());
        zip_iter last(
            ints_copy.data() + ints_copy.size(),
            ones_copy.data() + ones_copy.size());
        EXPECT_FALSE(std::equal(first, last, pairs.begin(), pairs.end()));
        std::sort(first, last);
        EXPECT_TRUE(std::equal(first, last, pairs.begin(), pairs.end()));
    }

    {
        udt_zip_iter first(udts.data(), ones.data());
        udt_zip_iter last(udts.data() + udts.size(), ones.data() + ones.size());
        EXPECT_TRUE(
            std::equal(first, last, udt_pairs.begin(), udt_pairs.end()));
    }

    {
        auto udts_copy = udts;
        std::reverse(udts_copy.begin(), udts_copy.end());
        auto ones_copy = ones;
        udt_zip_iter first(udts_copy.data(), ones_copy.data());
        udt_zip_iter last(
            udts_copy.data() + udts_copy.size(),
            ones_copy.data() + ones_copy.size());
        EXPECT_FALSE(
            std::equal(first, last, udt_pairs.begin(), udt_pairs.end()));
        std::sort(first, last);
        EXPECT_TRUE(
            std::equal(first, last, udt_pairs.begin(), udt_pairs.end()));
    }
}
