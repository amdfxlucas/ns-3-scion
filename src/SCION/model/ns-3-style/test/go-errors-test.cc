/*
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation;
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 * Author: Faker Moatamri <faker.moatamri@sophia.inria.fr>
 *
 */


#include "ns3/go-errors.h"
#include "ns3/basic-error.h"
#include "ns3/log.h"

#include "ns3/test.h"


using namespace ns3;




/**
 * \ingroup internet-test
 *
 * \brief IPv4 Test
 */
class GoErrorTestCase : public TestCase
{
  public:
    GoErrorTestCase();
    ~GoErrorTestCase() override;
    void DoRun() override;
    void Test01();
    void Test02();
    void Test03();
    void Test04();
    
};

GoErrorTestCase::GoErrorTestCase()
    : TestCase("Verify the type erased GoErrors")
{
}

GoErrorTestCase::~GoErrorTestCase()
{
}



void GoErrorTestCase::Test01()
{

static_assert( "proto_error"_h != "basic_error"_h );
static_assert( proto_error::static_type() != basic_error::static_type() );


proto_error e1{"e1"};
error e4;

 NS_TEST_ASSERT_MSG_EQ( e4.is(e4),true,"" );

 NS_TEST_ASSERT_MSG_EQ( static_cast<bool>(e4),false,"");

error e2{e1};

 NS_TEST_ASSERT_MSG_EQ( e2.is(e1),true,"wrong error type comparison");

auto e3 = e2.As<proto_error>();


 NS_TEST_ASSERT_MSG_EQ( e3.value()->what(),"e1", "wrong error message");

e4=e2;

NS_TEST_ASSERT_MSG_EQ( e4.what(), "e1","");

#ifdef ERRORS_WITH_CONTEXT
std::string str{"errorneous value"};

basic_error be{ "catastrophe", e2, "key1","value1", "key2",str}; 
// e2 is the cause, not the _err


NS_TEST_ASSERT_MSG_EQ( be.what(), "catastrophe [ \"key1\": \"value1\", \"key2\": \"errorneous value\" ] cause: e1", "" );


basic_error be2{be, "key3",  "val3"};

NS_TEST_ASSERT_MSG_EQ( be2.what(), "catastrophe [ \"key1\": \"value1\", \"key2\": \"errorneous value\", \"key3\": \"val3\" ] cause: e1" ,"" );

basic_error be4{ *e3.value(), "key5", "value5"};

NS_TEST_ASSERT_MSG_EQ( be4.what(), "err: { e1 }" , "" );

NS_TEST_ASSERT_MSG_EQ( !e2.As<basic_error>() ,true, "");

 basic_error be3{ e2, "key5","value5"};

 NS_TEST_ASSERT_MSG_EQ( be3.what() , "err: { e1 }", "");

   basic_error tmp{"scmp_error", "typecode",
                     std::to_string(15),
                      "cause", be3.cause().what() }; 

// std::cout << be.As<proto_error>().value()->what() << std::endl;

#endif


}

void
GoErrorTestCase::DoRun()
{
  Test01();
  Test02();
  Test03();
  Test04();
}

void GoErrorTestCase::Test02()
{
    


}

void GoErrorTestCase::Test03()
{
  

}


void GoErrorTestCase::Test04()
{
    
}

class GoErrorTestSuite : public TestSuite
{
  public:
    GoErrorTestSuite()
        : TestSuite("GoError", UNIT)
    {
        AddTestCase(new GoErrorTestCase(), TestCase::QUICK);
    }
};

static GoErrorTestSuite _gerror_TestSuite; //!< Static variable for test initialization
