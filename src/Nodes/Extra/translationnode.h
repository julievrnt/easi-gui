#ifndef TRANSLATIONNODE_H
#define TRANSLATIONNODE_H

#include "src/Nodes/nodebase.h"

class TranslationNode : public NodeBase
{
    Q_OBJECT
public:
    TranslationNode();

    void setValue(double value);

protected:
    void saveNodeContent(YAML::Emitter* out);
    void saveValues(YAML::Emitter* out);
};

#endif // TRANSLATIONNODE_H
