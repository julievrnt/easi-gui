#include "outputconnector.h"

OutputConnector::OutputConnector(NodeParentWidget* nodeParentWidget, int subtype) : ConnectorBase(nodeParentWidget)
{
    this->typeOfConnector = OUTPUTCONNECTOR;
    this->subtypeOfConnector = subtype;
    if (subtype == MATH)
        canDisconnect = false;
}

void OutputConnector::saveComponent(YAML::Emitter* out)
{
    if (connectorLineConnected)
        emit saveRequested(out);
}

bool OutputConnector::isFree()
{
    return !(connectorLineConnected || connectorLineCreated);
}
