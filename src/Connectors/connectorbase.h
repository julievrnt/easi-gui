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
    ~ConnectorBase();

    QSharedPointer<QStringList> getOutputs() const;
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

    void setSubtypeOfConnector(int newSubtypeOfConnector);

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

private:
    QSharedPointer<QStringList> outputs;
    bool highlight;
    NodeParentWidget* nodeParentWidget;

    void updateCenterPos();

private slots:
    void nodeParentWidgetHasMoved();
    void transferOutputs(QSharedPointer<QStringList> outputs);

    // QWidget interface
protected:
    int typeOfConnector;
    int subtypeOfConnector;
    QPointF centerPos;
    bool canDisconnect;
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
