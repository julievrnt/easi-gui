#ifndef CONSTANTMAPNODE_H
#define CONSTANTMAPNODE_H

#include "node.h"
#include <QStringList>
#include <QVBoxLayout>

class ConstantMapNode : public Node
{
    Q_OBJECT
public:
    ConstantMapNode(QStringList* outputs);
    ~ConstantMapNode();
    QMap<QString, double>* getValues();

private:
    ///  TODO: test if I really need to keep it
    QStringList* outputs;
    void createLayout();
    void addNewOutputsLayoutRow(QVBoxLayout* outputsLayout, int index);

private slots:
    void insertNewOutputAtIndex(int index);

protected:
    void performResize() override;
};

#endif // CONSTANTMAPNODE_H
