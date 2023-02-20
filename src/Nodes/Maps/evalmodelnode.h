#ifndef EVALMODELNODE_H
#define EVALMODELNODE_H

#include "src/Nodes/nodebase.h"

class EvalModelNode : public NodeBase
{
    Q_OBJECT
public:
    EvalModelNode(QStringList* inputs = nullptr, QStringList* outputs = nullptr);

    void setOutputConnectorModel(QGraphicsProxyWidget* newOutputConnectorModel);
    void performResize();

    OutputConnector* getOutputConnectorModel() const;

signals:
    void transferInputsRequested(QStringList* inputs);

private:
    QGraphicsProxyWidget* outputConnectorModel;
    void createLayout();
    void addModelLayout(QVBoxLayout* globalLayout);
    void addParametersLayout(QVBoxLayout* globalLayout);
    void addNewDimensionsLayoutRow(QVBoxLayout* dimensionsLayout, int index);

protected:
    void updateLayout();
    void saveValues(YAML::Emitter* out);
};

#endif // EVALMODELNODE_H
