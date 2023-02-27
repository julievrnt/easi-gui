#ifndef FUNCTIONNODE_H
#define FUNCTIONNODE_H

#include "src/Nodes/nodebase.h"

class FunctionNode : public NodeBase
{
    Q_OBJECT
public:
    FunctionNode(QStringList* inputs = nullptr);

    void setValue(QStringList* inputs, QString values);

private:
    void addNewDimensionsLayoutRow(QVBoxLayout *dimensionsLayout, int index);
    void updateLayout();
    void saveNodeContent(YAML::Emitter* out);
    void saveValues(YAML::Emitter* out);
    QString getValue();
};

#endif // FUNCTIONNODE_H
