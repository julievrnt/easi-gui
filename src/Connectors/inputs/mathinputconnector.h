#ifndef MATHINPUTCONNECTOR_H
#define MATHINPUTCONNECTOR_H

#include "inputconnector.h"

class MathInputConnector : public InputConnector
{
    Q_OBJECT
public:
    MathInputConnector(NodeParentWidget* nodeParentWidget);
};

#endif // MATHINPUTCONNECTOR_H
