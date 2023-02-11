#ifndef SWITCHCOMPONENTNODE_H
#define SWITCHCOMPONENTNODE_H

#include "src/Nodes/nodebase.h"

class SwitchComponentNode : public NodeBase
{
    Q_OBJECT
public:
    SwitchComponentNode(QStringList* inputs = nullptr, QStringList* outputs = nullptr);
    void setParameters(QStringList* parameters);
    void performResize();

private:
    QStringList* parameters;
    void createLayout();
    void addParametersLayout(QVBoxLayout* globalLayout);
    void addParameterLayout(QVBoxLayout* parametersLayout);

    // NodeBase interface
protected:
    void updateLayout();
    void addNewDimensionsLayoutRow(QVBoxLayout* dimensionsLayout, int index);
    void saveNodeContent(YAML::Emitter *out);
    void saveValues(YAML::Emitter* out);

private slots:
    void parameterNameChanged(QString newParameter);
};

#endif // SWITCHCOMPONENTNODE_H
