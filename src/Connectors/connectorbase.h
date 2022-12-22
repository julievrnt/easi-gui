#ifndef CONNECTORWIDGET_H
#define CONNECTORWIDGET_H

#include <QWidget>
#include "../Nodes/nodeparentwidget.h"
#include "yaml-cpp/yaml.h"

// types
#define CONNECTOR 0
#define INPUTCONNECTOR 1
#define OUTPUTCONNECTOR 2

// subtypes
#define NONE 0
#define MATHS 1

class ConnectorBase : public QWidget
{
    Q_OBJECT
public:
    explicit ConnectorBase(NodeParentWidget* nodeParentWidget, QWidget* parent = nullptr);
    ~ConnectorBase();

    QStringList* getOutputs() const;
    void outputsChanged();
    int getTypeOfConnector() const;
    QPointF getCenterPos() const;
    void connectorLineDeleted();
    void setConnectorLineConnected(bool isConnected);
    void deleteLine();

    NodeParentWidget* getNodeParentWidget() const;

    void setConnectorLineCreated(bool newConnectorLineCreated);

    bool getConnectorLineConnected() const;

    int getSubtypeOfConnector() const;

signals:
    void lineDragged();
    void lineNoMoreDragged();
    void createConnectorLineResquested(ConnectorBase* connector);
    void resetConnectorLineRequested(ConnectorBase* connector);
    void isConnectorLineOnMe(ConnectorBase* connector);
    void moveConnectorLineRequested(int connectorType);
    void deleteConnectorLineRequested();
    void transferOutputsRequested(QStringList* outputs);
    void saveRequested(YAML::Emitter* out);

private:
    QStringList* outputs;
    bool highlight;
    NodeParentWidget* nodeParentWidget;

    void updateCenterPos();

private slots:
    void nodeParentWidgetHasMoved();
    void transferOutputs(QStringList* outputs);

    // QWidget interface
protected:
    int typeOfConnector;
    int subtypeOfConnector;
    QPointF centerPos;
    bool connectorLineCreated;
    bool connectorLineConnected;

    void paintEvent(QPaintEvent* event);
    void enterEvent(QEnterEvent* event);
    void leaveEvent(QEvent* event);
    void mousePressEvent(QMouseEvent* event);
    void mouseReleaseEvent(QMouseEvent* event);
    void mouseMoveEvent(QMouseEvent* event);
    void moveEvent(QMoveEvent* event);
};

#endif // CONNECTORWIDGET_H
