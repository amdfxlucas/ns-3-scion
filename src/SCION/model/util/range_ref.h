#include <concepts>
#include <ranges>
#include <type_traits>
#include "ns3/function_ref.h"
#include "ns3/iterator-util.h"

namespace ns3
{
/*
https://artificial-mind.net/blog/2020/10/24/range_ref
*/

template <class RangeT, class ElementT, class = void>
struct is_compatible_range : std::false_type { };

template <class RangeT, class ElementT>
struct is_compatible_range<RangeT, ElementT, std::void_t<
        decltype(ElementT(*std::begin(std::declval<RangeT>()))),
        decltype(std::end(std::declval<RangeT>()))>
    >
     : std::true_type { };

// a non-owning, lightweight view of a range
// whose element types are convertible to T
template <class T>
struct range_ref
{
    // iterates over the viewed range and invokes callback for each element
    void for_each(function_ref<void(T)> callback) 
    { 
        _for_each(_range, callback); 
    }

    // empty range
    range_ref()
    {
        _for_each = [](void*, function_ref<void(T)>) {};
    }

    // any compatible range
    template <class Range, 
              std::enable_if_t<is_compatible_range<Range, T>::value, int> = 0>
    range_ref(Range&& range)
    {

  using WrappedIterator = std::remove_cvref_t< decltype( range.begin() )>; // or Range::iterator
            using namespace IteratorTypeErasure::detail;
     
        _range = const_cast<void*>(static_cast<void const*>(&range));
        _for_each = [](void* r, function_ref<void(T)> callback) {
            for (auto&& v : *reinterpret_cast<decltype(&range)>(r))
                callback(v);
        };
        
        if constexpr ( !std::is_const_v< std::remove_reference_t<std::iter_reference_t<WrappedIterator> > > )
        {
            static_assert( std::is_convertible_v< std::iter_reference_t<WrappedIterator>, std::iter_reference_t<iterator> > );
        _begin = [](void* r )
        {    
           
         return iterator( static_cast<decltype(&range)>(r) -> begin() );
           
        };
        _end = [](void* r )
        {
            return iterator( static_cast<decltype(&range)>(r) -> end() );
        };

        }
        // else
        {
            _cbegin = [](void * r)
            {
                return const_iterator( static_cast<decltype(&range)>(r)->begin() );
            };
            _cend = [](void * r)
            {
                return const_iterator( static_cast<decltype(&range)>(r)->end() );
            };
        }

        
    }

    // {initializer, list, syntax}
    template <class U, 
              std::enable_if_t<std::is_convertible_v<U const&, T>, int> = 0>
    range_ref(std::initializer_list<U> const& range)
    {
        _range = const_cast<void*>(static_cast<void const*>(&range));
        _for_each = [](void* r, function_ref<void(T)> f) {
            for (auto&& v : *static_cast<decltype(&range)>(r))
                f(v);
        };

        _begin = [](void* r )
        {
            return iterator( static_cast<decltype(&range)>(r) -> begin() );
        };
         _end = [](void* r )
        {
            return iterator( static_cast<decltype(&range)>(r) -> end() );
        };
    }

    auto begin() const{ return _begin(_range); }
    auto end() const { return _end(_range);}

    auto cbegin() const{ return _cbegin(_range); }
    auto cend() const { return _cend(_range);}

private:
    using iterator = anyiterator<T>;
    using const_iterator  = anyiterator<const T>;

    using range_fun_t = void (*)(void*, function_ref<void(T)>);
    using iter_fun_t = iterator (*)(void*);

    using const_iter_fun_t = const_iterator(*)(void*);

    // or via union depending on const_cast preference
    void* _range = nullptr;
    range_fun_t _for_each = nullptr;

    iter_fun_t _begin = nullptr;
    iter_fun_t _end = nullptr;
    const_iter_fun_t _cbegin = nullptr;
    const_iter_fun_t _cend =nullptr;
};

}