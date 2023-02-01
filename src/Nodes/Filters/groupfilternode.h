#ifndef GROUPFILTERNODE_H
#define GROUPFILTERNODE_H

#include "src/Nodes/nodebase.h"

class GroupFilterNode : public NodeBase
{
    Q_OBJECT
public:
    GroupFilterNode(QStringList* inputs = nullptr);
    GroupFilterNode(QStringList* inputs, QList<double>* values);

private:
    QList<double>* getValues();

    // NodeBase interface
protected:
    void addNewDimensionsLayoutRow(QVBoxLayout* dimensionsLayout, int index);
    void updateLayout();
    void saveValues(YAML::Emitter* out);
};

#endif // GROUPFILTERNODE_H
