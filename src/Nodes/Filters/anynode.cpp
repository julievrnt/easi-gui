#include "anynode.h"

AnyNode::AnyNode(QSharedPointer<QStringList> inputs) : NodeBase(inputs, inputs)
{
    typeOfNode = ANYNODE;
    localTag = "Any";
    this->setWindowTitle("Any");
    createLayout(false, false);

    setGeometry(QRect(0, 0, sizeHint().width(), sizeHint().height()));
}

void AnyNode::updateLayout()
{
    outputs = inputs;
    emit transferOutputsRequested(outputs);
}
