#include "matrixnode.h"
#include <QDoubleSpinBox>
#include <QHBoxLayout>
#include <QLabel>
#include "float.h"

MatrixNode::MatrixNode(QStringList* inputs) : NodeBase(inputs)
{
    this->typeOfNode = MATHNODE;
    setWindowTitle("Matrix");
    createLayout();

    setGeometry(QRect(0, 0, sizeHint().width(), sizeHint().height()));
}

MatrixNode::~MatrixNode()
{
    /// TODO
}

void MatrixNode::setValues(QStringList* inputs, QList<double>* values)
{
    if (inputs->size() != values->size())
    {
        qDebug() << "MATRIX NODE: sizes of inputs and values different !";
        return;
    }

    QObjectList matrixLayout = this->layout()->findChild<QVBoxLayout*>("matrixLayout")->children();
    foreach (QObject* matrixLayoutRow, matrixLayout)
    {
        QHBoxLayout* layout = (QHBoxLayout*) matrixLayoutRow;
        int index = inputs->indexOf(((QLabel*) layout->itemAt(0)->widget())->text());
        ((QDoubleSpinBox*) layout->itemAt(1)->widget())->setValue(values->at(index));
    }
}

void MatrixNode::createLayout()
{
    QVBoxLayout* globalLayout = new QVBoxLayout(this);
    addTitleLayout(globalLayout, true);

    // add a layout containing dimensions
    QVBoxLayout* matrixLayout = new QVBoxLayout();
    matrixLayout->setObjectName("matrixLayout");

    if (inputs != nullptr)
    {
        for (int i = 0; i < inputs->size(); i++)
        {
            addNewOutputRowLayout(matrixLayout, i);
        }
    }

    globalLayout->addLayout(matrixLayout);
    this->setLayout(globalLayout);
}

void MatrixNode::addNewOutputRowLayout(QVBoxLayout* matrixLayout, int index)
{
    if (index < 0)
        return;

    QHBoxLayout* matrixRowLayout = new QHBoxLayout();

    // add a label that contains the output name
    QLabel* outputName = new QLabel(inputs->at(index));
    outputName->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
    matrixRowLayout->addWidget(outputName);

    // add a case that will contain the value that has to be multiplied with the value of the output
    QDoubleSpinBox* valueField = new QDoubleSpinBox();
    valueField->setObjectName("value");
    valueField->setButtonSymbols(QAbstractSpinBox::NoButtons);
    valueField->setRange(-DBL_MAX, DBL_MAX);
    valueField->setDecimals(3);
    valueField->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    valueField->setMaximumWidth(100);
    valueField->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
    matrixRowLayout->addWidget(valueField);

    matrixLayout->insertLayout(index, matrixRowLayout);
}

QList<double> MatrixNode::getValues()
{
    QList<double> values;

    QObjectList matrixLayout = this->layout()->findChild<QVBoxLayout*>("matrixLayout")->children();
    foreach (QObject* matrixLayoutRow, matrixLayout)
    {
        QHBoxLayout* layout = (QHBoxLayout*) matrixLayoutRow;
        double value = ((QDoubleSpinBox*) layout->itemAt(1)->widget())->value();
        values.append(value);
    }

    return values;
}

void MatrixNode::updateLayout()
{
    QVBoxLayout* matrixLayout = this->layout()->findChild<QVBoxLayout*>("matrixLayout");
    QStringList oldInputs = QStringList();

    if (!matrixLayout->children().isEmpty())
    {
        foreach (QObject* child, matrixLayout->children())
        {
            oldInputs.append(((QLabel*) ((QHBoxLayout*) child)->itemAt(0)->widget())->text());
        }
    }

    for (int i = 0; i < oldInputs.size(); i++)
    {
        if (!inputs->contains(oldInputs.at(i)))
        {
            removeLayoutRow(matrixLayout, i);
            oldInputs.remove(i);
            i--;
        }
    }

    for (int i = 0; i < oldInputs.size(); i++)
    {
        if (oldInputs.at(i) != inputs->at(i))
        {
            if (!oldInputs.contains(inputs->at(i)))
            {
                addNewOutputRowLayout(matrixLayout, i);
                oldInputs.insert(i, inputs->at(i));
            }
        }
    }

    for (int i = oldInputs.size(); i < inputs->size(); i++)
    {
        addNewOutputRowLayout(matrixLayout, i);
    }
}

void MatrixNode::saveNodeContent(YAML::Emitter* out)
{
    *out << YAML::Flow;
    saveValues(out);
    *out << YAML::Block;
}

void MatrixNode::saveValues(YAML::Emitter* out)
{
    QList<double> values = getValues();
    *out << YAML::Value << YAML::BeginSeq;
    for (int i = 0; i < values.size(); i++)
        *out << values[i];
    *out << YAML::EndSeq;
}
