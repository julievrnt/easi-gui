#include "layeredmodelnode.h"

LayeredModelNode::LayeredModelNode()
{
    typeOfNode = LAYEREDMODELNODE;
    localTag = "LayeredModel";
    this->setWindowTitle("Layered Model");
    createLayout();

    setGeometry(QRect(0, 0, sizeHint().width(), sizeHint().height()));
}

void LayeredModelNode::performResize()
{
    
}

void LayeredModelNode::createLayout()
{
    NodeBase::createLayout();
}

void LayeredModelNode::addModelLayout(QVBoxLayout *globalLayout)
{
    
}

void LayeredModelNode::addParametersLayout(QVBoxLayout *globalLayout)
{
    
}

void LayeredModelNode::addNewDimensionsLayoutRow(QVBoxLayout *dimensionsLayout, int index)
{
    
}

void LayeredModelNode::updateLayout()
{
    
}

void LayeredModelNode::saveValues(YAML::Emitter* out)
{
    Q_UNUSED(out);
}
