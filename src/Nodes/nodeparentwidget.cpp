#include "nodeparentwidget.h"
#include <QPainter>

NodeParentWidget::NodeParentWidget()
{
    setObjectName("papa");
}

void NodeParentWidget::resize(QRectF rect)
{
    this->setGeometry(QRect(pos().x(), pos().y(), rect.width(), rect.height()));
    //qDebug() << "parent geo: " << geometry();
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
