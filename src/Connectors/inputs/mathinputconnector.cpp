#include "mathinputconnector.h"

MathInputConnector::MathInputConnector(NodeParentWidget* nodeParentWidget): InputConnector(nodeParentWidget)
{
    subtypeOfConnector = MATH;
    canDisconnect = false;
}
