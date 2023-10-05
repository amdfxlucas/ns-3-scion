

#include "ns3/gopacket++.h"
#include "ns3/log.h"

#include "ns3/simulator.h"
#include "ns3/test.h"

#include "ns3/tlv_option.h"
#include <compare>
#include <bitset>
#include <string_view>
#include "neo/as_dynamic_buffer.hpp"
#include "neo/byte_array.hpp"
#include "neo/bytewise_iterator.hpp"
#include "neo/as_buffer.hpp"



using namespace ns3;




/**
 * \ingroup internet-test
 *
 * \brief IPv4 Test
 */
class ExtnTestCase : public TestCase
{
  public:
    ExtnTestCase();
    ~ExtnTestCase() override;
    void DoRun() override;
    void TestSerializeTLVOptions();
    void TestSerializeTLVOptionsWithFinalPadding();
    
private:
// Adapted from RFC 8200 Appendix A

// Option X: 4-byte field, followed by 8-byte field. Alignment 8n + 2
 inline static TLVOption optX{
	 static_cast<TLVOption::OptionType>(0x1e),
	 neo::const_buffer( neo::byte_array{ std::byte(0xaa),
                                         std::byte(0xaa), 
                                         std::byte(0xaa),
                                         std::byte(0xaa),
                                         std::byte(0xbb),
                                         std::byte(0xbb),
                                         std::byte(0xbb),
                                         std::byte(0xbb),
                                         std::byte(0xbb),
                                         std::byte(0xbb),
                                         std::byte(0xbb),
                                         std::byte(0xbb)
                                        } ),
	{8, 2},
};

// Option Y: 1-byte field, followed by 2-byte field, followed by 4-byte field. Alignment 4n + 3
 inline static TLVOption optY{
	 static_cast<TLVOption::OptionType>(0x3e),
	neo::as_buffer( neo::byte_array{ std::byte(0x11),
                    std::byte(0x22),
                    std::byte(0x22),
                    std::byte(0x44),
                    std::byte(0x44),
                    std::byte(0x44),
                    std::byte(0x44)
                    } ),
	{4, 3},
};

 inline static auto rawTLVOptionsXY = neo::byte_array{
     std::byte(0x1e), std::byte(0x0c), std::byte(0xaa), std::byte(0xaa), std::byte(0xaa),
     std::byte(0xaa), std::byte(0xbb), std::byte(0xbb), std::byte(0xbb), std::byte(0xbb),
     std::byte(0xbb), std::byte(0xbb), std::byte(0xbb), std::byte(0xbb), std::byte(0x01),
     std::byte(0x01), std::byte(0x00), std::byte(0x3e), std::byte(0x07), std::byte(0x11),
     std::byte(0x22), std::byte(0x22), std::byte(0x44), std::byte(0x44), std::byte(0x44),
     std::byte(0x44)};

 inline static auto rawTLVOptionsYX = neo::byte_array{
     std::byte(0x00), std::byte(0x3e), std::byte(0x07), std::byte(0x11), std::byte(0x22),
     std::byte(0x22), std::byte(0x44), std::byte(0x44), std::byte(0x44), std::byte(0x44),
     std::byte(0x01), std::byte(0x04), std::byte(0x00), std::byte(0x00), std::byte(0x00),
     std::byte(0x00), std::byte(0x1e), std::byte(0x0c), std::byte(0xaa), std::byte(0xaa),
     std::byte(0xaa), std::byte(0xaa), std::byte(0xbb), std::byte(0xbb), std::byte(0xbb),
     std::byte(0xbb), std::byte(0xbb), std::byte(0xbb), std::byte(0xbb), std::byte(0xbb)};
};

ExtnTestCase::ExtnTestCase()
    : TestCase("Verify the SCION Extension Headers")
{
}


ExtnTestCase::~ExtnTestCase()
{
}



// A Hop-by-Hop or EndToEnd Options header containing both options X and Y would have one of the two
// following formats, depending on which option appeared first:
//
//	Option X | Option Y
//	+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//	|  Next Header  | Hdr Ext Len=6 | Option Type=X |Opt Data Len=12|
//	+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//	|                         4-octet field                         |
//	+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//	|                                                               |
//	+                         8-octet field                         +
//	|                                                               |
//	+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//	| PadN Option=1 |Opt Data Len=1 |       0       | Option Type=Y |
//	+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//	|Opt Data Len=7 | 1-octet field |         2-octet field         |
//	+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//	|                         4-octet field                         |
//	+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//
//	Option Y | Option X
//	+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//	|  Next Header  | Hdr Ext Len=7 | Pad1 Option=0 | Option Type=Y |
//	+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//	|Opt Data Len=7 | 1-octet field |         2-octet field         |
//	+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//	|                         4-octet field                         |
//	+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//	| PadN Option=1 |Opt Data Len=4 |       0       |       0       |
//	+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//	|       0       |       0       | Option Type=X |Opt Data Len=12|
//	+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//	|                         4-octet field                         |
//	+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//	|                                                               |
//	+                         8-octet field                         +
//	|                                                               |
//	+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

void
ExtnTestCase::DoRun()
{
	TestSerializeTLVOptions();
    TestSerializeTLVOptionsWithFinalPadding();
}

void ExtnTestCase::TestSerializeTLVOptionsWithFinalPadding() 
{
	
    struct _case_{ int opt_len; neo::const_buffer expected;   };

    std::array<_case_, 6> cases = {
        _case_{.opt_len = 0,
               // +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
               // |  Next Header  | Hdr Ext Len=0 | Option Type=V |Opt Data Len=0 |
               // +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
               .expected = neo::as_buffer(neo::byte_array{std::byte(0x76), std::byte(0x00) } )
        },
        _case_{.opt_len = 2,
               // +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
               // |  Next Header  | Hdr Ext Len=1 | Option Type=V |Opt Data Len=2 |
               // +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
               // |         2-octet data          | PadN Option=1 |Opt Data Len=0 |
               // +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
               .expected = neo::as_buffer(neo::byte_array{std::byte(0x76),
                                                          std::byte(0x02),
                                                          std::byte(0xff),
                                                          std::byte(0xff),
                                                          std::byte(0x01),
                                                          std::byte(0x00)})

        },
        _case_{ .opt_len = 3,
               // +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
               // |  Next Header  | Hdr Ext Len=1 | Option Type=V |Opt Data Len=3 |
               // +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
               // |                 3-octet data                  | Pad1 Option=0 |
               // +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
               .expected = neo::as_buffer(neo::byte_array{std::byte(0x76),
                                                          std::byte(0x03),
                                                          std::byte(0xff),
                                                          std::byte(0xff),
                                                          std::byte(0xff),
                                                          std::byte(0x00)} ) 
        },
        _case_{ .opt_len = 4,
               // +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
               // |  Next Header  | Hdr Ext Len=1 | Option Type=V |Opt Data Len=4 |
               // +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
               // |                          4-octet data                         |
               // +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
               .expected = neo::as_buffer(neo::byte_array{std::byte(0x76),
                                                          std::byte(0x04),
                                                          std::byte(0xff),
                                                          std::byte(0xff),
                                                          std::byte(0xff),
                                                          std::byte(0xff) } )
        },
        _case_{ .opt_len = 5,
            // +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
            // |  Next Header  | Hdr Ext Len=2 | Option Type=V |Opt Data Len=5 |
            // +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
            // |                          5-octet data                         |
            // +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
            // |      ...      | PadN Option=1 |Opt Data Len=1 |       0       |
            // +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
            .expected = neo::as_buffer(neo::byte_array{std::byte(0x76),
                                                       std::byte(0x05),
                                                       std::byte(0xff),
                                                       std::byte(0xff),
                                                       std::byte(0xff),
                                                       std::byte(0xff),
                                                       std::byte(0xff),
                                                       std::byte(0x01),
                                                       std::byte(0x01),
                                                       std::byte(0x00)}),

        },
        _case_{.opt_len = 1,
               // +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
               // |  Next Header  | Hdr Ext Len=1 | Option Type=V |Opt Data Len=1 |
               // +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
               // |  1-octet data | PadN Option=1 |Opt Data Len=1 |       0       |
               // +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
               .expected = neo::as_buffer(neo::byte_array{std::byte(0x76),
                                                          std::byte(0x01),
                                                          std::byte(0xff),
                                                          std::byte(0x01),
                                                          std::byte(0x01),
                                                          std::byte(0x00)})

        }
    };

    

	neo::byte_array ones{std::byte(0xff), std::byte(0xff), std::byte(0xff),std::byte(0xff), std::byte(0xff) };

  
	for (const auto& c : cases )
    {
		// variable length option test padding after different data lengths
	    TLVOption optV{
		static_cast<TLVOption::OptionType>( 0x76),
        neo::as_buffer(ones).first(c.opt_len),
		{1, 0}
		// data filled with repeated 0xff
        };
         
		
            auto options = std::ranges::single_view {optV};

		    auto l = serializeTLVOptions( std::nullopt, options, true);
			
            Buffer b;
            b.AddAtStart(l);
			serializeTLVOptions(b.begin(), options, true);

            std::stringstream s1,s2;

			std::ostream_iterator< std::bitset<8> > o1( s1, ", ");
            std::ostream_iterator< std::bitset<8> > o2( s2, ", " );
            auto print_byte = []( const auto b ){return  std::bitset<8>(std::to_integer<uint8_t>(b) ) ; };


    /*        std::ostream_iterator< uint8_t > o1( s1, ", ");
            std::ostream_iterator< uint8_t > o2( s2, ", " );
            s1<< std::hex;
            s2<< std::hex;
            auto print_byte = []( const auto b){return std::to_integer<uint8_t>(b); };
    */

            std::ranges::copy(b| std::views::transform( print_byte )  , o1 );
            std::ranges::copy( neo::bytewise_iterator(c.expected)| std::views::transform( print_byte ), o2);

            std::string_view ss1{s1.str()};
            std::string_view ss2{s2.str() };

            NS_TEST_ASSERT_MSG_EQ( ss1, ss2, "serialization failed for opt_len: " << c.opt_len );

            // NS_TEST_ASSERT_MSG_EQ( std::ranges::equal( b , neo::bytewise_iterator(c.expected) ),true , " serialization failed for opt_len: " << c.opt_len );
		
	}
    
}


void ExtnTestCase::TestSerializeTLVOptions()
{
    auto XY = std::views::all( std::vector({optX, optY}) );
    auto YX = std::views::all( std::vector({optY, optX}) );


    auto l = serializeTLVOptions( std::nullopt, XY, true);
	
    //Buffer b{ static_cast<uint32_t>(l),false};
    Buffer b;
    b.AddAtStart(l);
	
    serializeTLVOptions(b.Begin(), XY, true);
    NS_TEST_ASSERT_MSG_EQ( std::ranges::equal( b ,rawTLVOptionsXY ),true , "serialization of TLVOptions failed for {OptX|OptY} " );
	
	l = serializeTLVOptions( std::nullopt , YX, true);

	// Buffer b2{ static_cast<uint32_t>(l) , false};
    Buffer b2;
    b2.AddAtStart(l);
	serializeTLVOptions(b2.Begin(), YX, true);
    NS_TEST_ASSERT_MSG_EQ( std::ranges::equal( b2 ,rawTLVOptionsYX ),true , "serialization of TLVOptions failed for {OptY|OptX}" );

}


/**
 * \ingroup internet-test
 *
 * \brief IPv4 TestSuite
 */
class ExtnTestSuite : public TestSuite
{
  public:
    ExtnTestSuite()
        : TestSuite("SCIONExtensionHeaders", UNIT)
    {
        AddTestCase(new ExtnTestCase(), TestCase::QUICK);
    }
};

static ExtnTestSuite g_ExtnTestSuite; //!< Static variable for test initialization
