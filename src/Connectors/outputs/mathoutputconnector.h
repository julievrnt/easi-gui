#ifndef MATHOUTPUTCONNECTOR_H
#define MATHOUTPUTCONNECTOR_H

#include "outputconnector.h"

class MathOutputConnector : public OutputConnector
{
    Q_OBJECT
public:
    MathOutputConnector(NodeParentWidget* nodeParentWidget);
};

#endif // MATHOUTPUTCONNECTOR_H
