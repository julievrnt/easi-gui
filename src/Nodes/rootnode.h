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

    void updateOutputs();

private:
    QGraphicsProxyWidget* outputConnector;
    void createLayout();

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
