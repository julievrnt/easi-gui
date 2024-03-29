#ifndef CONNECTORLINE_H
#define CONNECTORLINE_H

#include <QWidget>
#include "inputconnector.h"
#include "outputconnector.h"
#include "src/easigraphicsview.h"
#include "yaml-cpp/yaml.h"
#include <QTimer>

class ConnectorLine : public QWidget
{
    Q_OBJECT
public:
    explicit ConnectorLine(ConnectorBase* connector, EasiGraphicsView* easiGraphicsView);
    explicit ConnectorLine(OutputConnector* outputConnector, InputConnector* inputConnector, EasiGraphicsView* easiGraphicsView);

    void setConnectorLineProxy(QGraphicsProxyWidget* newConnectorLineProxy);
    QPointF getPositionToCheck();

signals:
    void drawnIsDone(ConnectorLine* connectorLine);
    void connectorLineConnected();
    void deleteConnectorLineRequested(QGraphicsProxyWidget* connectorLineProxy);
    void transferOutputsRequested(QSharedPointer<QStringList> outputs);
    void saveRequested(YAML::Emitter* out);

private:
    // this attribute is used for the mapping when zooming in or out
    EasiGraphicsView* easiGraphicsView;
    QPointF inputConnectorPoint;
    QPointF outputConnectorPoint;
    InputConnector* inputConnector = nullptr;
    OutputConnector* outputConnector = nullptr;
    QGraphicsProxyWidget* connectorLineProxy;
    QTimer* getOtherConnectorTimer;

    bool isDone = false;

    void connectLineToConnector(ConnectorBase* connector);
    void disconnectLineFromConnector(ConnectorBase* connector);

    // inherited from qwidget
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
    void transferOutputs(QSharedPointer<QStringList> outputs);
    void save(YAML::Emitter* out);
};

#endif // CONNECTORLINE_H
