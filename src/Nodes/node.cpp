#include "node.h"
#include <QPainter>

Node::Node(QWidget* parent)
    : QWidget{parent}
{
    typeOfNode = NODE;
    //setAttribute(Qt::WA_TranslucentBackground);
    setGeometry(0, 0, 70, 100);
    setContextMenuPolicy(Qt::CustomContextMenu);
}

int Node::getTypeOfNode()
{
    return typeOfNode;
}

QStringList *Node::getOutputs() const
{
    return outputs;
}

int Node::getNumberOfComponents() const
{
    return numberOfComponents;
}

void Node::componentRemovedOrAdded(int numberOfComponents)
{
    this->numberOfComponents = numberOfComponents;
    performResize();
}

void Node::performResize()
{
    // do nothing
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
/*
void Node::paintEvent(QPaintEvent *event)
{
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);
    painter.setBrush(QBrush(Qt::darkGray)); // visible color of background
    painter.setPen(Qt::transparent); // thin border color

    // Change border radius
    QRect rect = this->rect();
    rect.setWidth(rect.width()-1);
    rect.setHeight(rect.height()-1);
    painter.drawRoundedRect(rect, 20, 20);

    QWidget::paintEvent(event);
}*/
