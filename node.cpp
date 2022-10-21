#include "node.h"

Node::Node(QWidget* parent)
    : QWidget{parent}
{
    this->setGeometry(0, 0, 70, 200);
    this->setContextMenuPolicy(Qt::CustomContextMenu);
}

void Node::mousePressEvent(QMouseEvent* event)
{
    if (event->button() == Qt::RightButton)
    {
        emit customContextMenuRequested(event->globalPosition().toPoint());
    }
    else
    {
        QWidget::mousePressEvent(event);
    }
}
