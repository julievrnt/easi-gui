#ifndef NODE_H
#define NODE_H

#include <QWidget>
#include <QPoint>
#include <QGraphicsScene>
#include <QVBoxLayout>
#include <QGraphicsProxyWidget>
#include "qevent.h"
#include <QLabel>
#include "yaml-cpp/yaml.h"
#include "../Connectors/outputs/outputconnector.h"

#define NODE 0
#define ROOT 1

// maps
#define CONSTANTMAPNODE 2
#define AFFINEMAPNODE 3

// filters

class NodeBase : public QWidget
{
    Q_OBJECT
public:
    explicit NodeBase(QStringList* outputs = nullptr, QWidget* parent = nullptr);

    int getTypeOfNode();
    QStringList* getOutputs() const;

    QGraphicsProxyWidget* getInputConnector() const;
    void setInputConnector(QGraphicsProxyWidget* newInputConnector);
    QList<QGraphicsProxyWidget*>* getOutputConnectors() const;
    void addOutputConnector(QGraphicsProxyWidget* newOutputConnector);
    void addMathOutputConnector(QGraphicsProxyWidget* newMathOutputConnector);
    virtual OutputConnector* getFirstAvailableOutputConnector();
    virtual void performResize();

signals:
    void nodeContextMenuRequested(QPoint pos);
    void resized(QRectF rect);
    void addOutputConnectorRequested();
    void deleteOutputConnectorRequested(QGraphicsProxyWidget* outputConnectorProxy);
    void transferOutputsRequested(QStringList* outputs);
    void deleteNodeRequested(QGraphicsProxyWidget* proxyNode);

protected:
    int typeOfNode;
    QStringList* outputs;
    QGraphicsProxyWidget* proxyNode;
    QGraphicsProxyWidget* inputConnector;
    QList<QGraphicsProxyWidget*>* outputConnectors;
    QList<QGraphicsProxyWidget*>* mathOutputConnectors;
    void mousePressEvent(QMouseEvent* event);
    void paintEvent(QPaintEvent* event);

    void outputsChanged();
    void saveComponents(YAML::Emitter* out);

    // layouts for all nodes
    void addTitleLayout(QVBoxLayout* globalLayout);
    void addComponentsLayout(QVBoxLayout* globalLayout);

    void clearLayout(QLayout* layout, bool deleteWidgets = true);

    virtual void removeLayoutAtIndex(QLayout* layout, int index);
    virtual void updateLayout();
    virtual void saveNodeContent(YAML::Emitter* out);
    virtual void saveValues(YAML::Emitter* out);

private slots:
    void addOutputConnectorButtonClicked(bool clicked);
    void deleteOutputConnectorButtonClicked(bool clicked);
    void transferOutputs(QStringList* outputs);
    void save(YAML::Emitter* out);

    // QObject interface
public:
    bool event(QEvent* event);
    void setProxyNode(QGraphicsProxyWidget* newProxyNode);
    QGraphicsProxyWidget* getProxyNode() const;
};

#endif // NODE_H
