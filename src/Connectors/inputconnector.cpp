#include "inputconnector.h"

InputConnector::InputConnector(NodeParentWidget* nodeParentWidget, int subtype, bool canDisconnect) : ConnectorBase(nodeParentWidget, subtype, canDisconnect)
{
    this->typeOfConnector = INPUTCONNECTOR;
}

void InputConnector::save(YAML::Emitter* out)
{
    emit saveRequested(out);
}
