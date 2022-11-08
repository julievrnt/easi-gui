#ifndef NODE_H
#define NODE_H

#include <QWidget>
#include <QPoint>
#include <QGraphicsScene>
#include <QVBoxLayout>
#include <QGraphicsProxyWidget>
#include "qevent.h"
#include <QLabel>

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
    void performResize();

signals:
    void nodeContextMenuRequested(QPoint pos);
    void resized(QRectF rect);
    void addOutputConnectorRequested();
    void deleteOutputConnectorRequested();
    void transferOutputsRequested(QStringList* outputs);

protected:
    QGraphicsProxyWidget* inputConnector;
    QList<QGraphicsProxyWidget*>* outputConnectors;
    int typeOfNode;
    QStringList* outputs;
    QGraphicsScene* nodeScene;
    void mousePressEvent(QMouseEvent* event);
    void paintEvent(QPaintEvent* event);

    void outputsChanged();

    // layouts for all nodes
    void addTitleLayout(QVBoxLayout* globalLayout);
    void addComponentsLayout(QVBoxLayout* globalLayout);

    void clearLayout(QLayout* layout, bool deleteWidgets = true);

    virtual void updateLayout();

private slots:
    void addOutputConnectorButtonClicked(bool clicked);
    void deleteOutputConnectorButtonClicked(bool clicked);
    void transferOutputs(QStringList* outputs);

    // QObject interface
public:
    bool event(QEvent* event);
};

#endif // NODE_H
