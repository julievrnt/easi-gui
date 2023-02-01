#ifndef IDENTITYMAPNODE_H
#define IDENTITYMAPNODE_H

#include "src/Nodes/nodebase.h"

class IdentityMapNode : public NodeBase
{
    Q_OBJECT
public:
    IdentityMapNode(QStringList* inputs = nullptr);

protected:
    void updateLayout();
};

#endif // IDENTITYMAPNODE_H
