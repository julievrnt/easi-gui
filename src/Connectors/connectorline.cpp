#include "connectorline.h"
#include <QPainter>

ConnectorLine::ConnectorLine(ConnectorBase* connector,  EasiGraphicsView* easiGraphicsView)
{
    this->easiGraphicsView = easiGraphicsView;
    getOtherConnectorTimer = new QTimer();

    inputConnectorPoint = connector->getCenterPos();
    outputConnectorPoint = connector->getCenterPos();
    connectLineToConnector(connector);

    setAttribute(Qt::WA_TranslucentBackground);
    setObjectName("Line");
}

ConnectorLine::ConnectorLine(OutputConnector* outputConnector, InputConnector* inputConnector,  EasiGraphicsView* easiGraphicsView)
{
    this->easiGraphicsView = easiGraphicsView;
    getOtherConnectorTimer = new QTimer();
    connectLineToConnector(outputConnector);
    connectLineToConnector(inputConnector);
    setAttribute(Qt::WA_TranslucentBackground);
    isDone = true;
    setObjectName("Line");
}

/**
 * @brief Saves the proxy of the connector line
 * @param newConnectorLineProxy
 */
void ConnectorLine::setConnectorLineProxy(QGraphicsProxyWidget* newConnectorLineProxy)
{
    connectorLineProxy = newConnectorLineProxy;
}

/**
 * @brief Returns the position of the end of the line that is not connected yet
 * @return
 */
QPointF ConnectorLine::getPositionToCheck()
{
    if (inputConnector == nullptr)
        return inputConnectorPoint;
    else
        return outputConnectorPoint;
}

/**
 * @brief Connect a line to the given connector
 * @param connector
 */
void ConnectorLine::connectLineToConnector(ConnectorBase* connector)
{
    if (connector->getTypeOfConnector() == INPUTCONNECTOR)
    {
        // don't connect input to input
        if (inputConnector != nullptr)
            return;

        if (outputConnector != nullptr)
        {
            // don't connect two connectors from the same node
            if (outputConnector->getNodeParentWidget() == connector->getNodeParentWidget())
                return;

            // turn none input connector into eval input connector if output connector is eval
            if (connector->getSubtypeOfConnector() == NONE && outputConnector->getSubtypeOfConnector() == EVAL)
                connector->setSubtypeOfConnector(EVAL);

            // don't connect two connectors with different subtypes
            if (connector->getSubtypeOfConnector() != outputConnector->getSubtypeOfConnector())
                return;
        }

        inputConnector = static_cast<InputConnector*>(connector);
        inputConnectorPoint = inputConnector->getCenterPos();
        connect(this, SIGNAL(transferOutputsRequested(QSharedPointer<QStringList>)), inputConnector, SLOT(transferOutputs(QSharedPointer<QStringList>)));
        connect(this, SIGNAL(saveRequested(YAML::Emitter*)), inputConnector, SLOT(save(YAML::Emitter*)));
    }
    else if (connector->getTypeOfConnector() == OUTPUTCONNECTOR)
    {
        // don't connect output to output
        if (outputConnector != nullptr)
            return;

        if (inputConnector != nullptr)
        {
            // don't connect two connectors from the same node
            if (inputConnector->getNodeParentWidget() == connector->getNodeParentWidget())
                return;

            // don't connect two connectors with different subtypes
            if (connector->getSubtypeOfConnector() != inputConnector->getSubtypeOfConnector())
                return;
        }

        outputConnector = static_cast<OutputConnector*>(connector);
        outputConnectorPoint = outputConnector->getCenterPos();
        connect(outputConnector, SIGNAL(transferOutputsRequested(QSharedPointer<QStringList>)), this, SLOT(transferOutputs(QSharedPointer<QStringList>)));
        connect(outputConnector, SIGNAL(saveRequested(YAML::Emitter*)), this, SLOT(save(YAML::Emitter*)));
    }
    else
        qDebug() << "error: neither input nor output";

    // remove connection with input node if already exists
    if (connector->getConnectorLineConnected())
        connector->deleteLine();

    connector->setConnectorLineCreated(true);
    this->repaint();

    connect(connector, SIGNAL(lineDragged()), this, SLOT(isDragged()));
    connect(connector, SIGNAL(lineNoMoreDragged()), this, SLOT(isNoMoreDragged()));
    connect(connector, SIGNAL(moveConnectorLineRequested(int)), this, SLOT(connectorHasMoved(int)));
    connect(connector, SIGNAL(resetConnectorLineRequested(ConnectorBase*)), this, SLOT(resetConnectorLine(ConnectorBase*)));
    connect(connector, SIGNAL(deleteConnectorLineRequested()), this, SLOT(deleteConnectorLine()));

    if (getOtherConnectorTimer->isActive())
        getOtherConnectorTimer->stop();

    if (inputConnector != nullptr && outputConnector != nullptr)
    {
        inputConnector->setConnectorLineConnected(true);
        outputConnector->setConnectorLineConnected(true);
        emit outputConnector->transferOutputsRequested(outputConnector->getOutputs());
        emit connectorLineConnected();
        isDone = true;
        this->repaint();
    }
}

/**
 * @brief Disconnect the line from a connector
 * @param connector
 */
void ConnectorLine::disconnectLineFromConnector(ConnectorBase* connector)
{
    connector->setConnectorLineConnected(false);
    connector->setConnectorLineCreated(false);

    disconnect(connector, SIGNAL(lineDragged()), this, SLOT(isDragged()));
    disconnect(connector, SIGNAL(lineNoMoreDragged()), this, SLOT(isNoMoreDragged()));
    disconnect(connector, SIGNAL(moveConnectorLineRequested(int)), this, SLOT(connectorHasMoved(int)));
    disconnect(connector, SIGNAL(resetConnectorLineRequested(ConnectorBase*)), this, SLOT(resetConnectorLine(ConnectorBase*)));
    disconnect(connector, SIGNAL(deleteConnectorLineRequested()), this, SLOT(deleteConnectorLine()));

    if (connector->getTypeOfConnector() == INPUTCONNECTOR)
    {
        disconnect(this, SIGNAL(transferOutputsRequested(QSharedPointer<QStringList>)), inputConnector, SLOT(transferOutputs(QSharedPointer<QStringList>)));
        if (connector->getSubtypeOfConnector() == EVAL)
            connector->setSubtypeOfConnector(NONE);
    }
    else if (connector->getTypeOfConnector() == OUTPUTCONNECTOR)
        disconnect(outputConnector, SIGNAL(transferOutputsRequested(QSharedPointer<QStringList>)), this, SLOT(transferOutputs(QSharedPointer<QStringList>)));
    else
        qDebug() << "error: neither input nor output";
}


/// ===========================================================================
/// ========================= INHERITED FROM QWIDGET ==========================
/// ===========================================================================

/**
 * @brief Paints a line from the input connector position to the output connector position. The color of the line corresponds to the color of the connectors.
 * @param event
 */
void ConnectorLine::paintEvent(QPaintEvent* event)
{
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);
    QPen linePen;
    if (!isDone)
        linePen.setColor(Qt::yellow);
    else
    {
        switch (inputConnector->getSubtypeOfConnector())
        {
            case SPECIALCOMPONENT:
                linePen.setColor(Qt::blue);
                break;
            case MATH :
                linePen.setColor(Qt::darkMagenta);
                break;
            case EVAL :
                linePen.setColor(Qt::red);
                break;
            default :
                linePen.setColor(Qt::green);
                break;
        }
    }
    linePen.setWidth(1);
    painter.setPen(linePen);
    painter.drawLine(inputConnectorPoint, outputConnectorPoint);

    QWidget::paintEvent(event);
}


/// ===========================================================================
/// ================================== SLOTS ==================================
/// ===========================================================================

/**
 * @brief Draws a line from a connector to the mouse
 */
void ConnectorLine::isDragged()
{
    if (isDone == true)
        isDone = false;
    if (inputConnector == nullptr)
        inputConnectorPoint = easiGraphicsView->mapToScene(easiGraphicsView->mapFromGlobal(QCursor::pos()));
    else
        outputConnectorPoint = easiGraphicsView->mapToScene(easiGraphicsView->mapFromGlobal(QCursor::pos()));
    this->repaint();
}

/**
 * @brief Emits a signal received by WidgetsHandler to inform it that the line is no more dragged and starts a timer. When the timer has timed out, the line is deleted.
 */
void ConnectorLine::isNoMoreDragged()
{
    connect(getOtherConnectorTimer, SIGNAL(timeout()), this, SLOT(getOhterConnectorTimeOut()));
    getOtherConnectorTimer->start(100);
    emit drawnIsDone(this);
}

/**
 * @brief Updates the position of the line when the connector has moved
 * @param connectorType indicates which end of the line has moved
 */
void ConnectorLine::connectorHasMoved(int connectorType)
{
    if (connectorType == INPUTCONNECTOR)
        inputConnectorPoint = inputConnector->getCenterPos();
    else if (connectorType == OUTPUTCONNECTOR)
        outputConnectorPoint = outputConnector->getCenterPos();
    else
        qDebug() << "error: neither input nor output";
    this->repaint();
}

/**
 * @brief Calles connectLineToConnector(ConnectorBase* connector) when received
 * @param connector
 */
void ConnectorLine::connectTo(ConnectorBase* connector)
{
    connectLineToConnector(connector);
}

/**
 * @brief When received from a connector, disconnects the line from the other connector
 * @param connector
 */
void ConnectorLine::resetConnectorLine(ConnectorBase* connector)
{
    if (connector == inputConnector)
    {
        inputConnector->setConnectorLineConnected(false);
        disconnectLineFromConnector(outputConnector);
        outputConnector = nullptr;
        outputConnectorPoint = inputConnectorPoint;
    }
    else if (connector == outputConnector)
    {
        outputConnector->setConnectorLineConnected(false);
        disconnectLineFromConnector(inputConnector);
        inputConnector = nullptr;
        inputConnectorPoint = outputConnectorPoint;
    }
    else
        qDebug() << "error: neither input nor output";
}

/**
 * @brief Calls deleteConnectorLine() when the timer has timed out
 */
void ConnectorLine::getOhterConnectorTimeOut()
{
    getOtherConnectorTimer->stop();
    deleteConnectorLine();
}

/**
 * @brief Disconnects the line from any connector it is connected to and deletes itself
 */
void ConnectorLine::deleteConnectorLine()
{
    delete getOtherConnectorTimer;
    if (inputConnector != nullptr)
        disconnectLineFromConnector(inputConnector);
    if (outputConnector != nullptr)
        disconnectLineFromConnector(outputConnector);
    emit deleteConnectorLineRequested(connectorLineProxy);
}

void ConnectorLine::resize(QRectF newRect)
{
    setGeometry(newRect.toRect());
}

void ConnectorLine::transferOutputs(QSharedPointer<QStringList> outputs)
{
    emit transferOutputsRequested(outputs);
}

/**
 * @brief Emits a signal received by the input connector to save the content of its node.
 * @param out YAML emitter
 */
void ConnectorLine::save(YAML::Emitter* out)
{
    emit saveRequested(out);
}
