#include "ns3/basic-error.h"

namespace ns3
{

/* corresponds to 'WrapStr()'
wraps the cause with an error that has msg in the error message and
 adds the additional context. The returned error implements Is and Is(cause)
returns true.
*/
/*
basic_error::basic_error(std::string_view err, const error& cause)
    : _what(err),
      _cause(cause)
{
    //	return basicError{
    //    msg:    errOrMsg{str: msg},
    //    cause:  cause,
    //    fields: errCtxToFields(errCtx),
    //    stack:  st, }

    
} */

bool
basic_error::is(const error& e) const //{ return e.type() == type(); }
{
    if (e.type() == type())
    {
        return what() == e.what();
    }
    else
    {
        if (_err)
        {
            return _err == e;
        }
        else
        {
            return false;
        }
    }

    /*
    switch other := err.(type) {
    case basicError:
        return e.msg == other.msg
    default:
        if e.msg.err != nil {
            return e.msg.err == err
        }
        return false
    }
     */
}

std::string
basic_error::what() const
{
    std::string result;

    if (_err)
    {
        result.append("err: { ");
        result.append(_err.what());
        result += " } ";
    }
    else
    {
        result.append(_what);
    }

#ifdef ERRORS_WITH_CONTEXT
    for (size_t i = 0; const auto& [k, v] : m_ctx)
    {
        if (i == 0 && m_ctx.size() > 0)
        {
            result.append(" [ ");
        }

        result.append( "\"" + k +"\"" + ": " + "\"" + v + "\"" );
        if (i != m_ctx.size() - 1)
        {
            result.append(", ");
        }
        else
        {
            result.append(" ]");
        }
        ++i;
    }
#endif

    if (_cause)
    {
        result.append(" cause: ");
        result.append(_cause.what());
    }

    /*
    var buf bytes.Buffer
    buf.WriteString(e.msg.Error())
    if len(e.fields) != 0 {
        fmt.Fprint(&buf, " ")
        encodeContext(&buf, e.ctxPairs())
    }
    if e.cause != nil {
        fmt.Fprintf(&buf, ": %s", e.cause)
    }
    return buf.String()
    */

    return result;
}

} // namespace ns3