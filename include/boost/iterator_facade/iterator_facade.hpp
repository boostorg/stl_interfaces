#ifndef BOOST_ITERATOR_FACADE_ITERATOR_FACADE_HPP
#define BOOST_ITERATOR_FACADE_ITERATOR_FACADE_HPP


namespace boost { namespace iterator_facade {

    struct access
    {
        template<typename Derived>
        constexpr static auto
        dereference(Derived d) noexcept(noexcept(d.dereference()))
            -> decltype(d.dereference())
        {
            return d.dereference();
        }
    };

    template<typename Derived, typename IteratorCategory>
    struct iterator_facade
    {
    private:
        constexpr Derived & derived() noexcept
        {
            return static_cast<Derived &>(*this);
        }
        constexpr Derived const & derived() const noexcept
        {
            return static_cast<Derived const &>(*this);
        }

    public:
        constexpr auto
        operator*() noexcept(noexcept(access::dereference(derived())))
            -> decltype(access::dereference(derived()))
        {
            return access::dereference(derived());
        }

        constexpr auto operator*() const
            noexcept(noexcept(access::dereference(derived())))
                -> decltype(access::dereference(derived()))
        {
            return access::dereference(derived());
        }

        // TODO
    };

}}

#endif
