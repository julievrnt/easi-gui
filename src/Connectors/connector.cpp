#include "connector.h"
#include <QPainter>

Connector::Connector(QStringList* outputs, QWidget* parent)
    : QWidget{parent}
{
    this->outputs = outputs;
    setGeometry(0, 0, 15, 15);
    setMinimumSize(QSize(15, 15));
    setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    setAttribute(Qt::WA_TranslucentBackground);
}

Connector::~Connector()
{
    //delete outputs;
}

QStringList* Connector::getOutputs() const
{
    return outputs;
}

void Connector::paintEvent(QPaintEvent* event)
{
    QPainter painter(this);
    painter.setPen(Qt::blue);
    painter.setRenderHint(QPainter::Antialiasing);
    painter.setBrush(Qt::green);
    painter.setPen(Qt::black);
    painter.drawEllipse(0, 0, 15, 15);

    QWidget::paintEvent(event);
}
