#pragma once

#include "ns3/go-errors.h"
#include "ns3/basic-error.h"
#include "ns3/scmp_typecodes.h"

namespace ns3
{
    /*
    type scmpError struct {
	TypeCode slayers.SCMPTypeCode
	Cause    error
}

func (e scmpError) Error() string {
	return serrors.New("scmp", "typecode", e.TypeCode, "cause", e.Cause).Error()
    */

class scmp_error
{
   _ERROR_TYPE_( scmp_error )
  
public:

constexpr static uint static_type(){return _hash_;}
constexpr uint type()const{return static_type(); }
constexpr scmp_error(){}

constexpr scmp_error( SCMPType type, SCMPCode code )
 : _type_code(type,code){}
scmp_error( SCMPType type, SCMPCode code ,const error& e)
 : _type_code(type,code),
 _cause(e){}
constexpr scmp_error( SCMPTypeCode tc ) : _type_code(tc){}
scmp_error( SCMPTypeCode tc,const error& e )
 : _type_code(tc),
  _cause(e) {}

//constexpr srcmp_error( std::string_view err ) : _what(err){}

bool is(const error& e )const { return e.type() == type(); }

std::string_view what()const
{
    basic_error tmp{"scmp_error", "typecode",
                     std::to_string(TypeCode().TypeCode() ),
                      "cause", _cause.what() }; 
    return tmp.what();
};

operator bool()const{return _type_code.TypeCode()!=0; } // or static_cast<bool>(_cause) ?!

SCMPTypeCode TypeCode() const{return _type_code;}
private:

//const std::string_view _what;
const SCMPTypeCode _type_code;
error _cause;



};

}