#ifndef ROOTNODE_H
#define ROOTNODE_H

#include "nodebase.h"
#include <QStringListModel>
#include <QList>
#include <QLabel>

class RootNode : public NodeBase
{
    Q_OBJECT
public:
    RootNode();
    ~RootNode();

private:
    QList<QLabel*> outputLabels;
    void createLayout();
    void updateLayout();
    void modifyOutputs();

    // QWidget interface
protected:
    void mousePressEvent(QMouseEvent* event);

private slots:
    void modifyOutputsButtonClicked(bool clicked);
    void sortOutputs(int result);
};

#endif // ROOTNODE_H
