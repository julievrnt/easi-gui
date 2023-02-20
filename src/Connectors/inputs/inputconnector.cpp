#include "inputconnector.h"

InputConnector::InputConnector(NodeParentWidget* nodeParentWidget, int subtype) : ConnectorBase(nodeParentWidget)
{
    this->typeOfConnector = INPUTCONNECTOR;
    subtypeOfConnector = subtype;
    if (subtype == MATH)
        canDisconnect = false;
}

void InputConnector::save(YAML::Emitter* out)
{
    emit saveRequested(out);
}
