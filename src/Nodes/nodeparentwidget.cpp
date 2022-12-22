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


/// FOR DEBUG ONLY
void NodeParentWidget::paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget)
{
    // painter->setPen(QPen(Qt::blue));
    // painter->drawRect(this->geometry());
}

void NodeParentWidget::moveEvent(QGraphicsSceneMoveEvent* event)
{
    emit hasMoved();
    QGraphicsWidget::moveEvent(event);
}
