#ifndef SPHERICALDOMAINFILTERNODE_H
#define SPHERICALDOMAINFILTERNODE_H

#include "src/Nodes/nodebase.h"
#include <QMap>

class SphericalDomainFilterNode : public NodeBase
{
    Q_OBJECT
public:
    SphericalDomainFilterNode(QSharedPointer<QStringList> inputs = nullptr);
    SphericalDomainFilterNode(QSharedPointer<QStringList> inputs, QList<double>* values);

private:
    QMap<QString, double>* getCenterValues();

    // NodeBase interface
protected:
    void addNewDimensionsLayoutRow(QVBoxLayout* dimensionsLayout, int index);
    void updateLayout();
    void saveValues(YAML::Emitter* out);
};

#endif // SPHERICALDOMAINFILTERNODE_H
