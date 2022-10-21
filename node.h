#ifndef NODE_H
#define NODE_H

#include <QWidget>
#include "qevent.h"

#define NODE 0
#define ROOT 1
#define CONSTANTMAPNODE 2

class Node : public QWidget
{
    Q_OBJECT
public:
    explicit Node(QWidget* parent = nullptr);
     int getTypeOfNode();

signals:
    void resized(QRectF rect);

    // QWidget interface
protected:
    void mousePressEvent(QMouseEvent* event);

private:
    const int type = NODE;
};

#endif // NODE_H
