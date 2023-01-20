#include "matrixnode.h"
#include <QDoubleSpinBox>
#include <QHBoxLayout>
#include <QLabel>
#include "float.h"

MatrixNode::MatrixNode(QStringList* outputs) : NodeBase(outputs)
{
    this->typeOfNode = MATH;
    setWindowTitle("Matrix");
    createLayout();

    setGeometry(QRect(0, 0, sizeHint().width(), sizeHint().height()));
}

MatrixNode::MatrixNode(QStringList* outputs, QList<double>* values) : MatrixNode(outputs)
{
    if (values == nullptr)
        return;

    QObjectList matrixLayout = this->layout()->findChild<QVBoxLayout*>("matrixLayout")->children();
    foreach (QObject* matrixLayoutRow, matrixLayout)
    {
        QHBoxLayout* layout = (QHBoxLayout*) matrixLayoutRow;
        int index = outputs->indexOf(((QLabel*) layout->itemAt(0)->widget())->text());
        ((QDoubleSpinBox*) layout->itemAt(1)->widget())->setValue(values->at(index));
    }
    delete values;
}

MatrixNode::~MatrixNode()
{
    /// TODO
}

void MatrixNode::createLayout()
{
    QVBoxLayout* globalLayout = new QVBoxLayout(this);
    addTitleLayout(globalLayout);

    // add a layout containing only one dimension
    QVBoxLayout* matrixLayout = new QVBoxLayout();
    matrixLayout->setObjectName("matrixLayout");

    if (outputs != nullptr)
    {
        for (int i = 0; i < outputs->size(); i++)
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
    QLabel* outputName = new QLabel(outputs->at(index));
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

void MatrixNode::updateLayout()
{
    /// TODO
    QVBoxLayout* matrixLayout = this->layout()->findChild<QVBoxLayout*>("matrixLayout");
    QStringList oldOutputs = QStringList();

    if (!matrixLayout->children().isEmpty())
    {
        foreach (QObject* child, matrixLayout->children())
        {
            oldOutputs.append(((QLabel*) ((QHBoxLayout*) child)->itemAt(0)->widget())->text());
        }
    }

    for (int i = 0; i < oldOutputs.size(); i++)
    {
        if (!outputs->contains(oldOutputs.at(i)))
        {
            removeLayoutAtIndex(matrixLayout, i);
            oldOutputs.remove(i);
            i--;
        }
    }

    for (int i = 0; i < oldOutputs.size(); i++)
    {
        if (oldOutputs.at(i) != outputs->at(i))
        {
            if (!oldOutputs.contains(outputs->at(i)))
            {
                addNewOutputRowLayout(matrixLayout, i);
                oldOutputs.insert(i, outputs->at(i));
            }
        }
    }

    for (int i = oldOutputs.size(); i < outputs->size(); i++)
    {
        addNewOutputRowLayout(matrixLayout, i);
    }
}
