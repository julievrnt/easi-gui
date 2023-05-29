#include "connectorbase.h"
#include <QPainter>
#include <QPaintEvent>

ConnectorBase::ConnectorBase(NodeParentWidget* nodeParentWidget, int subtype, bool canDisconnect)
{
    this->typeOfConnector = CONNECTOR;
    this->subtypeOfConnector = subtype;
    this->nodeParentWidget = nodeParentWidget;
    this->canDisconnect = canDisconnect;
    this->outputs = nullptr;
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


/// ===========================================================================
/// =========================== GETTERS AND SETTERS ===========================
/// ===========================================================================

/**
 * @brief Returns the list of the names of the outputs
 * @return
 */
QSharedPointer<QStringList> ConnectorBase::getOutputs() const
{
    return outputs;
}

/**
 * @brief Returns either INPUTCONNECTOR or OUTPUTCONNECTOR
 * @return
 */
int ConnectorBase::getTypeOfConnector() const
{
    return typeOfConnector;
}

/**
 * @brief Returns either NONE (default one), SPECIALCOMPONENT, MATH or EVAL
 * @return
 */
int ConnectorBase::getSubtypeOfConnector() const
{
    return subtypeOfConnector;
}

/**
 * @brief Some connectors can change types (e.g. NONE INPUTCONNECTOR can become EVAL INPUTCONNECTOR)
 * @param newSubtypeOfConnector
 */
void ConnectorBase::setSubtypeOfConnector(int newSubtypeOfConnector)
{
    subtypeOfConnector = newSubtypeOfConnector;
}

/**
 * @brief Returns the position of the center of the connector
 * @return
 */
QPointF ConnectorBase::getCenterPos() const
{
    return centerPos;
}

/**
 * @brief Returns the parent widget of the connector
 * @return
 */
NodeParentWidget* ConnectorBase::getNodeParentWidget() const
{
    return nodeParentWidget;
}

/**
 * @brief Returns true if the connector is connected to another connector via a line.
 * @return
 */
bool ConnectorBase::getConnectorLineConnected() const
{
    return connectorLineConnected;
}

/**
 * @brief Updates the attribute connectorLineConnected. connectorLineConnector is true if there is a connector line connected to this connector and another one.
 * @return
 */
void ConnectorBase::setConnectorLineConnected(bool isConnected)
{
    connectorLineConnected = isConnected;
}

/**
 * @brief Updates the attribute connectorLineCreated. connectorLineCreated is true if there is a connector line connected to this connector.
 * @return
 */
void ConnectorBase::setConnectorLineCreated(bool newConnectorLineCreated)
{
    connectorLineCreated = newConnectorLineCreated;
}

/**
 * @brief Emits a signal received by WidgetsHandler to remove and delete the connector line from the scene.
 */
void ConnectorBase::deleteLine()
{
    if (connectorLineConnected)
        emit deleteConnectorLineRequested();
}


/// ===========================================================================
/// ========================= INHERITED FROM QWIDGET ==========================
/// ===========================================================================

/**
 * @brief Paints the connector as a 15x15 circle. Blue stands for SPECIALCOMPONENT, Violet for MATH, Red for EVAL and Green for NONE connectors.
 * @param event
 */
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
            case SPECIALCOMPONENT :
                painter.setBrush(Qt::blue);
                break;
            case MATH :
                painter.setBrush(Qt::darkMagenta);
                break;
            case EVAL:
                painter.setBrush(Qt::red);
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

/**
 * @brief Changes the color of the connector to yellow when the mouse hovers the connector
 * @param event
 */
void ConnectorBase::enterEvent(QEnterEvent* event)
{
    QWidget::enterEvent(event);
    highlight = true;
    this->repaint();
    emit isConnectorLineOnMe(this);
}

/**
 * @brief Changes the color of the connector back to its default color when the mouse leaves the connector
 * @param event
 */
void ConnectorBase::leaveEvent(QEvent* event)
{
    QWidget::leaveEvent(event);
    highlight = false;
    this->repaint();
}

void ConnectorBase::mousePressEvent(QMouseEvent* event)
{
    Q_UNUSED(event);
    // does nothing.
}

/**
 * @brief Emits a signal to stop dragging a connector line if the connector can be disconnected from another one.
 * @param event
 */
void ConnectorBase::mouseReleaseEvent(QMouseEvent* event)
{
    Q_UNUSED(event);
    if (!canDisconnect)
        return;
    emit lineNoMoreDragged();
}

/**
 * @brief Returns if the connector can't be disconnected from another one. Otherwise, creates a line from the center of the connector to the mouse until the mouse is no more pressed.
 * @param event
 */
void ConnectorBase::mouseMoveEvent(QMouseEvent* event)
{
    QWidget::mouseMoveEvent(event);

    if (!canDisconnect)
        return;

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
}

/**
 * @brief Emits a signal to inform the connector line that the connector's position has changed.
 * @param event
 */
void ConnectorBase::moveEvent(QMoveEvent* event)
{
    updateCenterPos();
    QWidget::moveEvent(event);
    emit moveConnectorLineRequested(getTypeOfConnector());
}


/// ===========================================================================
/// ============================ UPDATE FUNCTIONS =============================
/// ===========================================================================

/**
 * @brief Updates the scene coordinates of the position of the center of the connector
 */
void ConnectorBase::updateCenterPos()
{
    centerPos = nodeParentWidget->pos() + pos() + QPointF(geometry().width() / 2, geometry().height() / 2);
}

/**
 * @brief Emits a signal to inform that the outputs have changed.
 */
void ConnectorBase::outputsChanged()
{
    emit transferOutputsRequested(outputs);
}

/**
 * @brief Resets the attributes connectorLineCreated and connectorLineConnected to false when the line is deleted
 */
void ConnectorBase::connectorLineDeleted()
{
    connectorLineCreated = false;
    connectorLineConnected = false;
}


/// ===========================================================================
/// ================================== SLOTS ==================================
/// ===========================================================================

/**
 * @brief Calls updateCenterPos() to updates the position of the center of the connector and emits a signal received by the connector line (if it exists) to inform it that it moved.
 */
void ConnectorBase::nodeParentWidgetHasMoved()
{
    updateCenterPos();
    emit moveConnectorLineRequested(getTypeOfConnector());
}

/**
 * @brief Updates the attribute outputs if the pointer has changed and calls outputsChanged()
 * @param outputs
 */
void ConnectorBase::transferOutputs(QSharedPointer<QStringList> outputs)
{
    if (this->outputs != outputs)
        this->outputs = outputs;
    outputsChanged();
}

