#include "outputconnector.h"

OutputConnector::OutputConnector(NodeParentWidget* nodeParentWidget) : ConnectorBase(nodeParentWidget)
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
