#pragma once
#include "ns3/buffer.h"
#include "ns3/fields-forward.h"
#include <string>
#include "scion-path-forward.h"

#include "ns3/iterator-util.h"
#include <stdint.h>
#include "ns3/go-errors.h"
#include <expected>

namespace ns3
{
/*
func RegisterPath()
{
    path.RegisterPath(path.Metadata{
        Type : 1,
        Desc : "SCION",
        New : func() path.Path{return &Raw{}},
    })
}
*/

// MetaHdr is the PathMetaHdr of a SCION (data-plane) path type.
struct MetaHdr
{
    uint8_t CurrINF;
    uint8_t CurrHF;
    uint8_t SegLen[3];

    void Serialize( buffer_iterator )const;
 //   void Serialize( Buffer::Iterator start ) const;

    std::expected<uint32_t,error> Deserialize( const_buffer_iterator );

    // uint32_t Deserialize(Buffer::Iterator); // change to ConstIterator

    //uint32_t Deserialize( const_buffer_iterator );

    static constexpr uint8_t GetSerializedSize(){return METALEN; }
    operator std::string()const;
    auto operator<=>(const MetaHdr& other)const = default;
};

// Base holds the basic information that is used by both raw and fully decoded paths.
struct Base
{
    // PathMeta is the SCION path meta header. It is always instantiated when
    // decoding a path from bytes.
    MetaHdr PathMeta;

    // theese two fields are derived from the metaHeader and not present in the binary representation
    // NumINF is the number of InfoFields in the path.
    uint8_t NumINF;
    // NumHops is the number HopFields in the path.
    uint8_t NumHops; // its type matches MetaHdr::CurrtHF

    // Len returns the length of the path in bytes.
    uint16_t Len() const
    {
        return METALEN + NumINF * INFO_FIELD_LEN + NumHops * HOPLEN;
    }

    // Type returns the type of the path.
    // pathType_t Type(){return 1;}

  //  uint32_t Deserialize(Buffer::Iterator); // change to ConstIterator
  //  void Serialize( Buffer::Iterator ) const;

    void Serialize( buffer_iterator )const;
   std::expected< uint32_t,error> Deserialize( const_buffer_iterator );


    static constexpr uint32_t GetSerializedSize(){ return METALEN /*+ 2*/; /*metaheader + numInf + NumHops */}

    // IncPath increases the currHF index and currINF index if appropriate.
    error IncPath();

    // IsXover returns whether we are at a crossover point. This includes
    // all segment switches, even over a peering link. Note that handling
    // of a regular segment switch and handling of a segment switch over a
    // peering link are fundamentally different. To distinguish the two,
    // you will need to extract the information from the info field.
    bool IsXover() const;

    // IsFirstHopAfterXover returns whether this is the first hop field after a crossover point.
    bool IsFirstHopAfterXover() const;
    auto operator<=>(const Base& other)const = default;
    operator std::string() const { return "[ " + static_cast<std::string>(PathMeta) +" numHops: " +
                                 std::to_string(NumHops) + " numInf: " +std::to_string(NumINF)  +" ]"; }

    friend std::ostream& operator<<( std::ostream& os, const Base& ) ;
private:
    uint8_t infIndexForHF(uint8_t hf)const;
};

inline std::ostream& operator<<( std::ostream& os, const Base& base )
{
    return os<< static_cast<std::string>( base ); 
}

} // namespace ns3



