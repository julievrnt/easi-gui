#include "connectorline.h"
#include <QPainter>

ConnectorLine::ConnectorLine(ConnectorBase* connector)
{
    getOtherConnectorTimer = new QTimer();

    inputConnectorPoint = connector->getCenterPos();
    outputConnectorPoint = connector->getCenterPos();
    connectLineToConnector(connector);

    setAttribute(Qt::WA_TranslucentBackground);
}

ConnectorLine::ConnectorLine(OutputConnector* outputConnector, InputConnector* inputConnector)
{
    getOtherConnectorTimer = new QTimer();
    connectLineToConnector(outputConnector);
    connectLineToConnector(inputConnector);
    setAttribute(Qt::WA_TranslucentBackground);
    isDone = true;
}

void ConnectorLine::setConnectorLineProxy(QGraphicsProxyWidget* newConnectorLineProxy)
{
    connectorLineProxy = newConnectorLineProxy;
}

QPointF ConnectorLine::getPositionToCheck()
{
    if (inputConnector == nullptr)
        return inputConnectorPoint;
    else
        return outputConnectorPoint;
}

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

        inputConnector = (InputConnector*) connector;
        inputConnectorPoint = inputConnector->getCenterPos();
        connect(this, SIGNAL(transferOutputsRequested(QStringList*)), inputConnector, SLOT(transferOutputs(QStringList*)));
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

        outputConnector = (OutputConnector*) connector;
        outputConnectorPoint = outputConnector->getCenterPos();
        connect(outputConnector, SIGNAL(transferOutputsRequested(QStringList*)), this, SLOT(transferOutputs(QStringList*)));
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
        disconnect(this, SIGNAL(transferOutputsRequested(QStringList*)), inputConnector, SLOT(transferOutputs(QStringList*)));
        if (connector->getSubtypeOfConnector() == EVAL)
            connector->setSubtypeOfConnector(NONE);
    }
    else if (connector->getTypeOfConnector() == OUTPUTCONNECTOR)
        disconnect(outputConnector, SIGNAL(transferOutputsRequested(QStringList*)), this, SLOT(transferOutputs(QStringList*)));
    else
        qDebug() << "error: neither input nor output";
}

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

void ConnectorLine::isDragged()
{
    if (isDone == true)
        isDone = false;
    if (inputConnector == nullptr)
        inputConnectorPoint = mapFromGlobal(QCursor::pos());
    else
        outputConnectorPoint = mapFromGlobal(QCursor::pos());
    this->repaint();
}

void ConnectorLine::isNoMoreDragged()
{
    connect(getOtherConnectorTimer, SIGNAL(timeout()), this, SLOT(getOhterConnectorTimeOut()));
    getOtherConnectorTimer->start(100);
    emit drawnIsDone(this);
}

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

void ConnectorLine::connectTo(ConnectorBase* connector)
{
    connectLineToConnector(connector);
}

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

void ConnectorLine::getOhterConnectorTimeOut()
{
    getOtherConnectorTimer->stop();
    deleteConnectorLine();
}

void ConnectorLine::deleteConnectorLine()
{
    delete getOtherConnectorTimer;
    if (inputConnector != nullptr)
        disconnectLineFromConnector(inputConnector);
    if (outputConnector != nullptr)
        disconnectLineFromConnector(outputConnector);
    emit deleteConnectorLine(connectorLineProxy);
}

void ConnectorLine::transferOutputs(QStringList* outputs)
{
    emit transferOutputsRequested(outputs);
}

void ConnectorLine::save(YAML::Emitter* out)
{
    emit saveRequested(out);
}
