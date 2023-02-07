#include "identitymapnode.h"

IdentityMapNode::IdentityMapNode(QStringList* inputs): NodeBase(inputs, inputs)
{
    typeOfNode = IDENTITYMAPNODE;
    localTag = "IdentityMap";
    this->setWindowTitle("Identity Map");
    createLayout(false, false);

    setGeometry(QRect(0, 0, sizeHint().width(), sizeHint().height()));
}

void IdentityMapNode::updateLayout()
{
    outputs = inputs;
    emit transferOutputsRequested(outputs);
}
