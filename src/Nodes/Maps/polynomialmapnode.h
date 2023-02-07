#ifndef POLYNOMIALMAPNODE_H
#define POLYNOMIALMAPNODE_H

#include "src/Nodes/nodebase.h"

class PolynomialMapNode : public NodeBase
{
    Q_OBJECT
public:
    PolynomialMapNode(QStringList* inputs = nullptr, QStringList* outputs = nullptr);

    void setValues(QMap<QString, QList<double>>* values);
    void addPolynomialMatrixProxy(QGraphicsProxyWidget* newPolynomialMatrixProxy);

    void performResize();
    void clearMathNodes();

    int getDegree() const;
    void setDegree(int newDegree);

signals:
    void addMathOutputConnectorRequested(QGraphicsProxyWidget* proxyNode, QPointF pos);

private:
    int degree = 0;
    QList<QGraphicsProxyWidget*> polynomialMatrixProxies;
    void addMathConnector(int index);
    void removeMathOfDimensionRow(int index);

private slots:
    void degreeHasChanged(int newDegree);

    // NodeBase interface
protected:
    void addNewDimensionsLayoutRow(QVBoxLayout* dimensionsLayout, int index);
    void addDimensionsLayoutRowRequested(bool clicked);
    void removeDimensionsLayoutRowRequested(bool clicked);
    void updateLayout();
    void saveValues(YAML::Emitter* out);
};

#endif // POLYNOMIALMAPNODE_H
