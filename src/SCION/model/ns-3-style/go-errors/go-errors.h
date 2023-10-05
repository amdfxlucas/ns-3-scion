#pragma once

#include <exception>
#include <iomanip>
#include <iostream>
#include <regex>
#include<optional>
#include <concepts>
#include <sstream>
#include <stdexcept>
#include <stdint.h>
#include <string>
#include <variant>
#include <iostream>
#include <ranges>
#include <algorithm>
#include <string_view>


#include <string>
#include <iostream>

#include <boost/container/flat_map.hpp>

namespace ns3
{


inline constexpr long hashCombine(int a, int b)
{
    long A = (long)(a >= 0 ? 2 * (long)a : -2 * (long)a - 1);
    long B = (long)(b >= 0 ? 2 * (long)b : -2 * (long)b - 1);
    long C = (long)((A >= B ? A * A + A + B : A + B * B) / 2);
    return ( (a < 0 ) && ( b < 0 ) ) || ( ( a >= 0 && b >= 0) ? C : (-C - 1) );
}


class conststr
{
    public:

        constexpr conststr(std::string_view s )
        : string( s.data()),
        size(s.size()) {}

        template<std::size_t N>
        constexpr conststr( const char(&STR)[N] )
        :string(STR), size(N-1)
        {}

        constexpr conststr(const char* STR, std::size_t N)
        :string(STR), size(N)
        {}

        constexpr char operator[](std::size_t n)
        {
            return n < size ? string[n] : 0;
        }

        constexpr std::size_t get_size()
        {
            return size;
        }
        constexpr operator std::string_view()const{ return {string,size}; }
        constexpr const char* get_string()
        {
            return string;
        }

        //This method is related with Fowler–Noll–Vo hash function
        constexpr unsigned hash( size_t n=0, unsigned h=2166136261) const
        {
            return n == size ? h : hash( n+1, (h * 16777619) ^ (string[n]));
        }

        constexpr bool operator ==(const conststr& other )const{return hash()==other.hash();}

    private:
        const char* string;
        std::size_t size;
};


inline int constexpr operator "" _h(const char* str, size_t sz)
{
    // auto c = conststr( str,sz);
    //static_assert( c.get_string() != 0 );
    return conststr(str,sz).hash();
}





 //--------------------------------------------------------------------------------


template < std::size_t N>
inline consteval auto hash( char const (&pp)[N])
{
    return conststr(pp,N-1).hash();
}


// std::ostream& operator<< ( std::ostream & os, MyClass& my){ return os << std::string(my);};


// Go's error interface
template< class E>
concept go_error = requires( E e)
{
 {e.what() } -> std::convertible_to<std::string_view>; //std::same_as<std::string_view>;
 { e.operator bool() } -> std::same_as<bool>;

 {  std::remove_reference_t<E>::static_type()}->std::integral;
 //{ e.type() }->std::same_as<uint>; 

};


template<class E, class E1>
concept has_As = requires( E e)
{
{ e.template As<E1>()} -> std::same_as< std::optional<E1> >;
};


struct dummy
{   template<class T>
    std::optional<T> As()const{return std::nullopt; }
};

template<class E>
concept has_As2 = requires( E e)
{

 { e. template As() };

// { e.template As<E1>();} -> std::same_as< std::optional<E1> >;
};


static_assert( has_As<dummy,dummy>);

class error;



#define _ERROR_TYPE_( error_type_name ) inline static constexpr uint _hash_ = hash( #error_type_name  );

class proto_error
{
   _ERROR_TYPE_( proto_error )
  
public:

constexpr static uint static_type(){return _hash_;}
constexpr uint type()const{return static_type(); }
constexpr proto_error(){}
constexpr proto_error( std::string_view err ) : _what(err){}
 bool is(const error& e )const;
std::string_view what()const{return _what; };
operator bool()const{return !_what.empty(); }
private:

const std::string_view _what;
private:



};





class error
{


class error_concept
{
public:


template<class Err>
std::optional<Err*> As() 
{ if (Err::static_type()== type() )
{return reinterpret_cast<Err*>(get_runtime_object() ); 
}else
{return std::nullopt;} 
};


// equivalent of err!=nil in Go
virtual operator bool() const = 0; 

// virtual bool operator==(const error& other) const =0 ;

virtual bool is( const error& other ) const =0;
virtual uint type()const =0;
virtual std::string_view what()const = 0;
// virtual error cause()const;

virtual void* get_runtime_object() = 0;

};

template<class E>
class error_model : public error_concept
{

public:

uint type()const{if(m_err){return m_err.type();}else{return "error"_h; } }
// error cause()const{return m_err.cause(); }
std::string_view what() const{ return m_err.what();}

bool is( const error& e )const{ if(m_err){return m_err.type()==e.type();}else{ return true;} }
operator bool() const{ return m_err;}
    
    error_model( E&& e)
     : m_err( std::forward<E>(e) ){}

private:
void* get_runtime_object(){return &m_err;}
std::remove_cvref_t<E> m_err;

};

public:

bool is(const error& e)const { if(m_err){return m_err->is(e);} else{return e.type()==type();} }

template<class Err>
std::optional<Err*> As(){ if(m_err){return m_err->As<Err>(); } else { return std::nullopt;} }

operator bool() const { if(m_err){ return m_err->operator bool();}else{return false; }  }
std::string_view what()const{ if(m_err){ return m_err->what();}else{ return "null";} }
uint type() const { if(m_err){return m_err->type();}else{return "error"_h; } }

constexpr error(){}

template < go_error Err>
error( Err && e ) : m_err( new error_model< Err > ( std::forward<Err>(e) ) ) {}

private:
std::shared_ptr<error_concept> m_err;

};

// return default-initialized null-error
// error error_concept::cause()const {return {}; }


inline bool proto_error::is(const error& e )const { return e.type() == type(); }

template<typename... Ts>
using AllStringorView = typename std::enable_if<std::conjunction< std::disjunction< std::is_convertible<Ts, std::string>,
                                                                                    std::is_same<Ts,std::string_view>   >...>::value>::type;
template <typename ...Ts>
constexpr bool AllStringorView_v = std::conjunction_v< std::disjunction< std::is_convertible<Ts, std::string>,
                                                                        std::is_same<Ts,std::string_view>   >...> ;

template<typename... Ts>
using AllStrings = typename std::enable_if<std::conjunction<std::is_convertible<Ts, std::string>...>::value>::type;

template<typename... Ts>
constexpr bool AllStrings_v = std::conjunction_v<std::is_convertible<Ts, std::string>...>;

static_assert( AllStringorView_v<const char[11],const char[9], const char[9], std::string ,std::string_view > ) ;
// static_assert( AllStrings_v<const char[11],const char[9], const char[9], std::string ,std::string_view> ) ;
// as it seems std::string_view is not convertible to std::string ?!

template<typename ...Ts>
inline int consteval length(){return sizeof...(Ts); }

template<typename ...Ts>
inline bool consteval divisible_by_two()
{//return  ( (length<Ts...>())%2 )==0; 
return (sizeof...(Ts))%2==0;
}


template <typename Fun>
inline void iteratePack(const Fun&) {}

template <typename Fun, typename Arg, typename ... Args>
inline void iteratePack(const Fun &fun, Arg &&arg, Args&& ... args)
{
    fun(std::forward<Arg>(arg));
    iteratePack(fun, std::forward<Args>(args)...);
}

template <typename Fun>
inline void iteratePack2(const Fun&) {}

template <typename Fun, typename Arg, typename Arg2, typename ... Args>
inline void iteratePack2(const Fun &fun, Arg &&arg,Arg2&& arg2, Args&& ... args)
{
    fun(std::forward<Arg>(arg), std::forward<Arg2>(arg2) );
    iteratePack2(fun, std::forward<Args>(args)... );
}


// static_assert( divisible_by_two<int,int,int>() );


//error error::error_model::cause() const{return m_err.cause();}


class error_exception : public std::runtime_error
{
    public:
    //error_exception(){}
    error_exception( error e) 
    : std::runtime_error( "go-error" ),
     m_err(std::move(e)){}

    const char* what()const noexcept{ return m_err.what().data(); }
    private:
    error m_err;
};

}