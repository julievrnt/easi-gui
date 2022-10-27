#include "nodeparentwidget.h"

NodeParentWidget::NodeParentWidget()
{

}

void NodeParentWidget::resize(QRectF rect)
{
    this->setGeometry(QRect(pos().x(), pos().y(), rect.width(), rect.height()));
}
