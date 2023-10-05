#include "ns3/scion-reply-selector.h"
#include "ns3/scion-snet-path.h"
#include "ns3/nstime.h"
#include "ns3/simulator.h" // only to get the time with Now()
#include <ranges>

namespace ns3
{

void
SCIONDefaultReplySelector::remoteEntry_t::insert(const SNETPath& p, size_t maxEntries)
{   size_t i = 0;
    for ( const auto& p : m_paths)
    {
        if (p.Fingerprint() == p.Fingerprint())
        {
            break;
        }
        ++i;
    }
    if (i == m_paths.size())
    {
        if (m_paths.size() < maxEntries)
        {
            // weird go code
        }
        else
        {
            i = m_paths.size() - 1; // overwrite LRU
        }
   
    }
    m_paths[i] = p;
         if (i != 0)
        {
            // auto pi = m_paths[i];
            //m_paths[i:i+1] = paths[0:i];
            // m_paths[0] = pi;
        }
}
            // interface has to change to return std::shared_ptr<SNETPath> as it can be null
          const SNETPath& SCIONDefaultReplySelector::Path( const SCIONAddress& remote ) const
          {
            auto iter =  m_remotes.find( remote);

          //  if ( iter != m_remotes.end() )
          //  {
                return iter->second.m_paths[0];
           // }
          }

        
         void SCIONDefaultReplySelector::Initialize( const SCIONAddress& local)
         {
                // nothing to do
         }


        
        void SCIONDefaultReplySelector::Record( const SNETPath& path, const SCIONAddress& remote)
        {
            auto& r = m_remotes[remote];
            r.m_lastSeen = Simulator::Now();
            m_remotes[remote].insert( path, DefaultReplySelectorMaxPaths );
            m_remotes[ remote] = r;
        }

        
         void SCIONDefaultReplySelector::PathDown( PathHash_t hash, PathInterface pi )
         {
            // TODO: failover
         }

         void SCIONDefaultReplySelector::Close()
         {
            // nothing to do
         }
}