#ifndef CONNECTORLINE_H
#define CONNECTORLINE_H

#include <QWidget>
#include "inputs/inputconnector.h"
#include "outputs/outputconnector.h"
#include "yaml-cpp/yaml.h"
#include <QTimer>

class ConnectorLine : public QWidget
{
    Q_OBJECT
public:
    explicit ConnectorLine(ConnectorBase* connector);
    explicit ConnectorLine(OutputConnector* outputConnector, InputConnector* inputConnector);

    void setConnectorLineProxy(QGraphicsProxyWidget* newConnectorLineProxy);
    QPointF getPositionToCheck();

signals:
    void drawnIsDone(ConnectorLine* connectorLine);
    void connectorLineConnected();
    void deleteConnectorLine(QGraphicsProxyWidget* connectorLineProxy);
    void transferOutputsRequested(QStringList* outputs);
    void saveRequested(YAML::Emitter* out);

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
    void resize(QRectF newRect);
    void transferOutputs(QStringList* outputs);
    void save(YAML::Emitter* out);
};

#endif // CONNECTORLINE_H
