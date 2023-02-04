#ifndef AFFINEMAPNODE_H
#define AFFINEMAPNODE_H

#include "../nodebase.h"

class AffineMapNode : public NodeBase
{
    Q_OBJECT
public:
    AffineMapNode(QStringList* inputs = nullptr);
    ~AffineMapNode();

    void setValues(QMap<QString, QList<double>>* values);
    void addAffineMatrixProxy(QGraphicsProxyWidget* newAffineMatrixProxy);
    void addTranslationProxy(QGraphicsProxyWidget* newTranslationProxy);

    void performResize();
    void clearMathNodes();

signals:
    void addMathOutputConnectorRequested(QGraphicsProxyWidget* proxyNode, QPointF pos);

private:
    QList<QGraphicsProxyWidget*> affineMatrixProxies;
    QList<QGraphicsProxyWidget*> translationProxies;
    void addNewDimensionsLayoutRow(QVBoxLayout* dimensionsLayout, int index);
    void addMathsConnectors(int index);
    void removeMathsOfDimensionRow(int index);

protected:
    void updateLayout();
    void saveValues(YAML::Emitter* out);

    // NodeBase interface
protected slots:
    void addDimensionsLayoutRowRequested(bool clicked);
    void removeDimensionsLayoutRowRequested(bool clicked);
};

#endif // AFFINEMAPNODE_H
