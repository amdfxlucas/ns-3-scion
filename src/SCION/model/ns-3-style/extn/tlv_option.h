#pragma once
#include "ns3/gopacket++.h"
#include <optional>
#include <ranges>
#include "ns3/range_ref.h"

namespace ns3
{


// input_t decodeTLVOption(input_t, LayerStore* );


class TLVOption 
{
public:
 // OptionType indicates the type of a TLV Option that is part of an extension header.
    enum class OptionType : uint8_t 
    {
        OptTypePad1,
        OptTypePadN,
        OptTypeAuthenticator
    };
    TLVOption(){}
    TLVOption( OptionType type, uint8_t len, neo::const_buffer data );
    TLVOption(OptionType type, neo::const_buffer data );
    TLVOption(OptionType type, neo::const_buffer data,std::initializer_list<uint8_t> align );
    TLVOption(OptionType type, std::initializer_list<uint8_t> align );
      

   
/*
   LayerType Type() const override ;
    // Contents returns the bytes making up this layer.
    input_t Contents();
    input_t Payload();
    LayerClass CanDecode() const override;
    std::optional<LayerType> NextType() const override;
    */

    uint32_t Deserialize(Buffer::Iterator start) ;
    uint32_t Deserialize(input_t start);

    void Serialize( output_t start , bool fixLength ) const;
    output_t Serialize( output_t start , SerializeOptions ) const ;
    uint32_t GetSerializedSize() const ;
/*     static TypeId GetTypeId();
    TypeId GetInstanceTypeId() const override;

    void Print(std::ostream& os) const override;
*/

void SetOptAlign( uint8_t x, uint8_t y) { OptAlign[0]=x; OptAlign[1]=y; }
const auto GetOptAlign() const {return OptAlign; }



    int length(bool fixLengths) const
    {
        if ( opt_type == OptionType::OptTypePad1) {
            return 1;
        }
        if (fixLengths) {
            return opt_data.size() + 2;
        }
        return static_cast<int>(opt_data_len) + 2;
    }

  
private:

/*    inline static LayerType m_type = LayerTypeRegistry::instance().RegisterLayerType(
                                LayerTypeMetaData{
                                .m_name = "TLVOption",
                                .m_decoder = &decodeTLVOption });
*/

input_t m_content;
input_t m_payload;

    OptionType opt_type;
    mutable uint8_t opt_data_len=0;
    int ActualLength=0;
   
   //neo::mutable_buffer opt_data;
   neo::bytes opt_data;


    uint8_t OptAlign[2];// Xn+Y = [2]uint8{X, Y}

  friend void serializeTLVOptionPadding(output_t start, uint8_t padLength ) ;
friend int serializeTLVOptions( std::optional<output_t> start, range_ref<TLVOption> options ,bool fixLengths ) ;
};


// serializeTLVOptionPadding adds an appropriate PadN extension.
void serializeTLVOptionPadding(output_t start, uint8_t padLength ) ;

int serializeTLVOptions( std::optional<output_t> start, range_ref<TLVOption>      options ,                bool fixLengths ) ;

 

}