#pragma once

#include "ns3/scion-path-selector.h"
#include <vector>

namespace ns3
{

// DefaultSelector is a Selector for a single dialed socket.
// This will keep using the current path, starting with the first path chosen
// by the policy, as long possible.
// Faults are detected passively via SCMP down notifications; whenever such
// a down notification affects the current path, the DefaultSelector will
// switch to the first path (in the order defined by the policy) that is not
// affected by down notifications.
class SCIONDefaultPathSelector
{
public:
	// Path selects the path for the next packet.
	// Invoked for each packet sent with Write.
    const SNETPath& Path()const; 
    // better std::optional<Path> here ?! as it can be empty
    // or std::shared_ptr<Path>
    // on the otherhand dialedConn throws an error anyway if Selcector returns nil


// Initialize the selector for a connection with the initial list of paths,
	// filtered/ordered by the Policy.
	// Invoked once during the creation of a Conn.

void Initialize( const SCIONAddress& local,
                const SCIONAddress& remote,
				 std::vector< SNETPath> paths );

// Refresh updates the paths. This is called whenever the Policy is changed or
	// when paths were about to expire and are refreshed from the SCION daemon.
	// The set and order of paths may differ from previous invocations.
void Refresh( const std::vector<SNETPath>& newPaths);

// PathDown is called whenever an SCMP down notification is received on any
	// connection so that the selector can adapt its path choice. The down
	// notification may be for unrelated paths not used by this selector.
void PathDown( PathHash_t, PathInterface );

void Close();
private:

std::vector< SNETPath > m_paths;
int m_current;

};
}