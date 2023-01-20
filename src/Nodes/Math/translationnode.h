#ifndef TRANSLATIONNODE_H
#define TRANSLATIONNODE_H

#include "src/Nodes/nodebase.h"

class TranslationNode : public NodeBase
{
    Q_OBJECT
public:
    TranslationNode(QStringList* outputs = nullptr);
    TranslationNode(double value);
    ~TranslationNode();

private:
    void createLayout();
};

#endif // TRANSLATIONNODE_H
