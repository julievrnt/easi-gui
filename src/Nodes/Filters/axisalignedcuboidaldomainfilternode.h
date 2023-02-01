#ifndef AXISALIGNEDCUBOIDALDOMAINFILTERNODE_H
#define AXISALIGNEDCUBOIDALDOMAINFILTERNODE_H

#include "src/Nodes/nodebase.h"
#include <QList>

class AxisAlignedCuboidalDomainFilterNode : public NodeBase
{
    Q_OBJECT
public:
    AxisAlignedCuboidalDomainFilterNode(QStringList* inputs = nullptr);
    AxisAlignedCuboidalDomainFilterNode(QStringList* inputs, QList<double>* values);

private:
    QList<double>* getValues();

    // NodeBase interface
protected:
    void addNewDimensionsLayoutRow(QVBoxLayout* dimensionsLayout, int index);
    void updateLayout();
    void saveValues(YAML::Emitter* out);
};

#endif // AXISALIGNEDCUBOIDALDOMAINFILTERNODE_H
