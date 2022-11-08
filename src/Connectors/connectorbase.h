#ifndef CONNECTORWIDGET_H
#define CONNECTORWIDGET_H

#include <QWidget>
#include "../Nodes/nodeparentwidget.h"

#define CONNECTOR 0
#define INPUTCONNECTOR 1
#define OUTPUTCONNECTOR 2

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

signals:
    void lineDragged();
    void lineNoMoreDragged();
    void createConnectorLineResquested(ConnectorBase* connector);
    void resetConnectorLineRequested(ConnectorBase* connector);
    void isConnectorLineOnMe(ConnectorBase* connector);
    void moveConnectorLineRequested(int connectorType);
    void deleteConnectorLineRequested();
    void transferOutputsRequested(QStringList* outputs);

private:
    bool highlight;
    bool connectorLineCreated;
    bool connectorLineConnected;
    QPointF centerPos;
    NodeParentWidget* nodeParentWidget;

    void updateCenterPos();

private slots:
    void nodeParentWidgetHasMoved();
    void transferOutputs(QStringList* outputs);

    // QWidget interface
protected:
    int typeOfConnector;
    QStringList* outputs;

    void paintEvent(QPaintEvent* event);
    void enterEvent(QEnterEvent* event);
    void leaveEvent(QEvent* event);
    void mousePressEvent(QMouseEvent* event);
    void mouseReleaseEvent(QMouseEvent* event);
    void mouseMoveEvent(QMouseEvent* event);
    void moveEvent(QMoveEvent* event);
};

#endif // CONNECTORWIDGET_H
