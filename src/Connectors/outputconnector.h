#ifndef OUTPUTCONNECTOR_H
#define OUTPUTCONNECTOR_H

#include "connectorbase.h"

class OutputConnector : public ConnectorBase
{
    Q_OBJECT
public:
    OutputConnector(NodeParentWidget* nodeParentWidget, int subtype = NONE, bool canDisconnect = true);

    void saveComponent(YAML::Emitter* out);
    bool isFree();
};

#endif // OUTPUTCONNECTOR_H
