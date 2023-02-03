#ifndef AFFINEMAPNODE_H
#define AFFINEMAPNODE_H

#include "../nodebase.h"

class AffineMapNode : public NodeBase
{
    Q_OBJECT
public:
    AffineMapNode(QStringList* inputs = nullptr);
    ~AffineMapNode();

    QList<QGraphicsProxyWidget*>* getMathOutputConnectors() const;

    void setValues(QMap<QString, QList<double>>* values);
    void addMatrixProxy(QGraphicsProxyWidget* newMatrixProxy);
    void addTranslationProxy(QGraphicsProxyWidget* newTranslationProxy);

signals:
    void addMathOutputConnectorRequested(QGraphicsProxyWidget* proxyNode, QPointF pos);

private:
    QList<QGraphicsProxyWidget*> matrixProxies;
    QList<QGraphicsProxyWidget*> translationProxies;
    void addNewDimensionsLayoutRow(QVBoxLayout* dimensionsLayout, int index);
    void addMathsConnectors(int index);

private slots:
    void dimensionNameChanged(QString newOutput);

protected:
    void updateLayout();
    void saveValues(YAML::Emitter* out);

    // NodeBase interface
protected:
    void removeMathsOfDimensionRow(int index);

    // NodeBase interface
public:
    void performResize();

    // NodeBase interface
protected slots:
    void addDimensionsLayoutRowRequested(bool clicked);
    void removeDimensionsLayoutRowRequested(bool clicked);
};

#endif // AFFINEMAPNODE_H
