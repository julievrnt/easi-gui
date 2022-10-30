#ifndef NODEPARENTWIDGET_H
#define NODEPARENTWIDGET_H

#include <QGraphicsWidget>
#include <QGraphicsSceneMoveEvent>

class NodeParentWidget : public QGraphicsWidget
{
    Q_OBJECT
public:
    NodeParentWidget();

signals:

private slots:
    void resize(QRectF rect);

    // QGraphicsItem interface
public:
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);
};

#endif // NODEPARENTWIDGET_H
