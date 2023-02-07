#ifndef LAYEREDMODELNODE_H
#define LAYEREDMODELNODE_H

#include "src/Nodes/nodebase.h"

class LayeredModelNode : public NodeBase
{
    Q_OBJECT
public:
    LayeredModelNode();

private:
    void createLayout();

    // NodeBase interface
protected:
    void saveValues(YAML::Emitter* out);
};

#endif // LAYEREDMODELNODE_H
