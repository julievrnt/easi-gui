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
    QMap<QString, double>* getValues();

private:
    void createLayout();
    void addNewOutputsLayoutRow(QVBoxLayout* outputsLayout, int index);
    void removeOldOutputsLayoutRow(QVBoxLayout* outputsLayout, int index);

    void updateLayout();
};

#endif // CONSTANTMAPNODE_H
