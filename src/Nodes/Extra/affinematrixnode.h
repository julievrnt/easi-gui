#ifndef AFFINEMATRIXNODE_H
#define AFFINEMATRIXNODE_H

#include "src/Nodes/nodebase.h"
#include <QVBoxLayout>

class AffineMatrixNode : public NodeBase
{
    Q_OBJECT
public:
    AffineMatrixNode(QSharedPointer<QStringList> inputs = nullptr);

    void setValues(QSharedPointer<QStringList> inputs, QList<double>* values);

private:
    void addNewDimensionsLayoutRow(QVBoxLayout* dimensionsLayout, int index);
    void updateLayout();
    void saveNodeContent(YAML::Emitter* out);
    void saveValues(YAML::Emitter* out);
    QList<double>* getValues();
};

#endif // AFFINEMATRIXNODE_H
