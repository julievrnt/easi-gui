#include "evalmodelnode.h"

EvalModelNode::EvalModelNode(QSharedPointer<QStringList> inputs, QSharedPointer<QStringList> outputs) : NodeBase(inputs, outputs)
{
    typeOfNode = EVALMODELNODE;
    localTag = "EvalModel";
    setWindowTitle("Eval Model");
    removeButtonIndex = 1;
    dimensionLineEditIndex = 0;
    outputConnectorModel = nullptr;
    createLayout();

    setGeometry(QRect(0, 0, sizeHint().width(), sizeHint().height()));
}

void EvalModelNode::performResize()
{
    NodeBase::performResize();
    if (outputConnectorModel != nullptr)
        outputConnectorModel->setX(outputConnectorModel->parentWidget()->geometry().width() - 7);
}

void EvalModelNode::createLayout()
{
    QVBoxLayout* globalLayout = new QVBoxLayout();
    addTitleLayout(globalLayout, true);

    addModelLayout(globalLayout);
    addParametersLayout(globalLayout);

    addComponentsLayout(globalLayout);
    setLayout(globalLayout);
}

void EvalModelNode::addModelLayout(QVBoxLayout* globalLayout)
{
    QHBoxLayout* modelLayout = new QHBoxLayout();
    modelLayout->setObjectName("modelLayout");

    addLabel(modelLayout, "model");

    globalLayout->addLayout(modelLayout);
}

void EvalModelNode::addParametersLayout(QVBoxLayout* globalLayout)
{
    QVBoxLayout* parametersLayout = new QVBoxLayout();
    addSeparatorLineInLayout(parametersLayout);

    addLabel(parametersLayout, "parameters");
    addDimensionLayout(parametersLayout, true);

    globalLayout->addLayout(parametersLayout);
}

void EvalModelNode::addNewDimensionsLayoutRow(QVBoxLayout* dimensionsLayout, int index)
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

void EvalModelNode::updateLayout()
{
    emit transferInputsRequested(inputs);
}

void EvalModelNode::saveValues(YAML::Emitter* out)
{
    *out << YAML::Key << "parameters";
    *out << YAML::Value << YAML::Flow << YAML::BeginSeq;
    for (int i = 0; i < outputs->size(); i++)
        *out << outputs->at(i).toStdString();
    *out << YAML::EndSeq << YAML::Block;

    *out << YAML::Key << "model" << YAML::Value;
    if (outputConnectorModel != nullptr && static_cast<OutputConnector*>(outputConnectorModel->widget())->getConnectorLineConnected())
        static_cast<OutputConnector*>(outputConnectorModel->widget())->saveComponent(out);
    else
        *out << YAML::BeginMap << YAML::EndMap;
}
