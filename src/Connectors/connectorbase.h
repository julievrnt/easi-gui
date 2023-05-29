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
#define SPECIALCOMPONENT 1
#define MATH 2
#define EVAL 3

class ConnectorBase : public QWidget
{
    Q_OBJECT
public:
    explicit ConnectorBase(NodeParentWidget* nodeParentWidget, int subtype, bool canDisconnect);

    // getters and setters
    QSharedPointer<QStringList> getOutputs() const;

    int getTypeOfConnector() const;
    int getSubtypeOfConnector() const;
    void setSubtypeOfConnector(int newSubtypeOfConnector);

    QPointF getCenterPos() const;
    NodeParentWidget* getNodeParentWidget() const;

    bool getConnectorLineConnected() const;
    void setConnectorLineConnected(bool isConnected);
    void setConnectorLineCreated(bool newConnectorLineCreated);
    void deleteLine();

signals:
    void lineDragged();
    void lineNoMoreDragged();
    void createConnectorLineResquested(ConnectorBase* connector);
    void resetConnectorLineRequested(ConnectorBase* connector);
    void isConnectorLineOnMe(ConnectorBase* connector);
    void moveConnectorLineRequested(int connectorType);
    void deleteConnectorLineRequested();
    void transferOutputsRequested(QSharedPointer<QStringList> outputs);
    void saveRequested(YAML::Emitter* out);

protected:
    int typeOfConnector;
    int subtypeOfConnector;
    QPointF centerPos;
    // is false if a connector can not be disconnected from another connector (e.g. extra nodes)
    bool canDisconnect;
    bool connectorLineCreated;
    bool connectorLineConnected;

    // inherited from QWidget
    void paintEvent(QPaintEvent* event);
    void enterEvent(QEnterEvent* event);
    void leaveEvent(QEvent* event);
    void mousePressEvent(QMouseEvent* event);
    void mouseReleaseEvent(QMouseEvent* event);
    void mouseMoveEvent(QMouseEvent* event);
    void moveEvent(QMoveEvent* event);

private:
    QSharedPointer<QStringList> outputs;
    // indicates if the connector needs to be highlighted when the mouse is on it.
    bool highlight;
    NodeParentWidget* nodeParentWidget;

    void updateCenterPos();
    void outputsChanged();
    void connectorLineDeleted();

private slots:
    void nodeParentWidgetHasMoved();
    void transferOutputs(QSharedPointer<QStringList> outputs);
};

#endif // CONNECTORWIDGET_H
