#include "connectorbase.h"
#include "../helpers.h"
#include <QPainter>
#include <QPaintEvent>

ConnectorBase::ConnectorBase(NodeParentWidget* nodeParentWidget, QWidget* parent)
    : QWidget{parent}
{
    this->typeOfConnector = CONNECTOR;
    this->subtypeOfConnector = NONE;
    this->outputs = nullptr;
    this->nodeParentWidget = nodeParentWidget;
    highlight = false;
    connectorLineCreated = false;
    connectorLineConnected = false;
    setGeometry(0, 0, 15, 15);
    setMinimumSize(QSize(15, 15));
    setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    setAttribute(Qt::WA_TranslucentBackground);
    setObjectName("Connector");
    updateCenterPos();
}

ConnectorBase::~ConnectorBase()
{
    //delete outputs;
}

QStringList* ConnectorBase::getOutputs() const
{
    return outputs;
}

int ConnectorBase::getTypeOfConnector() const
{
    return typeOfConnector;
}

QPointF ConnectorBase::getCenterPos() const
{
    return centerPos;
}

void ConnectorBase::connectorLineDeleted()
{
    connectorLineCreated = false;
    connectorLineConnected = false;
}

void ConnectorBase::setConnectorLineConnected(bool isConnected)
{
    connectorLineConnected = isConnected;
}

void ConnectorBase::deleteLine()
{
    if (connectorLineConnected)
        emit deleteConnectorLineRequested();
}

NodeParentWidget* ConnectorBase::getNodeParentWidget() const
{
    return nodeParentWidget;
}

void ConnectorBase::setConnectorLineCreated(bool newConnectorLineCreated)
{
    connectorLineCreated = newConnectorLineCreated;
}

bool ConnectorBase::getConnectorLineConnected() const
{
    return connectorLineConnected;
}

void ConnectorBase::updateCenterPos()
{
    centerPos = nodeParentWidget->pos() + pos() + QPointF(geometry().width() / 2, geometry().height() / 2);
    // qDebug() << this << " " << centerPos;
}

void ConnectorBase::nodeParentWidgetHasMoved()
{
    updateCenterPos();
    emit moveConnectorLineRequested(getTypeOfConnector());
}

void ConnectorBase::transferOutputs(QStringList* outputs)
{
    if (this->outputs == nullptr)
        this->outputs = outputs;
    outputsChanged();
}

int ConnectorBase::getSubtypeOfConnector() const
{
    return subtypeOfConnector;
}

void ConnectorBase::outputsChanged()
{
    emit transferOutputsRequested(outputs);
}

void ConnectorBase::paintEvent(QPaintEvent* event)
{
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);
    if (highlight)
        painter.setBrush(Qt::yellow);
    else
    {
        switch (subtypeOfConnector)
        {
            case NONE :
                painter.setBrush(Qt::green);
                break;
            case MATH :
                painter.setBrush(Qt::darkMagenta);
                break;
            default :
                painter.setBrush(Qt::green);
                break;
        }
    }
    painter.setPen(Qt::black);
    painter.drawEllipse(0, 0, 15, 15);

    QWidget::paintEvent(event);
}

void ConnectorBase::enterEvent(QEnterEvent* event)
{
    QWidget::enterEvent(event);
    highlight = true;
    this->repaint();
    emit isConnectorLineOnMe(this);
}

void ConnectorBase::leaveEvent(QEvent* event)
{
    QWidget::leaveEvent(event);
    highlight = false;
    this->repaint();
}

void ConnectorBase::mousePressEvent(QMouseEvent* event)
{
    UNUSED(event);
    // QWidget::mousePressEvent(event);
}

void ConnectorBase::mouseReleaseEvent(QMouseEvent* event)
{
    UNUSED(event);
    emit lineNoMoreDragged();
    // QWidget::mousePressEvent(event);
}

void ConnectorBase::mouseMoveEvent(QMouseEvent* event)
{
    if (connectorLineConnected)
        emit resetConnectorLineRequested(this);

    if (connectorLineCreated)
    {
        emit lineDragged();
    }
    else
    {
        emit createConnectorLineResquested(this);
        connectorLineCreated = true;
    }
    QWidget::mouseMoveEvent(event);
}

void ConnectorBase::moveEvent(QMoveEvent* event)
{
    updateCenterPos();
    QWidget::moveEvent(event);
    emit moveConnectorLineRequested(getTypeOfConnector());
}
