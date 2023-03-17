#include "nodeparentwidget.h"
#include <QPainter>

NodeParentWidget::NodeParentWidget()
{
    setObjectName("papa");
}

void NodeParentWidget::move(int x, int y)
{
    this->setPos(x, y);
    emit hasMoved();
}

void NodeParentWidget::resize(QRectF rect)
{
    this->setGeometry(pos().x(), pos().y(), rect.width(), rect.height());
}

void NodeParentWidget::moveEvent(QGraphicsSceneMoveEvent* event)
{
    emit hasMoved();
    QGraphicsWidget::moveEvent(event);
}
