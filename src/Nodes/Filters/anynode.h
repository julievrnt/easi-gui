#ifndef ANYNODE_H
#define ANYNODE_H

#include "src/Nodes/nodebase.h"

class AnyNode : public NodeBase
{
    Q_OBJECT
public:
    AnyNode(QStringList* inputs = nullptr);

protected:
    void updateLayout();
    void saveNodeContent(YAML::Emitter* out);
};

#endif // ANYNODE_H