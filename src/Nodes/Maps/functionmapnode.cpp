#include "functionmapnode.h"
#include "src/Nodes/Extra/functionnode.h"

FunctionMapNode::FunctionMapNode(QStringList* inputs, QStringList* outputs) : NodeBase(inputs, outputs)
{
    typeOfNode = FUNCTIONMAPNODE;
    localTag = "FunctionMap";
    setWindowTitle("Function Map");
    removeButtonIndex = 0;
    dimensionLineEditIndex = 1;
    createLayout();

    setGeometry(QRect(0, 0, sizeHint().width(), sizeHint().height()));
}

void FunctionMapNode::setValues(QMap<QString, QString> *values)
{
    if (values == nullptr)
        return;

    if (outputs->size() != values->size())
        qDebug() << "outputs and values sizes are different ";

    for (int i = 0; i < outputs->size(); i++)
    {
        addFunctionConnector(i);

        // get matrix and translation values
        QString function = values->value(outputs->at(i));

        // set matrix values
        FunctionNode* functionNode = (FunctionNode*) this->functionNodeProxies.at(i)->widget();
        functionNode->setValue(inputs, function);
    }

    delete values;
}

void FunctionMapNode::addFunctionNodeProxy(QGraphicsProxyWidget *newFunctionNodeProxy)
{
    functionNodeProxies.append(newFunctionNodeProxy);
}

void FunctionMapNode::performResize()
{
    NodeBase::performResize();
}

void FunctionMapNode::clearNodes()
{
    while (functionNodeProxies.size() > 0)
        removeFunctionOfDimensionRow(functionNodeProxies.size() - 1);
}

void FunctionMapNode::addFunctionConnector(int index)
{
    QPointF posMatrix(sizeHint().width() - 7, 110 + index * 46);
    emit addFunctionOutputConnectorRequested(proxyNode, posMatrix);
}

void FunctionMapNode::removeFunctionOfDimensionRow(int index)
{
    emit deleteOutputConnectorRequested(functionOutputConnectors->takeAt(index));
    emit deleteNodeRequested(functionNodeProxies.takeAt(index));
}

void FunctionMapNode::addNewDimensionsLayoutRow(QVBoxLayout *dimensionsLayout, int index)
{
    QHBoxLayout* row = new QHBoxLayout();

    // add button to remove output
    addRemoveButton(row, index);

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

    dimensionsLayout->insertLayout(index, row);
}

void FunctionMapNode::addDimensionsLayoutRowRequested(bool clicked)
{
    NodeBase::addDimensionsLayoutRowRequested(clicked);
    QVBoxLayout* dimensionsLayout = this->layout()->findChild<QVBoxLayout*>("dimensionsLayout");
    addFunctionConnector(dimensionsLayout->children().size() - 1);
    updateLayout();
}

void FunctionMapNode::removeDimensionsLayoutRowRequested(bool clicked)
{
    QPushButton* removeButton = qobject_cast<QPushButton*>(sender());
    removeFunctionOfDimensionRow(removeButton->objectName().toInt());
    NodeBase::removeDimensionsLayoutRowRequested(clicked);
}

void FunctionMapNode::updateLayout()
{
    foreach (QGraphicsProxyWidget* functionOutputConnector, (*functionOutputConnectors))
    {
        emit ((OutputConnector*) functionOutputConnector->widget())->transferOutputsRequested(inputs);
    }
}

void FunctionMapNode::saveValues(YAML::Emitter *out)
{
    *out << YAML::Key << "map";
    *out << YAML::BeginMap;

    QStringList dimensions(*outputs);
    dimensions.sort();

    for (int i = 0; i < dimensions.size(); i++)
    {
        int index = outputs->indexOf(dimensions[i]);
        *out << YAML::Key << dimensions.at(i).toStdString();
        *out << YAML::Value;
        ((OutputConnector*) functionOutputConnectors->at(index)->widget())->saveComponent(out);
    }

    *out << YAML::EndMap;
}
