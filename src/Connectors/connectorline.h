#ifndef CONNECTORLINE_H
#define CONNECTORLINE_H

#include <QWidget>
#include "inputconnector.h"
#include "outputconnector.h"
#include <QTimer>

class ConnectorLine : public QWidget
{
    Q_OBJECT
public:
    explicit ConnectorLine(ConnectorBase* connector, QPointF startPoint, QWidget* parent = nullptr);

    void setConnectorLineProxy(QGraphicsProxyWidget* newConnectorLineProxy);
    QPointF getPositionToCheck();

signals:
    void drawnIsDone(ConnectorLine* connectorLine);
    void deleteConnectorLine(QGraphicsProxyWidget* connectorLineProxy);
    void transferOutputsRequested(QStringList* outputs);

private:
    QPointF inputConnectorPoint;
    QPointF outputConnectorPoint;
    InputConnector* inputConnector = nullptr;
    OutputConnector* outputConnector = nullptr;
    QGraphicsProxyWidget* connectorLineProxy;
    QTimer* getOtherConnectorTimer;

    bool isDone = false;

    void connectLineToConnector(ConnectorBase* connector);
    void disconnectLineFromConnector(ConnectorBase* connector);

    // QWidget interface
protected:
    void paintEvent(QPaintEvent* event);

private slots:
    void isDragged();
    void isNoMoreDragged();
    void connectorHasMoved(int connectorType);
    void connectTo(ConnectorBase* connector);
    void resetConnectorLine(ConnectorBase* connector);
    void getOhterConnectorTimeOut();
    void deleteConnectorLine();
    void transferOutputs(QStringList* outputs);
};

#endif // CONNECTORLINE_H
