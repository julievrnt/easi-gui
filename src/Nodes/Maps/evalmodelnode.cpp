#include "evalmodelnode.h"
#include "src/Connectors/outputs/outputconnector.h"

EvalModelNode::EvalModelNode(QStringList* inputs, QStringList* outputs)
{
    typeOfNode = EVALMODELNODE;
    localTag = "EvalModel";
    setWindowTitle("Eval Model");
    createLayout();

    setGeometry(QRect(0, 0, sizeHint().width(), sizeHint().height()));
}

void EvalModelNode::setOutputConnectorModel(QGraphicsProxyWidget* newOutputConnectorModel)
{
    outputConnectorModel = newOutputConnectorModel;
}

void EvalModelNode::performResize()
{

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

    addSeparatorLineInLayout(parametersLayout);
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
    //((OutputConnector*) outputConnectorModel->widget())->transferOutputs(inputs);
}

void EvalModelNode::saveValues(YAML::Emitter* out)
{

}
