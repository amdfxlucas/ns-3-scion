#include "ns3/scion-snet-path-meta-data.h"
#include "ns3/sha256.h"

namespace ns3
{
// Fingerprint uniquely identifies the path based on the sequence of
// ASes and BRs, i.e. by its PathInterfaces.
// Other metadata, such as MTU or NextHop have no effect on the fingerprint.
// Returns empty string for paths where the interfaces list is not available.
PathHash_t
PathMetaData::Fingerprint() const
{
    std::stringstream ss;
    for (const auto& i : _interfaces)
    {
        ss << i._ia << i._id;
    }

    return sha256(ss.str());
}

bool
PathMetaData::IsInterfaceOnPath(const PathInterface& pi) const
{
    for (const auto& i : _interfaces)
    {
        if (i == pi)
        {
            return true;
        }
    }
    return false;
}
} // namespace ns3
