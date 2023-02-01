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
    void addNewOutputsLayoutRow(QVBoxLayout* outputsLayout, int index);
    void removeOutputsLayoutRow(QVBoxLayout* outputsLayout, int index);
    QMap<QString, double>* getValues();

protected:
    void saveNodeContent(YAML::Emitter* out);
    void saveValues(YAML::Emitter* out);

    // NodeBase interface
protected:
    void addNewDimensionsLayoutRow(QVBoxLayout* dimensionsLayout, int index);
};

#endif // CONSTANTMAPNODE_H
