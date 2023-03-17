#ifndef FUNCTIONNODE_H
#define FUNCTIONNODE_H

#include "src/Nodes/nodebase.h"

class FunctionNode : public NodeBase
{
    Q_OBJECT
public:
    FunctionNode(QSharedPointer<QStringList> inputs = nullptr);

    void setValue(QSharedPointer<QStringList> inputs, QString value);

private:
    void createLayout();
    void addInputsLayout(QVBoxLayout* globalLayout);
    void addFunctionLayout(QVBoxLayout* globalLayout);
    void updateLayout();
    void saveNodeContent(YAML::Emitter* out);
    void saveValues(YAML::Emitter* out);
    QString getValue();

private slots:
    void inputButtonClicked(bool clicked);
};

#endif // FUNCTIONNODE_H
