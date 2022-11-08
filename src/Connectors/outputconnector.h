#ifndef OUTPUTCONNECTOR_H
#define OUTPUTCONNECTOR_H

#include "connectorbase.h"
//#include "connectorline.h"

class OutputConnector : public ConnectorBase
{
    Q_OBJECT
public:
    OutputConnector(NodeParentWidget* nodeParentWidget);
};

#endif // OUTPUTCONNECTOR_H
