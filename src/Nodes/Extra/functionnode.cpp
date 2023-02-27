#include "functionnode.h"

FunctionNode::FunctionNode(QStringList* inputs) : NodeBase(inputs)
{
    typeOfNode = FUNCTIONNODE;
    setWindowTitle("Function");
    createLayout();

    setGeometry(QRect(0, 0, sizeHint().width(), sizeHint().height()));
}

void FunctionNode::setValue(QStringList *inputs, QString values)
{

}

void FunctionNode::addNewDimensionsLayoutRow(QVBoxLayout *dimensionsLayout, int index)
{

}

void FunctionNode::updateLayout()
{

}

void FunctionNode::saveNodeContent(YAML::Emitter *out)
{

}

void FunctionNode::saveValues(YAML::Emitter *out)
{

}

QString FunctionNode::getValue()
{

}
