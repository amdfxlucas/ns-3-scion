#include "ns3/scion-stack-helper.h"
#include "ns3/node-container.h"
#include "ns3/object-factory.h"
namespace ns3
{

void SCIONStackHelper::Install(Ptr<Node> node )const 
{
/* EXAMPLE CODE from IPStackhelper
 if (m_ipv4Enabled)
    {
        // IPv4 stack 
        CreateAndAggregateObjectFromTypeId(node, "ns3::ArpL3Protocol");
        CreateAndAggregateObjectFromTypeId(node, "ns3::Ipv4L3Protocol");
        CreateAndAggregateObjectFromTypeId(node, "ns3::Icmpv4L4Protocol");
        if (!m_ipv4ArpJitterEnabled)
        {
            Ptr<ArpL3Protocol> arp = node->GetObject<ArpL3Protocol>();
            NS_ASSERT(arp);
            arp->SetAttribute("RequestJitter",
                              StringValue("ns3::ConstantRandomVariable[Constant=0.0]"));
        }

        // Set routing
        Ptr<Ipv4> ipv4 = node->GetObject<Ipv4>();
        if (!ipv4->GetRoutingProtocol())
        {
            Ptr<Ipv4RoutingProtocol> ipv4Routing = m_routing->Create(node);
            ipv4->SetRoutingProtocol(ipv4Routing);
        }
*/
}


void
SCIONStackHelper::CreateAndAggregateObjectFromTypeId(Ptr<Node> node, const std::string typeId)
{
    TypeId tid = TypeId::LookupByName(typeId);
    if (node->GetObject<Object>(tid))
    {
        return;
    }

    ObjectFactory factory;
    factory.SetTypeId(typeId);
    Ptr<Object> protocol = factory.Create<Object>();
    node->AggregateObject(protocol);
}

void SCIONStackHelper::Install( NodeContainer c ) const
{
 for (auto i = c.Begin(); i != c.End(); ++i)
    {
        Install(*i);
    }
}

}