#ifndef CONSTANTMAPNODE_H
#define CONSTANTMAPNODE_H

#include "src/Nodes/nodebase.h"
#include <QStringList>
#include <QVBoxLayout>

class ConstantMapNode : public NodeBase
{
    Q_OBJECT
public:
    ConstantMapNode(QGraphicsScene* nodeScene, QStringList* outputs);
    ~ConstantMapNode();
    QMap<QString, double>* getValues();

private:
    void createLayout();
    void addNewOutputsLayoutRow(QVBoxLayout* outputsLayout, int index);

private slots:
    void insertNewOutputAtIndex(QList<int> indexes);
};

#endif // CONSTANTMAPNODE_H
