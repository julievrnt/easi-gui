#include "affinematrixnode.h"
#include <QDoubleSpinBox>
#include <QHBoxLayout>
#include <QLabel>
#include "float.h"

AffineMatrixNode::AffineMatrixNode(QStringList* inputs) : NodeBase(inputs)
{
    this->typeOfNode = AFFINEMATHNODE;
    setWindowTitle("Matrix");
    createLayout(true, false, false);

    setGeometry(QRect(0, 0, sizeHint().width(), sizeHint().height()));
}

AffineMatrixNode::~AffineMatrixNode()
{
    /// TODO
}

void AffineMatrixNode::setValues(QStringList* inputs, QList<double>* values)
{
    if (inputs->size() != values->size())
    {
        qDebug() << "MATRIX NODE: sizes of inputs and values different !";
        return;
    }

    this->inputs = inputs;

    QVBoxLayout* dimensionsLayout = this->layout()->findChild<QVBoxLayout*>("dimensionsLayout");

    for (int i = 0; i < inputs->size(); i++)
    {
        addNewDimensionsLayoutRow(dimensionsLayout, i);
        QHBoxLayout* row = (QHBoxLayout*) dimensionsLayout->children().at(i);
        int index = inputs->indexOf(((QLabel*) row->itemAt(0)->widget())->text());
        ((QDoubleSpinBox*) row->itemAt(1)->widget())->setValue(values->at(index));
    }
}

void AffineMatrixNode::addNewDimensionsLayoutRow(QVBoxLayout* dimensionsLayout, int index)
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

QList<double>* AffineMatrixNode::getValues()
{
    QList<double>* values = new QList<double>();

    QObjectList dimensionsLayout = this->layout()->findChild<QVBoxLayout*>("dimensionsLayout")->children();
    foreach (QObject* row, dimensionsLayout)
    {
        QHBoxLayout* layout = (QHBoxLayout*) row;
        double value = ((QDoubleSpinBox*) layout->itemAt(1)->widget())->value();
        values->append(value);
    }

    return values;
}

void AffineMatrixNode::updateLayout()
{
    QVBoxLayout* dimensionsLayout = this->layout()->findChild<QVBoxLayout*>("dimensionsLayout");
    QStringList oldInputs = QStringList();

    if (!dimensionsLayout->children().isEmpty())
    {
        foreach (QObject* child, dimensionsLayout->children())
        {
            oldInputs.append(((QLabel*) ((QHBoxLayout*) child)->itemAt(0)->widget())->text());
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

void AffineMatrixNode::saveNodeContent(YAML::Emitter* out)
{
    saveValues(out);
}

void AffineMatrixNode::saveValues(YAML::Emitter* out)
{
    QList<double>* values = getValues();
    *out << YAML::Flow << YAML::BeginSeq;
    for (int i = 0; i < values->size(); i++)
        *out << values->at(i);
    *out << YAML::EndSeq << YAML::Block;

    delete values;
}
