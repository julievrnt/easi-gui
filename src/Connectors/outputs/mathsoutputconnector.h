#ifndef MATHSOUTPUTCONNECTOR_H
#define MATHSOUTPUTCONNECTOR_H

#include "outputconnector.h"

class MathsOutputConnector : public OutputConnector
{
    Q_OBJECT
public:
    MathsOutputConnector(NodeParentWidget* nodeParentWidget);
};

#endif // MATHSOUTPUTCONNECTOR_H
