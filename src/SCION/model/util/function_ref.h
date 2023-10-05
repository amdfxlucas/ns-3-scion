
#include <ranges>

#include <stdint.h>

#include <type_traits>


#include <string>

namespace ns3
{


template <class A_t, class B_t, std::size_t... I>
consteval bool same_impl( std::index_sequence<I...>)
{
    return (  std::is_convertible_v< decltype(std::get<I>(std::declval<A_t>() ) ),
                                    decltype(std::get<I>(std::declval<B_t>() ) )> && ...);
    
}

// static_assert( same_impl< std::tuple<int,float,short>, std::tuple< long, double, int> > (std::make_index_sequence<3>() ) );

template< typename ...A>
struct  all_convertible
{
  using A_t = const std::tuple< A...>;

  template <typename... B>
  static consteval bool same(  )
  {
    using B_t = const std::tuple<B...>;

    static_assert(sizeof...(A) == sizeof...(B));
    return same_impl<A_t,B_t>( std::index_sequence_for<B...>{} );

  }



};


/*
static_assert( std::is_convertible_v<double,long double> );
static_assert( std::is_convertible_v<float, double> );
static_assert( all_convertible<int,short,long,float>::same<int,short,long , double>() );
*/




template <typename Returned, typename Required>
struct compatible_return_type
    : std::integral_constant<bool, std::is_void<Required>::value
                                       || std::is_convertible<Returned, Required>::value>
{
};

template <typename Func, typename Return, typename... Args>
using enable_matching_function =
    std::enable_if_t< compatible_return_type<decltype(std::declval<Func&>()(std::declval<Args>()...)),
                                                   Return>::value,
                    int>;


struct matching_function_pointer_tag {};
struct matching_functor_tag{};
struct invalid_functor_tag{};

template <typename Func, typename Return, typename... Args>
struct get_callable_tag
{
    // use unary + to convert to function pointer
    template <typename T>
    static matching_function_pointer_tag test(
        int, T& obj, enable_matching_function<decltype(+obj), Return, Args...> = 0);

    template <typename T>
    static matching_functor_tag test(short, T& obj,
                                     enable_matching_function<T, Return, Args...> = 0);

    static invalid_functor_tag test(...);

    using type = decltype(test(0, std::declval<Func&>()));
};                    


template <typename Signature>
class function_ref;

template <typename Return, typename... Args>
class function_ref<Return(Args...)>
{
    
    using storage  =  std::aligned_union_t<8,void*, Return (*)(Args...)>;
    using callback = Return (*)(const void*, Args...);

    storage  storage_;
    callback cb_;

    void* get_memory() noexcept
    {
        return &storage_;
    }

    const void* get_memory() const noexcept
    {
        return &storage_;
    }

public:
    using signature = Return(Args...);

    function_ref(Return (*fptr)(Args...))
    {
        using pointer_type        = Return (*)(Args...);

        // DEBUG_ASSERT(fptr, detail::precondition_error_handler{},
        //             "function pointer must not be null");
        ::new (get_memory()) pointer_type(fptr);

        cb_ = [](const void* memory, Args... args) {
            auto func  = *static_cast<const pointer_type*>(memory);
            return func(static_cast<Args>(args)...);
        };
    }

    // participates in overload resolution iff signature is compatible
template <typename Return2, typename ... Args2
//,  typename  std::enable_if_t<  ( std::declval< all_convertible<  Args2... > >().same<   Args... >() ) , int>  
> requires requires( Args...a, Args2...b)
{ all_convertible< Args...>::template same<Args2...>(); 
    std::is_convertible_v<Return2,Return>;
}
function_ref( Return2(*fptr2)(Args2...) )
{
/*
So we can reinterpret_cast the function pointer to Return(*)(Args...),
construct that in the storage and set the callback,
 so it reads a function pointer of Return(*)(Args...) from the storage, 
 reinterpret_cast that to Return2(*)(Args2...) and calls that.

*/ 
    using pointer_type        = Return (*)(Args...);
     using pointer_type2        = Return2 (*)(Args2...);
    ::new (get_memory()) pointer_type( reinterpret_cast<pointer_type>(fptr2) );

        cb_ = [](const void* memory, Args... args) {
            auto func  = static_cast<const pointer_type*>(memory);
            auto func2 = *reinterpret_cast<const pointer_type2*>( func);
            return static_cast<Return>( func2(static_cast<Args>(args)...) );
        };
}
    

    template <typename Functor
    //,        typename  std::enable_if_t< std::is_same_v<decltype( get_callable_tag<Functor,Return, Args...>::test),matching_functor_tag> ,int > = 0 
    >
        //      typename = HERE BE SFINAE> // disable if Functor not a functor requires matching_functor_tag
        requires requires (Functor, Return, Args...)
        {
            std::is_same_v<decltype( get_callable_tag<Functor,Return, Args...>::test),matching_functor_tag>;
        }
    explicit function_ref(Functor& f)
    : cb_([](const void* memory, Args... args) 
    {
          using ptr_t = void*;
          auto  ptr   = *static_cast<const ptr_t*>(memory);
          auto& func  = *static_cast<Functor*>(ptr);
          // deliberately assumes operator(), see further below
          return static_cast<Return>(func(static_cast<Args>(args)...));
      })
    {
        ::new (get_memory()) void*(&f);
    }

    Return operator()(Args... args) const
    {
        return cb_(get_memory(), static_cast<Args>(args)...);
    }
};


}