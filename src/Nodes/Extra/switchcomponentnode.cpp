#include "switchcomponentnode.h"

SwitchComponentNode::SwitchComponentNode(QSharedPointer<QStringList> inputs, QSharedPointer<QStringList> outputs) : NodeBase(inputs, outputs)
{
    typeOfNode = SWITCHCOMPONENTNODE;
    setWindowTitle("Switch Component");
    removeButtonIndex = 1;
    dimensionLineEditIndex = 0;
    parameters = new QStringList();

    createLayout();

    setGeometry(QRect(0, 0, sizeHint().width(), sizeHint().height()));
}

void SwitchComponentNode::setParameters(QStringList* parameters)
{
    if (parameters == nullptr)
        return;
    else
        this->parameters = parameters;

    QVBoxLayout* dimensionsLayout = this->layout()->findChild<QVBoxLayout*>("dimensionsLayout");
    for (int i = 0; i < parameters->size(); i++)
    {
        addNewDimensionsLayoutRow(dimensionsLayout, i);
    }
}

void SwitchComponentNode::performResize()
{
    setGeometry(QRect(0, 0, sizeHint().width(), sizeHint().height()));
    emit resized(this->geometry());
    if (outputConnectors->size() > 0)
        outputConnectors->at(0)->setX(sizeHint().width() - 7);
}

void SwitchComponentNode::createLayout()
{
    QVBoxLayout* globalLayout = new QVBoxLayout();
    addTitleLayout(globalLayout, true);
    addParametersLayout(globalLayout);
    setLayout(globalLayout);
}

void SwitchComponentNode::addParametersLayout(QVBoxLayout* globalLayout)
{
    QVBoxLayout* parametersLayout = new QVBoxLayout();
    addLabel(parametersLayout, "parameters");
    addParameterLayout(parametersLayout);
    globalLayout->addLayout(parametersLayout);
}

void SwitchComponentNode::addParameterLayout(QVBoxLayout* parametersLayout)
{
    // add an empty layout that will contain the dimensions
    QVBoxLayout* dimensionsLayout = new QVBoxLayout();
    dimensionsLayout->setObjectName("dimensionsLayout");
    if (parameters != nullptr)
    {
        for (int i = 0; i < parameters->size(); i++)
        {
            addNewDimensionsLayoutRow(dimensionsLayout, i);
        }
    }
    parametersLayout->addLayout(dimensionsLayout);

    // add button to add dimension
    QPushButton* addButton = new QPushButton("+");
    connect(addButton, SIGNAL(clicked(bool)), this, SLOT(addDimensionsLayoutRowRequested(bool)));

    parametersLayout->addWidget(addButton);
}

void SwitchComponentNode::updateLayout()
{
    outputs = inputs;
    emit transferOutputsRequested(outputs);
}

void SwitchComponentNode::addNewDimensionsLayoutRow(QVBoxLayout* dimensionsLayout, int index)
{
    QHBoxLayout* row = new QHBoxLayout();

    QLineEdit* parameter = new QLineEdit();
    parameter->setPlaceholderText("Parameter " + QString::number(index));
    parameter->setObjectName(QString::number(index));
    if (parameters->size() > index)
        parameter->setText(parameters->at(index));
    else
        parameters->append("");
    parameter->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    connect(parameter, SIGNAL(textChanged(QString)), this, SLOT(parameterNameChanged(QString)));
    row->addWidget(parameter);

    addRemoveButton(row, index);
    dimensionsLayout->insertLayout(index, row);
}

void SwitchComponentNode::saveNodeContent(YAML::Emitter* out)
{
    saveValues(out);
    if (!((OutputConnector*) outputConnectors->at(0)->widget())->getConnectorLineConnected())
        return;
    *out << YAML::Value;
    ((OutputConnector*) outputConnectors->at(0)->widget())->saveComponent(out);
}

void SwitchComponentNode::saveValues(YAML::Emitter* out)
{
    *out << YAML::Key;
    *out << YAML::Flow << YAML::BeginSeq;
    for (int i = 0; i < parameters->size(); i++)
        *out << parameters->at(i).toStdString();
    *out << YAML::EndSeq;
}

void SwitchComponentNode::parameterNameChanged(QString newParameter)
{
    QLineEdit* parameter = qobject_cast<QLineEdit*>(sender());
    int index = parameter->objectName().toInt();
    parameters->insert(index, newParameter);
    parameters->removeAt(index + 1);
}
