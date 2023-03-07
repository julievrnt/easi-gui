#ifndef LAYEREDMODELNODE_H
#define LAYEREDMODELNODE_H

#include "src/Nodes/nodebase.h"

class LayeredModelNode : public NodeBase
{
    Q_OBJECT
public:
    LayeredModelNode();

    void performResize();

signals:
    void transferInputsRequested(QStringList* inputs);

private:
    void createLayout();
    void addModelLayout(QVBoxLayout* globalLayout);
    void addParametersLayout(QVBoxLayout* globalLayout);
    void addNewDimensionsLayoutRow(QVBoxLayout* dimensionsLayout, int index);

protected:
    void updateLayout();
    void saveValues(YAML::Emitter* out);
};

#endif // LAYEREDMODELNODE_H
