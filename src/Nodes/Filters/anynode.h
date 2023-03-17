#ifndef ANYNODE_H
#define ANYNODE_H

#include "src/Nodes/nodebase.h"

class AnyNode : public NodeBase
{
    Q_OBJECT
public:
    AnyNode(QSharedPointer<QStringList> inputs = nullptr);

protected:
    void updateLayout();
};

#endif // ANYNODE_H
