#ifndef INPUTCONNECTOR_H
#define INPUTCONNECTOR_H

#include "connectorbase.h"

class InputConnector : public ConnectorBase
{
    Q_OBJECT
public:
    InputConnector(NodeParentWidget* nodeParentWidget);
};

#endif // INPUTCONNECTOR_H
