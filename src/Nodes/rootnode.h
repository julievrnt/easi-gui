#ifndef ROOTNODE_H
#define ROOTNODE_H

#include "nodebase.h"
#include <QStringListModel>>

class RootNode : public NodeBase
{
    Q_OBJECT
public:
    RootNode();

private:
    QStringListModel* outputsModel;
    void createLayout();

    // QWidget interface
protected:
    void mousePressEvent(QMouseEvent *event);
};

#endif // ROOTNODE_H
