#ifndef NODE_H
#define NODE_H

#include <QWidget>
#include <QPoint>
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
    int getNumberOfComponents() const;
    QStringList *getOutputs() const;

signals:
    void resized(QRectF rect);

private:
    int numberOfComponents = 1;
    QStringList* outputs;

private slots:
    void componentRemovedOrAdded(int numberOfComponents);

    // QWidget interface
protected:
    int typeOfNode;
    virtual void performResize();
    void mousePressEvent(QMouseEvent* event);
    //void paintEvent(QPaintEvent *event);
};

#endif // NODE_H
