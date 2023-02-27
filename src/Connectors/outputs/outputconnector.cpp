#include "outputconnector.h"

OutputConnector::OutputConnector(NodeParentWidget* nodeParentWidget, int subtype, bool canDisconnect) : ConnectorBase(nodeParentWidget, subtype, canDisconnect)
{
    this->typeOfConnector = OUTPUTCONNECTOR;
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
