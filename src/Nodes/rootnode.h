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
    RootNode(QStringList* outputs = nullptr);
    ~RootNode();

    void updateOutputs();

private:
    QList<QLabel*> outputLabels;
    QGraphicsProxyWidget* outputConnector;
    void createLayout();
    void updateLayout();
    void modifyOutputs();

    // QWidget interface
protected:
    void mousePressEvent(QMouseEvent* event);

private slots:
    void modifyOutputsButtonClicked(bool clicked);
    void sortOutputs(int result);

    // NodeBase interface
protected:
    void saveNodeContent(YAML::Emitter* out);

    // NodeBase interface
public:
    void performResize();
    void setOutputConnector(QGraphicsProxyWidget* newOutputConnector);
    OutputConnector* getFirstAvailableOutputConnector();
};

#endif // ROOTNODE_H
