#pragma once
#include "ns3/gopacket++.h"

namespace ns3
{

std::expected<input_t,error> decodeSCMPDestinationUnreachable( input_t data, LayerStore* pb);

/*!
  \brief SCMPDestinationUnreachable represents the structure of a destination
  unreachable message.

	 0                   1                   2                   3
	 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
	+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
	|                             Unused                            |
	+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
*/

class SCMPDestinationUnreachable : public virtual Layer, public Header
{
public:


  static LayerType staticLayerType(){ return m_type;}

    LayerType Type() const override  {     return m_type;   }

    // Contents returns the bytes making up this layer.
    input_t Contents() const override;
    input_t Payload() const override;

    LayerClass CanDecode() const override;

    std::optional<LayerType> NextType() const override;

    std::expected<uint32_t,error> Deserialize(input_t start, LayerStore* ) override;

    uint32_t Deserialize(Buffer::Iterator );

    void Serialize( Buffer::Iterator  start ) const override;
    output_t Serialize( output_t start, SerializeOptions ) const ;
    uint32_t GetSerializedSize() const ;
     static TypeId GetTypeId();
    TypeId GetInstanceTypeId() const override;

    void Print(std::ostream& os) const override;


  
private:

    inline static LayerType m_type = LayerTypeRegistry::instance().RegisterLayerType(LayerTypeMetaData{
                                .m_name = "SCMPDestinationUnreachable",
                                .m_decoder = &decodeSCMPDestinationUnreachable                  });


    input_t m_start;
    input_t m_payload;
  


};



}