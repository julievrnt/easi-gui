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
};

#endif // NODEPARENTWIDGET_H
