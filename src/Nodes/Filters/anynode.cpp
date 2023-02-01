#include "anynode.h"

AnyNode::AnyNode(QStringList* inputs) : NodeBase(inputs, inputs)
{
    typeOfNode = ANYNODE;
    this->setWindowTitle("Any");
    createLayout();

    setGeometry(QRect(0, 0, sizeHint().width(), sizeHint().height()));
}

void AnyNode::updateLayout()
{
    outputs = inputs;
    emit transferOutputsRequested(outputs);
}

void AnyNode::saveNodeContent(YAML::Emitter* out)
{
    (*out) << YAML::LocalTag("Any");
    *out << YAML::BeginMap;
    saveComponents(out);
    *out << YAML::EndMap;
}
