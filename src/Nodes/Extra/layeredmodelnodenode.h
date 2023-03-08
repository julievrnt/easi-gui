#ifndef LAYEREDMODELNODENODE_H
#define LAYEREDMODELNODENODE_H

#include "src/Nodes/nodebase.h"

class LayeredModelNodeNode : public NodeBase
{
    Q_OBJECT
public:
    LayeredModelNodeNode(QStringList* inputs = nullptr);

    void setValues(QStringList* inputs, QList<double>* values);

private:
    void addNewDimensionsLayoutRow(QVBoxLayout* dimensionsLayout, int index);
    void updateLayout();
    void saveNodeContent(YAML::Emitter* out);
    void saveValues(YAML::Emitter* out);
    QList<double>* getValues();
};

#endif // LAYEREDMODELNODENODE_H
