#include "inputconnector.h"

InputConnector::InputConnector(NodeParentWidget* nodeParentWidget) : ConnectorBase(nodeParentWidget)
{
    this->typeOfConnector = INPUTCONNECTOR;
}

void InputConnector::save(YAML::Emitter* out)
{
    emit saveRequested(out);
}
