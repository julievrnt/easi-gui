#ifndef FUNCTIONMAPNODE_H
#define FUNCTIONMAPNODE_H

#include "src/Nodes/nodebase.h"

class FunctionMapNode : public NodeBase
{
    Q_OBJECT
public:
    FunctionMapNode(QStringList* inputs = nullptr, QStringList* outputs = nullptr);

    void setValues(QMap<QString, QString>* values);
    void addFunctionNodeProxy(QGraphicsProxyWidget* newFunctionNodeProxy);

    void performResize();
    void clearNodes();

signals:
    void addFunctionOutputConnectorRequested(QGraphicsProxyWidget* proxyNode, QPointF pos);

private:
    QList<QGraphicsProxyWidget*> functionNodeProxies;
    void addFunctionConnector(int index);
    void removeFunctionOfDimensionRow(int index);

protected:
    void addNewDimensionsLayoutRow(QVBoxLayout *dimensionsLayout, int index);
    void addDimensionsLayoutRowRequested(bool clicked);
    void removeDimensionsLayoutRowRequested(bool clicked);
    void updateLayout();
    void saveValues(YAML::Emitter* out);
};

#endif // FUNCTIONMAPNODE_H
