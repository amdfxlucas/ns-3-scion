#include "ns3/raw-path.h"
#include "neo/mutable_buffer.hpp"
#include "ns3/basic-error.h"

namespace ns3
{

/*// IsLastHop returns whether the current hop is the last hop on the path.
func (s *Raw) IsLastHop() bool {
    return int(s.PathMeta.CurrHF) == (s.NumHops - 1)
}
*/
bool
RawPath::IsLastHop() const
{
    return base.PathMeta.CurrHF == base.NumHops - 1;
}

// IncPath increments the path and writes it to the buffer.
error
RawPath::IncPath()
{
    auto err = base.IncPath();
	if(err){ return err; }
    auto iter = neo::bytewise_iterator(neo::as_buffer(raw));
    base.PathMeta.Serialize(iter);
	return {};
}

/*
    return s.PathMeta.SerializeTo(s.Raw[:MetaLen])
}
*/

// DecodeFromBytes only decodes the PathMetaHeader. Otherwise the nothing is decoded and simply kept
// as raw bytes.
/*func (s *Raw) DecodeFromBytes(data []byte) error {
    if err := s.Base.DecodeFromBytes(data); err != nil {
        return err
    }
    pathLen := s.Len()
    if len(data) < pathLen {
        return serrors.New("RawPath raw too short", "expected", pathLen, "actual", len(data))
    }
    s.Raw = data[:pathLen]
    return nil
}*/

// SerializeTo writes the path to a slice. The slice must be big enough to hold the entire data,
// otherwise an error is returned.
/*func (s *Raw) SerializeTo(b []byte) error {
    if s.Raw == nil {
        return serrors.New("raw is nil")
    }
    if minLen := s.Len(); len(b) < minLen {
        return serrors.New("buffer too small", "expected", minLen, "actual", len(b))
    }
    // XXX(roosd): This modifies the underlying buffer. Consider writing to data
    // directly.
    if err := s.PathMeta.SerializeTo(s.Raw[:MetaLen]); err != nil {
        return err
    }
    copy(b, s.Raw)
    return nil
}*/

// IsPenultimateHop returns whether the current hop is the penultimate hop on the path.
bool
RawPath::IsPenultimateHop() const
{
    return base.PathMeta.CurrHF == (base.NumHops - 2);
}

// IsFirstHop returns whether the current hop is the first hop on the path.
bool
RawPath::IsFirstHop() const
{
    return base.PathMeta.CurrHF == 0;
}

void
RawPath::Serialize(buffer_iterator start) const
{
	 auto buff = neo::mutable_buffer( const_cast<neo::bytes&>( raw) );
	 // neo::mutable_buffer( neo::byte_pointer(raw.data()), raw.size());
    auto iter = neo::bytewise_iterator(buff);

    // auto pathlen = base.Len();
    base.Serialize(iter); 

   
    std::ranges::copy(iter.begin(), iter.end(), start);
}

std::expected<uint32_t,error>
RawPath::Deserialize([[maybe_unused]] const_buffer_iterator start)
{
   if (  auto tmp = base.Deserialize(start); !tmp)
   {
    return std::unexpected( tmp.error() );
   }
   
    raw = neo::bytes::copy(neo::as_buffer(&(*start), base.Len()));
    // raw = neo::bytes::copy( neo::as_buffer( start ) );
    // return raw.size();
    return base.Len();
}

/*
 uint32_t RawPath::Deserialize( Buffer::ConstIterator start )
 {
    base.Deserialize( start );
    auto pathLen = base.Len();
    auto end = raw.Begin();
    end.Next(pathLen);

  //  raw.Begin().Write(start,end );// , pathLen
  //  m_bytes = neo::bytes::copy( neo::as_buffer( &(*m_start), m_start.GetRemainingSize()) );
   raw = neo::bytes::copy( neo::as_buffer( start ) );

  return 5555;
 }

void RawPath::Serialize( [[maybe_unused]] Buffer::Iterator start ) const
{
    auto pathlen = base.Len();
    base.Serialize( start );

    auto buff = neo::const_buffer(raw.data(),raw.size());
    auto iter = neo::bytewise_iterator(buff);
    std::ranges::copy( iter.begin(),iter.end() ,start ) ;


  //  start.Write( raw, raw.Next( pathLen ) );
    // or raw.Read( start, pathLen );
}*/

/*

// GetInfoField returns the InfoField at a given index.
func (s *Raw) GetInfoField(idx int) (path.InfoField, error) {
if idx >= s.NumINF {
    return path.InfoField{},
        serrors.New("InfoField index out of bounds", "max", s.NumINF-1, "actual", idx)
}
infOffset := MetaLen + idx*path.InfoLen
info := path.InfoField{}
if err := info.DecodeFromBytes(s.Raw[infOffset : infOffset+path.InfoLen]); err != nil {
    return path.InfoField{}, err
}
return info, nil
}
*/

std::expected<InfoField,error>
RawPath::GetInfoField(int index) const
{
   // NS_ASSERT_MSG(index < base.NumINF, "InfoField index out of bounds");
   if( index >= base.NumINF ) 
   {
		return std::unexpected( basic_error{"InfoField index out of bounds",
							 "max", std::to_string( base.NumINF-1),
							  "actual", std::to_string( index) } );
	}

    [[maybe_unused]] auto infOffset = METALEN + index * INFO_FIELD_LEN;
    InfoField info;
    //  auto iter = raw.Begin();
    //  iter.Next(infOffset);
    auto iter = neo::bytewise_iterator(neo::as_buffer(raw));
    iter += infOffset;
    info.Deserialize(iter);
    return info;
}

/*
// GetHopField returns the HopField at a given index.
func (s *Raw) GetHopField(idx int) (path.HopField, error) {
    if idx >= s.NumHops {
        return path.HopField{},
            serrors.New("HopField index out of bounds", "max", s.NumHops-1, "actual", idx)
    }
    hopOffset := MetaLen + s.NumINF*path.InfoLen + idx*path.HopLen
    hop := path.HopField{}
    if err := hop.DecodeFromBytes(s.Raw[hopOffset : hopOffset+path.HopLen]); err != nil {
        return path.HopField{}, err
    }
    return hop, nil
}*/

std::expected<HopField,error>
RawPath::GetHopField(int index) const
{
    // NS_ASSERT_MSG(index < base.NumHops, "HopField index out of bounds");
	if( index >= base.NumHops )
	{
		return std::unexpected( basic_error{"HopField index out of bounds", "max",std::to_string( base.NumHops-1), "actual",std::to_string( index) } );
	}

    [[maybe_unused]] auto hopOffset = METALEN + base.NumINF * INFO_FIELD_LEN + index * HOPLEN;

    HopField hop;
    auto iter = neo::bytewise_iterator(neo::as_buffer(raw));
    iter += hopOffset;
    hop.Deserialize(iter);
    return hop;
}

/*
// GetCurrentInfoField is a convenience method that returns the current hop field pointed to by the
// CurrINF index in the path meta header.
func (s *Raw) GetCurrentInfoField() (path.InfoField, error) {
    return s.GetInfoField(int(s.PathMeta.CRawPath( neo::const_buffer buff );
RawPath( neo::bytes&&  );urrINF))
}
*/
std::expected<InfoField,error>
RawPath::GetCurrentInfoField() const
{
    return GetInfoField(base.PathMeta.CurrINF);
}

/*
// GetCurrentHopField is a convenience method that returns the current hop field pointed to by the
// CurrHF index in the path meta header.
func (s *Raw) GetCurrentHopField() (path.HopField, error) {
    return s.GetHopField(int(s.PathMeta.CurrHF))
}*/

std::expected<HopField,error>
RawPath::GetCurrentHopField() const
{
    return GetHopField(base.PathMeta.CurrHF);
}

void
RawPath::Reverse()
{
/*
// XXX(shitz): The current implementation is not the most performant, since it parses the entire
	// path first. If this becomes a performance bottleneck, the implementation should be changed to
	// work directly on the raw representation.

	decoded, err := s.ToDecoded()
	if err != nil {
		return nil, err
	}
	reversed, err := decoded.Reverse()
	if err != nil {
		return nil, err
	}
	if err := reversed.SerializeTo(s.Raw); err != nil {
		return nil, err
	}
	err = s.DecodeFromBytes(s.Raw)
	return s, err
*/

DecodedPath decoded {ConvertToDecoded() };

decoded.Reverse();

decoded.Serialize( neo::bytewise_iterator( neo::as_buffer(raw) ) );

Deserialize( neo::bytewise_iterator( neo::as_buffer(raw) ) );

}

// SetInfoField updates the InfoField at a given index.
void
RawPath::SetInfoField(const InfoField& info, int idx)
{
    auto iter = neo::bytewise_iterator(neo::as_buffer(raw));
    auto infOffset = METALEN + idx * INFO_FIELD_LEN;
    info.Serialize(iter + infOffset);
}

/*
func (s *Raw) SetInfoField(info path.InfoField, idx int) error {
    if idx >= s.NumINF {
        return serrors.New("InfoField index out of bounds", "max", s.NumINF-1, "actual", idx)
    }
    infOffset := MetaLen + idx*path.InfoLen
    return info.SerializeTo(s.Raw[infOffset : infOffset+path.InfoLen])
}
*/

// SetHopField updates the HopField at a given index.
void
RawPath::SetHopField(const HopField& hop, int idx)
{
    auto hopOffset = METALEN + base.NumINF * INFO_FIELD_LEN + idx * HOPLEN;

    auto iter = neo::bytewise_iterator(neo::as_buffer(raw));

    hop.Serialize(iter + hopOffset);
}

/*
func (s *Raw) SetHopField(hop path.HopField, idx int) error {
    if idx >= s.NumHops {
        return serrors.New("HopField index out of bounds", "max", s.NumHops-1, "actual", idx)
    }
    hopOffset := MetaLen + s.NumINF*path.InfoLen + idx*path.HopLen
    return hop.SerializeTo(s.Raw[hopOffset : hopOffset+path.HopLen])
}*/

RawPath::RawPath( const Base& b, neo::bytes&& buff )
: base(b),
  raw( std::move(buff) )
   {
	Base bb;
    bb.Deserialize(neo::bytewise_iterator(neo::as_buffer(raw)));
    NS_ASSERT_MSG(bb == base, "Constructor called with invalid arguments !");
   }

RawPath::RawPath(neo::const_buffer buff)
{
    //	raw = neo::bytes::copy(  buff );
    Deserialize(neo::bytewise_iterator(buff));
}

// actually this is dangerous, as integrity could be violated.
//  buff could contain something different than base
RawPath::RawPath(const Base& b, neo::const_buffer buff)
    : base(b)
{
    raw = neo::bytes::copy(buff);
    Base bb;
    bb.Deserialize(neo::bytewise_iterator(neo::as_buffer(raw)));
    NS_ASSERT_MSG(bb == base, "Constructor called with invalid arguments !");
}

RawPath::RawPath(neo::bytes&& b)
    : raw(std::move(b))
{
    base.Deserialize(neo::bytewise_iterator(neo::as_buffer(raw)));
}

/*
    Path& RawPath::Reverse()
    {

    }

    */
DecodedPath
RawPath::ConvertToDecoded()const
{
    // Serialize PathMeta to ensure potential changes are reflected Raw.

    auto iter = neo::bytewise_iterator(neo::as_buffer( const_cast<neo::bytes&>(raw) ) );
    base.Serialize(iter);
   // iter += base.GetSerializedSize();

    DecodedPath decoded{};
    decoded.Deserialize(iter);

    return decoded;
}

/*    Path RawPath::ConvertTo(const RawPath& )
    {

    }
    RawPath RawPath::ConvertFrom(const Path& )
    {

    }
*/

} // namespace ns3