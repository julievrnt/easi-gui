#include "layeredmodelnode.h"

LayeredModelNode::LayeredModelNode()
{
    typeOfNode = LAYEREDMODELNODE;
    localTag = "LayeredModel";
    this->setWindowTitle("Layered Model");
    createLayout();

    setGeometry(QRect(0, 0, sizeHint().width(), sizeHint().height()));
}
