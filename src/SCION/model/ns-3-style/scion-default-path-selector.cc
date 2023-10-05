#include "ns3/scion-default-path-selector.h"

namespace ns3
{

// if Selector has no paths, this deliberately throws an exception and ends the simulation
const SNETPath& SCIONDefaultPathSelector::Path()const
{
    return m_paths.at(m_current);

}


void SCIONDefaultPathSelector::Initialize( const SCIONAddress& local,
                                        const SCIONAddress& remote,
                                         std::vector<SNETPath> paths )
{
    m_paths = paths;
    m_current = 0;
}

void SCIONDefaultPathSelector::Refresh( const std::vector<SNETPath>& newPaths)
{
int newcurrent =0;
if (newPaths.size() > 0)
{
    auto currentPathFingerprint = m_paths.at(m_current).Fingerprint();
    for( int i = 0; const auto & p : newPaths )
    {
        if( p.Fingerprint() == currentPathFingerprint )
        {
            newcurrent = i;
            break;
        }
        ++i;
    }

    m_paths = newPaths;
    m_current = newcurrent;
}
}


void SCIONDefaultPathSelector::PathDown( PathHash_t pf, PathInterface pi )
{
if( const auto& currentPath = m_paths.at(m_current); currentPath.IsInterfaceOnPath(pi) || pf == currentPath.Fingerprint()  )
{

}
}

void SCIONDefaultPathSelector::Close()
{

}

}