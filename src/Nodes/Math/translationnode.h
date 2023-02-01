#ifndef TRANSLATIONNODE_H
#define TRANSLATIONNODE_H

#include "src/Nodes/nodebase.h"

class TranslationNode : public NodeBase
{
    Q_OBJECT
public:
    TranslationNode();
    ~TranslationNode();

    void setValue(double value);

private:
    void createLayout();

    // NodeBase interface
protected:
    void saveNodeContent(YAML::Emitter* out);
    void saveValues(YAML::Emitter* out);
};

#endif // TRANSLATIONNODE_H
