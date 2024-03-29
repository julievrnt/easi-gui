#ifndef EVALMODELNODE_H
#define EVALMODELNODE_H

#include "src/Nodes/nodebase.h"

class EvalModelNode : public NodeBase
{
    Q_OBJECT
public:
    EvalModelNode(QSharedPointer<QStringList> inputs = nullptr, QSharedPointer<QStringList> outputs = nullptr);
    void performResize();

signals:
    void transferInputsRequested(QSharedPointer<QStringList> inputs);

private:
    void createLayout();
    void addModelLayout(QVBoxLayout* globalLayout);
    void addParametersLayout(QVBoxLayout* globalLayout);
    void addNewDimensionsLayoutRow(QVBoxLayout* dimensionsLayout, int index);

protected:
    void updateLayout();
    void saveValues(YAML::Emitter* out);
};

#endif // EVALMODELNODE_H
