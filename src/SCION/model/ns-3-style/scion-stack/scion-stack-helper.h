#pragma once
#include "ns3/object.h"

namespace ns3
{
    class Node;
    class NodeContainer;

    class SCIONStackHelper
    {
    public:
        void Install(Ptr<Node> node)const;
        void Install( NodeContainer c ) const;
        void CreateAndAggregateObjectFromTypeId(Ptr<Node> node, const std::string typeId);
    };
}