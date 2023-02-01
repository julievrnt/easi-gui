#ifndef CONSTANTMAPNODE_H
#define CONSTANTMAPNODE_H

#include "src/Nodes/nodebase.h"
#include <QStringList>
#include <QVBoxLayout>

class ConstantMapNode : public NodeBase
{
    Q_OBJECT
public:
    ConstantMapNode(QStringList* outputs = nullptr);
    ConstantMapNode(QStringList* outputs, QList<double>* values);
    ~ConstantMapNode();

private:
    QMap<QString, double>* getValues();

private slots:
    void dimensionNameChanged(QString newOutput);

protected:
    void addNewDimensionsLayoutRow(QVBoxLayout* dimensionsLayout, int index);
    void saveValues(YAML::Emitter* out);
};

#endif // CONSTANTMAPNODE_H
