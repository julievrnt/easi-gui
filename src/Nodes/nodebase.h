#ifndef NODE_H
#define NODE_H

#include <QWidget>
#include <QPoint>
#include <QGraphicsScene>
#include <QVBoxLayout>
#include <QGraphicsProxyWidget>
#include "qevent.h"

#define NODE 0
#define ROOT 1
#define CONSTANTMAPNODE 2

class NodeBase : public QWidget
{
    Q_OBJECT
public:
    explicit NodeBase(QWidget* parent = nullptr);

    int getTypeOfNode();
    QStringList* getOutputs() const;

    QGraphicsProxyWidget* getInputConnector() const;
    void setInputConnector(QGraphicsProxyWidget* newInputConnector);
    QList<QGraphicsProxyWidget*>* getOutputConnectors() const;
    void addOutputConnector(QGraphicsProxyWidget* newOutputConnector);
    void performResize(bool replaceConnectors);

signals:
    void nodeContextMenuRequested(QPoint pos);
    void resized(QRectF rect);
    void addOutputConnectorRequested();
    void deleteOutputConnectorRequested();

protected:
    QGraphicsProxyWidget* inputConnector;
    QList<QGraphicsProxyWidget*>* outputConnectors;
    int typeOfNode;
    QStringList* outputs;
    QGraphicsScene* nodeScene;
    void mousePressEvent(QMouseEvent* event);
    void paintEvent(QPaintEvent* event);

    // layouts for all nodes
    void addTitleLayout(QVBoxLayout* globalLayout);
    void addComponentsLayout(QVBoxLayout* globalLayout);

private slots:
    void addOutputConnectorButtonClicked(bool clicked);
    void deleteOutputConnectorButtonClicked(bool clicked);
};

#endif // NODE_H
