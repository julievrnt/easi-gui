#ifndef NODEPARENTWIDGET_H
#define NODEPARENTWIDGET_H

#include <QGraphicsWidget>

class NodeParentWidget : public QGraphicsWidget
{
    Q_OBJECT
public:
    NodeParentWidget();

private slots:
    void resize(QRectF rect);
};

#endif // NODEPARENTWIDGET_H
