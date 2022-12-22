#ifndef NODEPARENTWIDGET_H
#define NODEPARENTWIDGET_H

#include <QGraphicsWidget>
#include <QGraphicsSceneMoveEvent>

class NodeParentWidget : public QGraphicsWidget
{
    Q_OBJECT
public:
    NodeParentWidget();

    void move(int top, int left);

signals:
    void hasMoved();

private slots:
    void resize(QRectF rect);

    // QGraphicsItem interface
public:
    void paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget);

    // QGraphicsWidget interface
protected:
    void moveEvent(QGraphicsSceneMoveEvent* event);
};

#endif // NODEPARENTWIDGET_H
