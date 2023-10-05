#pragma once

#include "ns3/gopacket++.h"
#include "ns3/extn_base.h"
#include "ns3/scion-types.h"
#include "ns3/tlv_option.h"

namespace ns3
{

std::expected<input_t,error> decodeEndToEndExtn( input_t , LayerStore* );

    class EndToEndExtn : public ExtnBase, public virtual Layer, public Header
    {
        public:


  static LayerType staticLayerType(){ return m_type;}

    LayerType Type() const override ;

    // Contents returns the bytes making up this layer.
    input_t Contents() const override;
    input_t Payload() const override ;
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

    const std::vector<TLVOption>& GetOptions()const override{return m_opts;}
  
private:
    std::vector<TLVOption> m_opts;

    inline static LayerType m_type = LayerTypeRegistry::instance().RegisterLayerType(
                                LayerTypeMetaData{
                                .m_name = "EndToEndExtn",
                                .m_decoder = &decodeEndToEndExtn });

    };


LayerType scionNextLayerTypeAfterE2E( L4ProtocolType_t t);



}