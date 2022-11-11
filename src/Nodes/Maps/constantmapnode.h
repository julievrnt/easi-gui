#ifndef CONSTANTMAPNODE_H
#define CONSTANTMAPNODE_H

#include "src/Nodes/nodebase.h"
#include <QStringList>
#include <QVBoxLayout>

class ConstantMapNode : public NodeBase
{
    Q_OBJECT
public:
    ConstantMapNode(QGraphicsScene* nodeScene);
    ~ConstantMapNode();

private:
    void createLayout();
    void addNewOutputsLayoutRow(QVBoxLayout* outputsLayout, int index);
    void removeOldOutputsLayoutRow(QVBoxLayout* outputsLayout, int index);
    QMap<QString, double>* getValues();

    void updateLayout();

protected:
    void saveNodeContent(YAML::Emitter* out);
    void saveValues(YAML::Emitter* out);
};

#endif // CONSTANTMAPNODE_H
