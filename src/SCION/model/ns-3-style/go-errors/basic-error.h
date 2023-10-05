#pragma once
#include "ns3/go-errors.h"

namespace ns3
{

class basic_error
{
    _ERROR_TYPE_(basic_error)

  public:
    template <class E>
    std::optional<E*> As() const
    {
        if (_err)
        {
            return _err.As<E>();
        }
        return std::nullopt;
    }

    /* corresponds to 'WithCtx()'
     returns an error that is the same as the given error but contains the
     additional context. The additional context is printed in the Error method.
     The returned error implements Is and Is(err) returns true.
     Deprecated: use WrapStr or New instead.
    */
    template <typename... Ts, typename = AllStrings<Ts...>>
    basic_error( error err, Ts... ts)
    {
        static_assert(divisible_by_two<Ts...>());

        if (const auto& be = err.As<basic_error>())
        {
            
            const_cast<std::string&>(_what) = be.value()->_what;
            _cause = be.value()->cause();

#ifdef ERRORS_WITH_CONTEXT
            m_ctx = be.value()->m_ctx;
            iteratePack2([&](const auto& k, const auto& v) { m_ctx[k] = v; }, ts...);

#endif
        }
        else
        {
            _err = err;
        }
    }

    /*// WithCtx
    func WithCtx(err error, errCtx ...interface{}) error {
        if top, ok := err.(basicError); ok {
            return basicError{
                msg:    top.msg,
                fields: combineFields(top.fields, errCtxToFields(errCtx)),
                cause:  top.cause,
                stack:  top.stack,
            }
        }

        return basicError{
            msg:    errOrMsg{err: err},
            fields: errCtxToFields(errCtx),
        }
    */

    constexpr static uint static_type()
    {
        return _hash_;
    }

    constexpr uint type() const
    {
        return static_type();
    }

    basic_error()
    {
    }

    /* connesponds to 'New()' */
    template <typename... Ts, typename = AllStringorView<Ts...>>    
    basic_error(std::string_view msg,  Ts&&... ts)
        : _what(msg)          
    {
        static_assert(divisible_by_two<Ts...>());
#ifdef ERRORS_WITH_CONTEXT

        iteratePack2([&](const auto& k, const auto& v) { m_ctx[k] = v; }, std::forward<Ts>(ts)...);

#endif
    }

    // basic_error(std::string_view msg, const error& cause);

    /*corresponds to 'WrapStr()' with context
     */
    template <typename... Ts, typename = AllStrings<Ts...>>
    // ,typename = typename std::enable_if<    sizeof...(Ts) %2== 0   >::type >
    basic_error(std::string_view msg, error cause, Ts... ts)
        : _what(msg),
          _cause( std::move(cause) )
    {
        static_assert(divisible_by_two<Ts...>());
#ifdef ERRORS_WITH_CONTEXT

        iteratePack2([&](const auto& k, const auto& v) { m_ctx[k] = v; }, ts...);

#endif
    }

    bool is(const error& e) const;
    std::string what() const;

    const error& cause() const
    {
        return _cause;
    }

    operator bool() const
    {
        return !_what.empty();
    } // || _err

  private:
    error _err;
    const std::string _what;

    error _cause;

#ifdef ERRORS_WITH_CONTEXT
    boost::container::flat_map<std::string, std::string> m_ctx;
#endif
};
} // namespace ns3