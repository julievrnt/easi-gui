#ifndef OUTPUTCONNECTOR_H
#define OUTPUTCONNECTOR_H

#include "connectorbase.h"

class OutputConnector : public ConnectorBase
{
    Q_OBJECT
public:
    OutputConnector(NodeParentWidget* nodeParentWidget);

    void saveComponent(YAML::Emitter* out);
};

#endif // OUTPUTCONNECTOR_H
