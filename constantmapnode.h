#ifndef CONSTANTMAPNODE_H
#define CONSTANTMAPNODE_H

#include "node.h"
#include <QStringList>
#include <QVBoxLayout>


class ConstantMapNode : public Node
{
    Q_OBJECT
public:
    ConstantMapNode(QStringList* constants);
    ~ConstantMapNode();
    QMap<QString, double>* getValues();

private:
    const int type = CONSTANTMAPNODE;
    QMap<QString, double> values;
    ///  TODO: test if I really need to keep it
    QStringList* constants;
    void createLayout();
    void addNewConstantsLayoutRow(QVBoxLayout* constantsLayout, int index);

private slots:
    void insertNewConstantAtIndex(int index);
};

#endif // CONSTANTMAPNODE_H
