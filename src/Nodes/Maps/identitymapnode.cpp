#include "identitymapnode.h"

IdentityMapNode::IdentityMapNode(QStringList* inputs): NodeBase(inputs, inputs)
{
    typeOfNode = IDENTITYMAPNODE;
    this->setWindowTitle("Identity Map");
    createLayout();

    setGeometry(QRect(0, 0, sizeHint().width(), sizeHint().height()));
}

void IdentityMapNode::updateLayout()
{
    outputs = inputs;
    emit transferOutputsRequested(outputs);
}

void IdentityMapNode::saveNodeContent(YAML::Emitter* out)
{
    (*out) << YAML::LocalTag("IdentityMap");
    *out << YAML::BeginMap;
    saveComponents(out);
    *out << YAML::EndMap;
}
