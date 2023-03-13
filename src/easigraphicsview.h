#ifndef EASIGRAPHICSVIEW_H
#define EASIGRAPHICSVIEW_H

#include <QGraphicsView>

class EasiGraphicsView : public QGraphicsView
{
    Q_OBJECT
public:
    EasiGraphicsView();

    // QWidget interface
protected:
    void wheelEvent(QWheelEvent* event);
};

#endif // EASIGRAPHICSVIEW_H
