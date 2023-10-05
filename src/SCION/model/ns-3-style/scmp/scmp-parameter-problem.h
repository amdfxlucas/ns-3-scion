#pragma once
#include "ns3/gopacket++.h"

namespace ns3
{
    std::expected<input_t,error> decodeSCMPParameterProblem(input_t, LayerStore* );

/*!
 \brief SCMPParameterProblem represents the structure of a parameter problem message.

	+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
	|            reserved           |           Pointer             |
	+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
*/

    class SCMPParameterProblem : public virtual Layer, public Header
    {
    public:
    SCMPParameterProblem(){}
    SCMPParameterProblem( uint16_t ptr )
    : m_pointer(ptr){}

  static LayerType staticLayerType(){ return m_type;}

    LayerType Type()const;

    // Contents returns the bytes making up this layer.
    input_t Contents() const override;
    input_t Payload() const override;
    LayerClass CanDecode() const override;

    std::optional<LayerType> NextType() const override;

    uint32_t Deserialize(Buffer::Iterator start) override;

    std::expected<uint32_t,error> Deserialize(input_t start, LayerStore* = nullptr );

    void Serialize( Buffer::Iterator start ) const override;
    output_t Serialize( output_t start , SerializeOptions ) const ;
    uint32_t GetSerializedSize() const ;
     static TypeId GetTypeId();
    TypeId GetInstanceTypeId() const override;

    void Print(std::ostream& os) const override;


  
private:

    inline static LayerType m_type = LayerTypeRegistry::instance().RegisterLayerType(
                                LayerTypeMetaData{
                                .m_name = "SCMPParameterProblem",
                                .m_decoder = &decodeSCMPParameterProblem    });


    input_t m_start;
    input_t m_payload;
    
        uint16_t m_pointer;
    };



}