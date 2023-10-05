
#pragma once 

#include <iterator>
#include <cstddef>
#include "any_iterator/any_iterator.hpp"
#include "neo/iterator_facade.hpp"
#include "neo/bytewise_iterator.hpp"
#include <functional>

namespace ns3
{

    template< typename T, typename C = std::random_access_iterator_tag >
    using anyiterator = IteratorTypeErasure::any_iterator<T, C >;

/*
    struct byte_conv    
    {   
        template< typename T, typename = std::enable_if_t< sizeof(T) == 1  , int >     >
        byte_conv( T val ){ m_byte = static_cast<std::byte>( val) ; }

        template< typename T, typename = std::enable_if_t< sizeof(T) == 1  , int >     >
        byte_conv( T& val ){ m_byte = static_cast<std::byte>( val) ; }

        operator std::byte&(){return m_byte;}

         template< typename T, typename = std::enable_if_t< sizeof(T) == 1  , int >     >
         operator T&(){return static_cast<T&>(m_byte); }

         operator char&() & { return ( *reinterpret_cast<char*>(&m_byte)   ); }
    private:
    std::byte m_byte;
    };


    // static_assert( std::is_convertible_v<char&,byte_conv&> );
    // static_assert( std::is_convertible_v<char&,byte_conv> );
    static_assert( std::is_convertible_v<char,byte_conv> );
    static_assert( std::is_convertible_v<byte_conv, char> );
    static_assert( std::is_convertible_v<byte_conv&, char&> );
*/

 // static_assert( IteratorTypeErasure::detail::is_iterator_type_erasure_compatible< anyiterator<byte_conv>,anyiterator<uint8_t> >::value );  // fails

    // static_assert( IteratorTypeErasure::detail::is_iterator_type_erasure_compatible< anyiterator<std::byte>,anyiterator<uint8_t> >::value ); // fails

    //    static_assert( IteratorTypeErasure::detail::reference_types_erasure_compatible_1< anyiterator<std::byte>, anyiterator<uint8_t> >::type::value ); // fails

/*
        template < typename T, typename = std::enable_if_t< std::same_as_v<sizeof(T) , 1 > , int > 
                typename = std::enable_if_t< std::disjunction_v< std::same_as_v<C,std::bidirectional_iterator_tag >,
                                                                std::same_as_v<C,std::random_access_iterator_tag > 
                                                               > >
        >
        class byte_iter_wrapper // must not be an any_iterator itself !! 
        {
            byte_iter_wrapper( anyiterator<T,C> iter )
            {
                m_iter =iter;
            }



            private:
            anyiterator<T,C> m_iter;

        };

*/
    template<class Derived>
    class byte_iter_mixin
    {
        auto& m_iter() { return static_cast<Derived&>(*this).m_iter; }
        public:


    void WriteU8( uint8_t data )
    {
        *m_iter() = std::byte( data );  
        ++m_iter();
    }

    void WriteU8( uint8_t data, uint32_t len )
    {
        for( size_t i = 0; i<len; ++i)
        {
            *m_iter() = std::byte( data );
            ++m_iter();
        }
    }

    void WriteU16( uint16_t data )
    {
    WriteU8(data & 0xff);
    data >>= 8;
    WriteU8(data & 0xff);
    }

    void WriteU32( uint32_t data )
    {
 WriteU8(data & 0xff);
    data >>= 8;
    WriteU8(data & 0xff);
    data >>= 8;
    WriteU8(data & 0xff);
    data >>= 8;
    WriteU8(data & 0xff);
    }

    void WriteU64( uint64_t data )
    {
  WriteU8(data & 0xff);
    data >>= 8;
    WriteU8(data & 0xff);
    data >>= 8;
    WriteU8(data & 0xff);
    data >>= 8;
    WriteU8(data & 0xff);
    data >>= 8;
    WriteU8(data & 0xff);
    data >>= 8;
    WriteU8(data & 0xff);
    data >>= 8;
    WriteU8(data & 0xff);
    data >>= 8;
    WriteU8(data & 0xff);
    }


void WriteHtolsbU16(uint16_t data)
{
    WriteU8((data >> 0) & 0xff);
    WriteU8((data >> 8) & 0xff);
}

void WriteHtolsbU32(uint32_t data)
{
    WriteU8((data >> 0) & 0xff);
    WriteU8((data >> 8) & 0xff);
    WriteU8((data >> 16) & 0xff);
    WriteU8((data >> 24) & 0xff);
}

void WriteHtolsbU64(uint64_t data)
{
    WriteU8((data >> 0) & 0xff);
    WriteU8((data >> 8) & 0xff);
    WriteU8((data >> 16) & 0xff);
    WriteU8((data >> 24) & 0xff);
    WriteU8((data >> 32) & 0xff);
    WriteU8((data >> 40) & 0xff);
    WriteU8((data >> 48) & 0xff);
    WriteU8((data >> 56) & 0xff);
}

void WriteHtonU16(uint16_t data)
 {
  *m_iter() = std::byte( (data >> 8) & static_cast<uint8_t>(0xff) );
  ++m_iter();
    *m_iter() = std::byte( (data >> 0) & static_cast<uint8_t>(0xff) );
    ++m_iter();
 }

void WriteHtonU32(uint32_t data)
{
    /*m_iter()[0] = (data >> 24) & 0xff;
    m_iter()[1] = (data >> 16) & 0xff;
    m_iter()[2] = (data >> 8) & 0xff;
    m_iter()[3] = (data >> 0) & 0xff;*/
    *m_iter() = std::byte( (data >> 24) & 0xff );
    ++m_iter();
    *m_iter() = std::byte( (data >> 16) & 0xff );
    ++m_iter();
    *m_iter() = std::byte( (data >> 8) & 0xff );
    ++m_iter();
    *m_iter() = std::byte( (data >> 0) & 0xff );
    ++m_iter();
}

void WriteHtonU64(uint64_t data)
{
 WriteU8((data >> 56) & 0xff);
    WriteU8((data >> 48) & 0xff);
    WriteU8((data >> 40) & 0xff);
    WriteU8((data >> 32) & 0xff);
    WriteU8((data >> 24) & 0xff);
    WriteU8((data >> 16) & 0xff);
    WriteU8((data >> 8) & 0xff);
    WriteU8((data >> 0) & 0xff);
}

void Write(const uint8_t* buffer, uint32_t size)
{
    auto tmp{m_iter()};
// std::ranges::copy( m_iter(), buffer,size );
//std::ranges::transform( m_iter()| std::ranges::views::take(size), [](const auto b ){ return std::to_integer<uint8_t>(b); } , buffer );
std::transform( buffer, buffer+size, m_iter(), [](const auto b ){ return std::byte(b); }  );
// static_cast<Derived&>(*this).operator+=(size);
m_iter()+=size;
NS_ASSERT_MSG( std::distance( tmp, m_iter() ) == size, "logic error in iter impl " << std::to_string(std::distance( m_iter(),tmp )) );
}

// void Write(const_buffer_iterator start, const_buffer_iterator end);


    };

    template<class Derived>
    class const_byte_iter_mixin
    {
        auto & m_iter(){ return static_cast<Derived&>(*this).m_iter; }

        public:


// why no PeekU16|32|64() ?!

uint8_t PeekU8()
{
return std::to_integer<uint8_t>(*m_iter());
}

uint8_t ReadU8()
{
 auto tmp{PeekU8()};
 ++m_iter();
 return tmp;
}

uint16_t ReadU16()
{
  uint8_t byte0 = ReadU8();
    uint8_t byte1 = ReadU8();
    uint16_t data = byte1;
    data <<= 8;
    data |= byte0;

    return data;
}

uint32_t ReadU32()
{
 uint8_t byte0 = ReadU8();
    uint8_t byte1 = ReadU8();
    uint8_t byte2 = ReadU8();
    uint8_t byte3 = ReadU8();
    uint32_t data = byte3;
    data <<= 8;
    data |= byte2;
    data <<= 8;
    data |= byte1;
    data <<= 8;
    data |= byte0;
    return data;
}

uint64_t ReadU64()
{
  uint8_t byte0 = ReadU8();
    uint8_t byte1 = ReadU8();
    uint8_t byte2 = ReadU8();
    uint8_t byte3 = ReadU8();
    uint8_t byte4 = ReadU8();
    uint8_t byte5 = ReadU8();
    uint8_t byte6 = ReadU8();
    uint8_t byte7 = ReadU8();
    uint64_t data = byte7;
    data <<= 8;
    data |= byte6;
    data <<= 8;
    data |= byte5;
    data <<= 8;
    data |= byte4;
    data <<= 8;
    data |= byte3;
    data <<= 8;
    data |= byte2;
    data <<= 8;
    data |= byte1;
    data <<= 8;
    data |= byte0;

    return data;
}

uint16_t ReadNtohU16()
{
    uint16_t retval = 0;
    retval |= std::to_integer<uint16_t>( *m_iter() );
    retval <<= 8;

    ++m_iter();

    retval |= std::to_integer<uint16_t>(*m_iter() );

    ++m_iter();
    
    return retval;
}

uint32_t ReadNtohU32()
{
    uint32_t retval = 0;
    retval |= std::to_integer<uint32_t>(*m_iter());
    retval <<= 8;
    ++m_iter();
    retval |= std::to_integer<uint32_t>(*m_iter());
    retval <<= 8;
    ++m_iter();
    retval |= std::to_integer<uint32_t>(*m_iter());
    retval <<= 8;
    ++m_iter();
    retval |= std::to_integer<uint32_t>(*m_iter());

    ++m_iter();
    
    return retval;
}

uint64_t ReadNtohU64()
{
 uint64_t retval = 0;
    retval |= ReadU8();
    retval <<= 8;
    retval |= ReadU8();
    retval <<= 8;
    retval |= ReadU8();
    retval <<= 8;
    retval |= ReadU8();
    retval <<= 8;
    retval |= ReadU8();
    retval <<= 8;
    retval |= ReadU8();
    retval <<= 8;
    retval |= ReadU8();
    retval <<= 8;
    retval |= ReadU8();
    return retval;
}



uint16_t ReadLsbtohU16()
{
 uint8_t byte0 = ReadU8();
    uint8_t byte1 = ReadU8();
    uint16_t data = byte1;
    data <<= 8;
    data |= byte0;
    return data;
}

uint32_t ReadLsbtohU32()
{
  uint8_t byte0 = ReadU8();
    uint8_t byte1 = ReadU8();
    uint8_t byte2 = ReadU8();
    uint8_t byte3 = ReadU8();
    uint32_t data = byte3;
    data <<= 8;
    data |= byte2;
    data <<= 8;
    data |= byte1;
    data <<= 8;
    data |= byte0;
    return data;
}

uint64_t ReadLsbtohU64()
{
 uint8_t byte0 = ReadU8();
    uint8_t byte1 = ReadU8();
    uint8_t byte2 = ReadU8();
    uint8_t byte3 = ReadU8();
    uint8_t byte4 = ReadU8();
    uint8_t byte5 = ReadU8();
    uint8_t byte6 = ReadU8();
    uint8_t byte7 = ReadU8();
    uint64_t data = byte7;
    data <<= 8;
    data |= byte6;
    data <<= 8;
    data |= byte5;
    data <<= 8;
    data |= byte4;
    data <<= 8;
    data |= byte3;
    data <<= 8;
    data |= byte2;
    data <<= 8;
    data |= byte1;
    data <<= 8;
    data |= byte0;

    return data;
}

void Read(uint8_t* buffer, uint32_t size)
{
 for (uint32_t i = 0; i < size; i++)
    {
        buffer[i] = ReadU8();
    }
}

// void Read( buffer_terator start, uint32_t size);
    };

    class buffer_iterator : public neo::iterator_facade<buffer_iterator>,
                            public const_byte_iter_mixin<buffer_iterator>,
                            public byte_iter_mixin<buffer_iterator>
    {
        friend class const_byte_iter_mixin<buffer_iterator>;
        friend class byte_iter_mixin<buffer_iterator>;

        anyiterator<std::byte> m_iter;

      public:
        /*
            template < typename T, typename C, typename = std::enable_if_t<sizeof(T) == 1  , int > ,
                    typename = std::enable_if_t< std::disjunction_v<
           std::is_same<C,std::bidirectional_iterator_tag >,
                                                                    std::is_same<C,std::random_access_iterator_tag
           >
                                                                   >,int >
                                                                    >
            buffer_iterator( anyiterator<T,C> iter)
            {
                m_iter = iter;
            }
            */
            buffer_iterator(const buffer_iterator& other)
            : neo::iterator_facade<buffer_iterator>(other),
              m_iter(other.m_iter),
              begin_impl(other.begin_impl),
              end_impl(other.end_impl)
        {
        }

        buffer_iterator& operator=(const buffer_iterator&) = default;


        buffer_iterator() = default;

        buffer_iterator& operator=(const Buffer::Iterator& iter)
        {
            m_iter = iter;
            begin_impl = [](const buffer_iterator* _this) { return buffer_iterator(*_this); };

            end_impl = [iter](const buffer_iterator* _this) {
                auto tmp{*_this};
                // tmp.advance( iter.GetRemainingSize() );
                auto iter2{iter};
                while (!iter2.IsEnd())
                {
                    iter2.Next();
                }
                tmp += tmp.distance_to(iter2);
                return tmp;
            };
            return *this;
        }

        buffer_iterator(Buffer::Iterator iter)
            : m_iter(iter)
        {
            begin_impl = [](const buffer_iterator* _this) { return buffer_iterator(*_this); };

            end_impl = [iter](const buffer_iterator* _this) {
                auto tmp{*_this};
                // tmp.advance( iter.GetRemainingSize() );
                auto iter2{iter};
                while (!iter2.IsEnd())
                {
                    iter2.Next();
                }
                tmp += tmp.distance_to(iter2);
                return tmp;
            };
        }

        template <neo::single_buffer T>
        buffer_iterator(neo::bytewise_iterator<T> iter)
            : m_iter(iter)
        {
            begin_impl = []( const buffer_iterator* _this) { return buffer_iterator(*_this); };

            end_impl = [iter ](const buffer_iterator* _this ) {
                //auto tmp{ *_this };
                //tmp.advance( std::distance(iter, iter.end()) );
                //return tmp;
                return iter.end();
            };
        }

        template <class T>
        buffer_iterator& operator=(const neo::bytewise_iterator<T> iter)
        {
            m_iter = iter;
            begin_impl = [](const buffer_iterator* _this) { return buffer_iterator(*_this); };

            end_impl = [iter](const buffer_iterator* _this) {
                auto tmp{*_this};
                tmp.advance(std::distance(iter, iter.end()));
                return tmp;
            };
            return *this;
        }

        std::function<buffer_iterator(const buffer_iterator*)> begin_impl;
        std::function<buffer_iterator(const buffer_iterator*)> end_impl;

        [[nodiscard]] auto begin() const noexcept
        {
            return begin_impl(this);
        }

        [[nodiscard]] auto end() const noexcept
        {
            return end_impl(this);
        }

        [[nodiscard]] std::ptrdiff_t size() const;
        [[nodiscard]] size_t GetRemainingSize() const;

        void advance(std::ptrdiff_t diff) noexcept
        {
            m_iter += diff;
        }

        auto& dereference() const noexcept
        {
            return *m_iter;
        }

        std::ptrdiff_t distance_to(buffer_iterator other) const noexcept
        {
            return other.m_iter - m_iter;
        }

        bool operator==(const buffer_iterator& other) const noexcept
        {
            return other.m_iter == m_iter;
        }
    };

    inline std::ptrdiff_t
    buffer_iterator::size() const
    {
        return std::distance(begin_impl(this), end_impl(this));
    }

    inline size_t
    buffer_iterator::GetRemainingSize() const
    {
        return std::distance(*this, end_impl(this));
    }

    class const_buffer_iterator : public neo::iterator_facade<const_buffer_iterator>,
                                  public const_byte_iter_mixin<const_buffer_iterator>
    {
        friend class const_byte_iter_mixin<const_buffer_iterator>;

        anyiterator<const std::byte> m_iter;

      public:
        /*
            template < typename T, typename C, typename = std::enable_if_t<sizeof(T) == 1  , int > ,
                    typename = std::enable_if_t< std::disjunction_v<
           std::is_same<C,std::bidirectional_iterator_tag >,
                                                                    std::is_same<C,std::random_access_iterator_tag
           >
                                                                   >,int >
                                                                    >
            buffer_iterator( anyiterator<T,C> iter)
            {
                m_iter = iter;
            }
            */

        const_buffer_iterator(const const_buffer_iterator& other)
            : neo::iterator_facade<const_buffer_iterator>(other),
              m_iter(other.m_iter),
              cbegin_impl(other.cbegin_impl),
              cend_impl(other.cend_impl)
        {
        }

        const_buffer_iterator& operator=(const const_buffer_iterator&) = default;

        const_buffer_iterator() = default;

        const_buffer_iterator& operator=(const Buffer::ConstIterator& iter)
        {
            m_iter = iter;
            cbegin_impl = [](const const_buffer_iterator* _this) {
                return const_buffer_iterator(*_this);
            };

            cend_impl = [iter](const const_buffer_iterator* _this) {
                auto tmp{*_this};
                // tmp.advance( iter.GetRemainingSize() );
                auto iter2{iter};
                while (!iter2.IsEnd())
                {
                    iter2.Next();
                }
                tmp += tmp.distance_to(iter2);
                return tmp;
            };
            return *this;
        }

        const_buffer_iterator(Buffer::ConstIterator iter)
            : m_iter(iter)
        {
            cbegin_impl = [](const const_buffer_iterator* _this) {
                return const_buffer_iterator(*_this);
            };

            cend_impl = [iter](const const_buffer_iterator* _this) {
                auto tmp{*_this};
                //  tmp.advance( iter.GetRemainingSize() );
                auto iter2{iter};
                while (!iter2.IsEnd())
                {
                    iter2.Next();
                }
                tmp += tmp.distance_to(iter2);

                return tmp;
            };
        }

        template <neo::single_buffer T>
        const_buffer_iterator(neo::bytewise_iterator<T> iter)
            : m_iter(iter)
        {
            cbegin_impl = [](const const_buffer_iterator* _this) {
                return const_buffer_iterator(*_this);
            };

            cend_impl = [iter](const const_buffer_iterator* _this) {
                /*
                HALT: CONTRACT VIOLATION
      ===== ==================
      What: A caller violated the preconditions of an API.
  Expected: false
  Location: File "/usr/local/include/neo/buffer/neo/bytewise_iterator.hpp", line 154,
            in [constexpr void neo::bytewise_iterator<T>::advance(std::ptrdiff_t) [with T = neo::mutable_buffer; std::ptrdiff_t = long int]]
   Message: Advancing bytewise iteator beyond end of the buffer
   Context: diff := 68:int64
                */
           //     auto tmp{*_this};
           //     tmp.advance(std::distance(iter, iter.end()));
           //     return tmp;
           return iter.end();
            };
        }

        template <class T>
        const_buffer_iterator& operator=(const neo::bytewise_iterator<T>& iter)
        {
            m_iter = iter;
            cbegin_impl = [](const const_buffer_iterator* _this) {
                return const_buffer_iterator(*_this);
            };

            cend_impl = [iter](const const_buffer_iterator* _this) {
                auto tmp{*_this};
                tmp.advance(std::distance(iter, iter.end()));
                return tmp;
            };
            return *this;
        }

        std::function<const_buffer_iterator(const const_buffer_iterator*)> cbegin_impl;
        std::function<const_buffer_iterator(const const_buffer_iterator*)> cend_impl;

        [[nodiscard]] auto cbegin() const noexcept
        {
            return cbegin_impl(this);
        }

        [[nodiscard]] auto cend() const noexcept
        {
            return cend_impl(this);
        }

        [[nodiscard]] std::ptrdiff_t size() const;

        void advance(std::ptrdiff_t diff) noexcept
        {
            m_iter += diff;
        }

        const std::byte& dereference() const noexcept
        {
            return *m_iter;
        }

        // [[nodiscard]] auto GetRemainingSize()const{return std::distance( *this, cend_impl() );  }
        [[nodiscard]] inline uint32_t GetRemainingSize() const;

        std::ptrdiff_t distance_to(const_buffer_iterator other) const noexcept
        {
            return other.m_iter - m_iter;
        }

        bool operator==(const const_buffer_iterator& other) const noexcept
        {
            return other.m_iter == m_iter;
        }
    };

    inline uint32_t
    const_buffer_iterator::GetRemainingSize() const
    {
        return std::distance(*this, cend_impl(this));
    }

    inline std::ptrdiff_t
    const_buffer_iterator::size() const
    {
        return std::distance(cbegin_impl(this), cend_impl(this));
    }
    } // namespace ns3