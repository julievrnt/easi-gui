#include "luamapnode.h"
#include "src/Nodes/Extra/functionnode.h"

LuaMapNode::LuaMapNode(QSharedPointer<QStringList> inputs, QSharedPointer<QStringList> outputs) : NodeBase(inputs, outputs)
{
    typeOfNode = LUAMAPNODE;
    localTag = "LuaMap";
    setWindowTitle("Lua Map");
    removeButtonIndex = 1;
    dimensionLineEditIndex = 0;
    createLayout();

    setGeometry(QRect(0, 0, sizeHint().width(), sizeHint().height()));
}

LuaMapNode::~LuaMapNode()
{
    // functionNodeProxy deleted in clearNodes()
}

void LuaMapNode::setValue(QString value)
{
    if (value.isEmpty())
        return;

    // set function value
    FunctionNode* functionNode = (FunctionNode*) functionNodeProxy->widget();
    functionNode->setValue(inputs, value);
}

void LuaMapNode::setFunctionNodeProxy(QGraphicsProxyWidget* newFunctionNodeProxy)
{
    functionNodeProxy = newFunctionNodeProxy;
}

void LuaMapNode::performResize()
{
    NodeBase::performResize();
    for (int i = 0; i < functionOutputConnectors->size(); i++)
    {
        int x = sizeHint().width() - 7;
        functionOutputConnectors->at(i)->setPos(x, 20);
    }
}

void LuaMapNode::clearNodes()
{
    emit deleteNodeRequested(functionNodeProxy);
}

void LuaMapNode::addNewDimensionsLayoutRow(QVBoxLayout* dimensionsLayout, int index)
{
    QHBoxLayout* row = new QHBoxLayout();

    QLineEdit* dimension = new QLineEdit();
    dimension->setObjectName(QString::number(index));
    dimension->setPlaceholderText("Dimension " + QString::number(index));
    if (outputs->size() > index)
        dimension->setText(outputs->at(index));
    else
        outputs->append("");
    dimension->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    dimension->setFixedSize(100, 40);
    connect(dimension, SIGNAL(textChanged(QString)), this, SLOT(dimensionNameChanged(QString)));
    row->addWidget(dimension);

    // add button to remove output
    addRemoveButton(row, index);

    dimensionsLayout->insertLayout(index, row);
}

void LuaMapNode::updateLayout()
{
    foreach (QGraphicsProxyWidget* functionOutputConnector, (*functionOutputConnectors))
    {
        emit ((OutputConnector*) functionOutputConnector->widget())->transferOutputsRequested(inputs);
    }
}

void LuaMapNode::saveValues(YAML::Emitter* out)
{
    *out << YAML::Key << "returns";

    QStringList dimensions(*outputs);
    dimensions.sort();
    *out << YAML::Value << YAML::Flow << YAML::BeginSeq;

    for (int i = 0; i < dimensions.size(); i++)
    {
        *out << dimensions.at(i).toStdString();
    }
    *out << YAML::EndSeq << YAML::Block;

    *out << YAML::Key << "function";
    *out << YAML::Value;
    ((OutputConnector*) functionOutputConnectors->at(0)->widget())->saveComponent(out);
}
