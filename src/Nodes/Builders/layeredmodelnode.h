#ifndef LAYEREDMODELNODE_H
#define LAYEREDMODELNODE_H

#include "src/Nodes/nodebase.h"
#include <QGridLayout>

class LayeredModelNode : public NodeBase
{
    Q_OBJECT
public:
    LayeredModelNode(QStringList* inputs = nullptr, QStringList* outputs = nullptr, QString interpolation = "lower");

    void setValues(QMap<double, QList<double>>* values);
    void performResize();
    void clearNodes();

    void addNodeProxy(QGraphicsProxyWidget*& newNodeProxy);

signals:
    void transferInputsRequested(QStringList* inputs);
    void addMathOutputConnectorRequested(QGraphicsProxyWidget* proxyNode, QPointF pos);

private:
    QString interpolation;
    QList<QGraphicsProxyWidget*> nodeProxies;

    void createLayout();
    void addModelLayout(QVBoxLayout* globalLayout);
    void addInterpolationLayout(QVBoxLayout* globalLayout);
    void addParametersLayout(QVBoxLayout* globalLayout);
    void addNodesLayout(QVBoxLayout* globalLayout);
    void addNewDimensionsLayoutRow(QVBoxLayout* dimensionsLayout, int index);
    void addNewNodesLayoutRow(QVBoxLayout* nodesLayout, int index);

    void addNodeConnector(int index);
    void removeNodeOfRow(int index);

    QList<double>* getNodesValues();

private slots:
    void setInterpolation(const QString& newInterpolation);
    void addNodesLayoutRowRequested(bool clicked);
    void removeNodesLayoutRowRequested(bool clicked);

protected:
    void updateLayout();
    void saveValues(YAML::Emitter* out);
};

#endif // LAYEREDMODELNODE_H
