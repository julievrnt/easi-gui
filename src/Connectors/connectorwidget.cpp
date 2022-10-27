#include "connectorwidget.h"
#include <QPainter>

ConnectorWidget::ConnectorWidget(QStringList* outputs, QWidget *parent)
    : QWidget{parent}
{
    this->outputs = outputs;
    setGeometry(0,0,15,15);
}

QStringList* ConnectorWidget::getOutputs() const
{
    return outputs;
}

void ConnectorWidget::paintEvent(QPaintEvent *event)
{
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);
    painter.setBrush(Qt::green);
    painter.setPen(Qt::black);
    painter.drawEllipse(50,1,15,15);

    QWidget::paintEvent(event);
}
