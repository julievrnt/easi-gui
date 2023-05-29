#include "layeredmodelnodenode.h"
#include <QDoubleSpinBox>
#include <QHBoxLayout>
#include <QLabel>
#include "float.h"

LayeredModelNodeNode::LayeredModelNodeNode(QSharedPointer<QStringList> inputs) : NodeBase(inputs)
{
    this->typeOfNode = LAYEREDMODELNODENODE;
    hasMenu = false;
    setWindowTitle("Node");
    createLayout(true, false, false);

    setGeometry(QRect(0, 0, sizeHint().width(), sizeHint().height()));
}

void LayeredModelNodeNode::setValues(QSharedPointer<QStringList> inputs, QList<double>* values)
{
    if (inputs->size() != values->size())
    {
        qDebug() << "MATRIX NODE: sizes of inputs and values different !";
        return;
    }

    this->inputs = inputs;

    QVBoxLayout* dimensionsLayout = this->layout()->findChild<QVBoxLayout*>("dimensionsLayout");
    while (dimensionsLayout->children().size() < inputs->size())
        addNewDimensionsLayoutRow(dimensionsLayout, dimensionsLayout->children().size());
    for (int i = 0; i < inputs->size(); i++)
    {
        QHBoxLayout* row = static_cast<QHBoxLayout*>(dimensionsLayout->children().at(i));
        int index = inputs->indexOf(static_cast<QLabel*>(row->itemAt(0)->widget())->text());
        static_cast<QDoubleSpinBox*>(row->itemAt(1)->widget())->setValue(values->at(index));
    }
}

void LayeredModelNodeNode::addNewDimensionsLayoutRow(QVBoxLayout* dimensionsLayout, int index)
{
    QHBoxLayout* row = new QHBoxLayout();

    // add a label that contains the output name
    QLabel* outputName = new QLabel(inputs->at(index));
    outputName->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
    outputName->setAlignment(Qt::AlignCenter);
    row->addWidget(outputName);

    // add a case that will contain the value that has to be multiplied with the value of the output
    QDoubleSpinBox* valueField = new QDoubleSpinBox();
    valueField->setObjectName("value");
    valueField->setButtonSymbols(QAbstractSpinBox::NoButtons);
    valueField->setRange(-DBL_MAX, DBL_MAX);
    valueField->setDecimals(3);
    valueField->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    valueField->setMaximumWidth(100);
    valueField->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
    row->addWidget(valueField);

    dimensionsLayout->insertLayout(index, row);
}

QList<double>* LayeredModelNodeNode::getValues()
{
    QList<double>* values = new QList<double>();

    QObjectList dimensionsLayout = this->layout()->findChild<QVBoxLayout*>("dimensionsLayout")->children();
    foreach (QObject* row, dimensionsLayout)
    {
        QHBoxLayout* layout = static_cast<QHBoxLayout*>(row);
        double value = static_cast<QDoubleSpinBox*>(layout->itemAt(1)->widget())->value();
        values->append(value);
    }

    return values;
}

void LayeredModelNodeNode::updateLayout()
{
    QVBoxLayout* dimensionsLayout = this->layout()->findChild<QVBoxLayout*>("dimensionsLayout");
    QStringList oldInputs = QStringList();

    if (!dimensionsLayout->children().isEmpty())
    {
        foreach (QObject* child, dimensionsLayout->children())
        {
            oldInputs.append(static_cast<QLabel*>(static_cast<QHBoxLayout*>(child)->itemAt(0)->widget())->text());
        }
    }

    for (int i = 0; i < oldInputs.size(); i++)
    {
        if (!inputs->contains(oldInputs.at(i)))
        {
            removeLayoutRow(dimensionsLayout, i);
            oldInputs.remove(i);
            i--;
        }
    }

    for (int i = 0; i < inputs->size(); i++)
    {
        if (!oldInputs.contains(inputs->at(i)))
        {
            oldInputs.insert(i, inputs->at(i));
            addNewDimensionsLayoutRow(dimensionsLayout, i);
        }
    }
}

void LayeredModelNodeNode::saveNodeContent(YAML::Emitter* out)
{
    saveValues(out);
}

void LayeredModelNodeNode::saveValues(YAML::Emitter* out)
{
    QList<double>* values = getValues();
    *out << YAML::Flow << YAML::BeginSeq;
    for (int i = 0; i < values->size(); i++)
        *out << values->at(i);
    *out << YAML::EndSeq << YAML::Block;

    delete values;
}
