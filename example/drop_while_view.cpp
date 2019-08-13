// Copyright (C) 2019 T. Zachary Laine
//
// Distributed under the Boost Software License, Version 1.0. (See
// accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
#include <boost/stl_interfaces/view_interface.hpp>

#include <algorithm>
#include <vector>

#include <cassert>


//[ all_view
template<typename Iterator, typename Sentinel>
struct subrange
{
    subrange() = default;

    constexpr subrange(Iterator it, Sentinel s) : first_(it), last_(s) {}

    constexpr Iterator begin() const { return first_; }
    constexpr Iterator end() const { return last_; }

private:
    Iterator first_;
    Sentinel last_;
};

template<typename Range>
auto all(Range && range)
{
    return subrange<decltype(range.begin()), decltype(range.end())>(
        range.begin(), range.end());
}

template<typename Range>
using all_view = decltype(all(std::declval<Range>()));
//]

//[ drop_while_view_template
template<typename Range, typename Pred>
struct drop_while_view
    : boost::stl_interfaces::view_interface<drop_while_view<Range, Pred>>
{
    using base_type = all_view<Range>;

    drop_while_view() = default;

    constexpr drop_while_view(Range & base, Pred pred) :
        base_(all(base)),
        pred_(std::move(pred))
    {}

    constexpr base_type base() const { return base_; }
    constexpr const Pred & pred() const noexcept { return pred_; }

    constexpr auto begin()
    {
        auto first = base_.begin();
        auto const last = base_.end();
        for (; first != last; ++first) {
            if (!pred_(*first))
                break;
        }
        return first;
    }

    constexpr auto end() { return base_.end(); }

private:
    base_type base_;
    Pred pred_;
};

template<typename Range, typename Pred>
auto make_drop_while_view(Range & base, Pred pred)
{
    return drop_while_view<Range, Pred>(base, std::move(pred));
}
//]


int main()
{
    {
    //[ all_usage
    std::vector<int> const ints = {2, 4, 3, 4, 5, 6};
    auto all_ints = all(ints);
    assert(
        std::equal(all_ints.begin(), all_ints.end(), ints.begin(), ints.end()));
    //]
    }

    {
    //[ drop_while_simple_usage
    std::vector<int> const ints = {2, 4, 3, 4, 5, 6};
    auto even = [](int x) { return x % 2 == 0; };
    auto ints_after_even_prefix = make_drop_while_view(ints, even);
    assert(std::equal(
        ints_after_even_prefix.begin(),
        ints_after_even_prefix.end(),
        ints.begin() + 2,
        ints.end()));
    //]
    }

    {
    //[ drop_while_view_interface_usage
    std::vector<int> const ints = {2, 4, 3, 4, 5, 6};
    auto even = [](int x) { return x % 2 == 0; };
    auto ints_after_even_prefix = make_drop_while_view(ints, even);
    assert(ints_after_even_prefix[2] == 5);
    //]
    }
}
