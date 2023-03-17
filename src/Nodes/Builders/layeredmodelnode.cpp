#include "layeredmodelnode.h"
#include "src/Nodes/Extra/affinematrixnode.h"
#include <QComboBox>
#include <QTableWidget>

LayeredModelNode::LayeredModelNode(QSharedPointer<QStringList> inputs, QSharedPointer<QStringList> outputs, QString interpolation) : NodeBase(inputs, outputs)
{
    typeOfNode = LAYEREDMODELNODE;
    localTag = "LayeredModel";
    this->setWindowTitle("Layered Model");
    removeButtonIndex = 1;
    dimensionLineEditIndex = 0;
    outputConnectorModel = nullptr;

    if (interpolation != "lower" && interpolation != "upper" && interpolation != "linear")
        this->interpolation = "lower";
    else
        this->interpolation = interpolation;

    createLayout();

    setGeometry(QRect(0, 0, sizeHint().width(), sizeHint().height()));
}

void LayeredModelNode::setValues(QMap<double, QList<double>>* values)
{
    if (values == nullptr)
        return;

    QVBoxLayout* nodesLayout = this->layout()->findChild<QVBoxLayout*>("nodesLayout");
    QList<double> keys = values->keys();
    for (int i = 0; i < values->size(); i++)
    {
        addNewNodesLayoutRow(nodesLayout, i);
        QHBoxLayout* row = (QHBoxLayout*) nodesLayout->children().at(i);
        ((QDoubleSpinBox*) row->itemAt(1)->widget())->setValue(keys.at(i));

        addNodeConnector(i);

        // get matrix and translation values
        QList<double> matrixValues = values->value(keys.at(i));

        // set matrix values
        AffineMatrixNode* matrixNode = (AffineMatrixNode*) this->nodeProxies.at(i)->widget();
        matrixNode->setValues(outputs, &matrixValues);
    }

    delete values;
}

void LayeredModelNode::performResize()
{
    NodeBase::performResize();
    if (outputConnectorModel != nullptr)
        outputConnectorModel->setX(outputConnectorModel->parentWidget()->geometry().width() - 7);
    for (int i = 0; i < mathOutputConnectors->size(); i++)
    {
        int x = sizeHint().width() - 7;
        int y = (i + outputs->size()) * 32 + 220;
        mathOutputConnectors->at(i)->setPos(x, y);
    }
}

void LayeredModelNode::clearNodes()
{
    while (mathOutputConnectors->size() > 0)
    {
        removeNodeOfRow(mathOutputConnectors->size() - 1);
    }
}

void LayeredModelNode::addNodeProxy(QGraphicsProxyWidget*& newNodeProxy)
{
    nodeProxies.append(newNodeProxy);
}

void LayeredModelNode::createLayout()
{
    QVBoxLayout* globalLayout = new QVBoxLayout();

    addTitleLayout(globalLayout, true);
    addModelLayout(globalLayout);
    addInterpolationLayout(globalLayout);
    addParametersLayout(globalLayout);
    addNodesLayout(globalLayout);

    setLayout(globalLayout);
}

void LayeredModelNode::addModelLayout(QVBoxLayout* globalLayout)
{
    QHBoxLayout* modelLayout = new QHBoxLayout();
    modelLayout->setObjectName("mapLayout");

    addLabel(modelLayout, "map");

    globalLayout->addLayout(modelLayout);
}

void LayeredModelNode::addInterpolationLayout(QVBoxLayout* globalLayout)
{
    addSeparatorLineInLayout(globalLayout);
    QHBoxLayout* interpolationLayout = new QHBoxLayout();
    interpolationLayout->setObjectName("interpolationLayout");

    addLabel(interpolationLayout, "interpolation");

    QComboBox* interpolation = new QComboBox();
    interpolation->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    interpolation->addItem("lower");
    interpolation->addItem("upper");
    interpolation->addItem("linear");
    interpolation->setCurrentText(this->interpolation);
    connect(interpolation, SIGNAL(currentTextChanged(QString)), this, SLOT(setInterpolation(QString)));
    interpolationLayout->addWidget(interpolation);

    globalLayout->addLayout(interpolationLayout);
}

void LayeredModelNode::addParametersLayout(QVBoxLayout* globalLayout)
{
    QVBoxLayout* parametersLayout = new QVBoxLayout();
    addSeparatorLineInLayout(parametersLayout);

    addLabel(parametersLayout, "parameters");
    addDimensionLayout(parametersLayout, true);

    globalLayout->addLayout(parametersLayout);
}

void LayeredModelNode::addNodesLayout(QVBoxLayout* globalLayout)
{
    QVBoxLayout* globalNodesLayout = new QVBoxLayout();
    addSeparatorLineInLayout(globalNodesLayout);

    addLabel(globalNodesLayout, "nodes");
    // add an empty layout that will contain the nodes
    QVBoxLayout* nodesLayout = new QVBoxLayout();
    nodesLayout->setObjectName("nodesLayout");
    globalNodesLayout->addLayout(nodesLayout);

    // add button to add dimension
    QPushButton* addButton = new QPushButton("+");
    connect(addButton, SIGNAL(clicked(bool)), this, SLOT(addNodesLayoutRowRequested(bool)));

    globalNodesLayout->addWidget(addButton);

    globalLayout->addLayout(globalNodesLayout);
}

void LayeredModelNode::addNewDimensionsLayoutRow(QVBoxLayout* dimensionsLayout, int index)
{
    QHBoxLayout* row = new QHBoxLayout();

    QLineEdit* parameter = new QLineEdit();
    parameter->setPlaceholderText("Parameter " + QString::number(index));
    parameter->setObjectName(QString::number(index));
    if (outputs->size() > index)
        parameter->setText(outputs->at(index));
    else
        outputs->append("");
    parameter->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    connect(parameter, SIGNAL(textChanged(QString)), this, SLOT(dimensionNameChanged(QString)));
    row->addWidget(parameter);

    addRemoveButton(row, index);
    dimensionsLayout->insertLayout(index, row);
}

void LayeredModelNode::addNewNodesLayoutRow(QVBoxLayout* nodesLayout, int index)
{
    QHBoxLayout* row = new QHBoxLayout();

    QPushButton* removeButton = new QPushButton("-");
    removeButton->setObjectName(QString::number(index));
    connect(removeButton, SIGNAL(clicked(bool)), this, SLOT(removeNodesLayoutRowRequested(bool)));
    row->addWidget(removeButton);

    addDoubleSpinBox(row);

    nodesLayout->insertLayout(index, row);
}

void LayeredModelNode::addNodeConnector(int index)
{
    QPointF pos(sizeHint().width() - 7, (index + outputs->size()) * 32 + 220);
    emit addMathOutputConnectorRequested(proxyNode, pos);
}

void LayeredModelNode::removeNodeOfRow(int index)
{
    emit deleteOutputConnectorRequested(mathOutputConnectors->takeAt(index));
    emit deleteNodeRequested(nodeProxies.takeAt(index));
}

QList<double>* LayeredModelNode::getNodesValues()
{
    QList<double>* nodes = new QList<double>();
    QVBoxLayout* nodesLayout = this->layout()->findChild<QVBoxLayout*>("nodesLayout");
    foreach (QObject* child, nodesLayout->children())
    {
        QHBoxLayout* row = (QHBoxLayout*) child;
        double node = ((QDoubleSpinBox*) row->itemAt(1)->widget())->value();
        nodes->append(node);
    }

    return nodes;
}

void LayeredModelNode::setInterpolation(const QString& newInterpolation)
{
    this->interpolation = newInterpolation;
}

void LayeredModelNode::addNodesLayoutRowRequested(bool clicked)
{
    Q_UNUSED(clicked);
    QVBoxLayout* nodesLayout = this->layout()->findChild<QVBoxLayout*>("nodesLayout");
    addNewNodesLayoutRow(nodesLayout, nodesLayout->children().size());
    addNodeConnector(nodesLayout->children().size() - 1);
    performResize();
}

void LayeredModelNode::removeNodesLayoutRowRequested(bool clicked)
{
    Q_UNUSED(clicked);
    QVBoxLayout* nodesLayout = this->layout()->findChild<QVBoxLayout*>("nodesLayout");
    QPushButton* removeButton = qobject_cast<QPushButton*>(sender());
    int index = removeButton->objectName().toInt();
    removeNodeOfRow(index);
    removeLayoutRow(nodesLayout, index);
    if (index != nodesLayout->children().size())
    {
        removeButtonIndex = 0;
        renameNextRemoveButtons(nodesLayout, index);
        removeButtonIndex = 1;
    }
    performResize();
}

void LayeredModelNode::updateLayout()
{
    emit transferInputsRequested(inputs);
}

void LayeredModelNode::saveValues(YAML::Emitter* out)
{
    *out << YAML::Key << "map" << YAML::Value;
    if (outputConnectorModel != nullptr && ((OutputConnector*) outputConnectorModel->widget())->getConnectorLineConnected())
        ((OutputConnector*) outputConnectorModel->widget())->saveComponent(out);
    else
        *out << YAML::BeginMap << YAML::EndMap;

    *out << YAML::Key << "interpolation";
    *out << YAML::Value << interpolation.toStdString();

    *out << YAML::Key << "parameters";
    *out << YAML::Value << YAML::Flow << YAML::BeginSeq;
    for (int i = 0; i < outputs->size(); i++)
        *out << outputs->at(i).toStdString();
    *out << YAML::EndSeq << YAML::Block;

    QList<double>* nodes = getNodesValues();
    *out << YAML::Key << "nodes";
    *out << YAML::BeginMap;
    for (int i = 0; i < nodes->size(); i++)
    {
        *out << YAML::Key << nodes->at(i) << YAML::Value;
        ((OutputConnector*) mathOutputConnectors->at(i)->widget())->saveComponent(out);
    }
    *out << YAML::EndMap;

    delete nodes;
}
