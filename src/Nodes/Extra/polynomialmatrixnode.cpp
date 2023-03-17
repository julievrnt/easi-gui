#include "polynomialmatrixnode.h"
#include <QLabel>
#include <QDoubleSpinBox>
#include "float.h"

PolynomialMatrixNode::PolynomialMatrixNode(QSharedPointer<QStringList> inputs, int degree) : NodeBase(inputs)
{
    this->typeOfNode = POLYNOMIALMATHNODE;
    setWindowTitle("Matrix");
    this->degree = degree;
    if (inputs->size() == 0)
        inputs->append("");
    createLayout(true, false, false);

    QVBoxLayout* dimensionsLayout = this->layout()->findChild<QVBoxLayout*>("dimensionsLayout");
    while (dimensionsLayout->children().size() < degree + 1)
        addNewDimensionsLayoutRow(dimensionsLayout, dimensionsLayout->children().size());

    setGeometry(QRect(0, 0, sizeHint().width(), sizeHint().height()));
}

void PolynomialMatrixNode::setValues(QSharedPointer<QStringList> inputs, QList<double>* values)
{
    if (inputs == nullptr || values == nullptr)
        return;

    this->inputs = inputs;

    QVBoxLayout* dimensionsLayout = this->layout()->findChild<QVBoxLayout*>("dimensionsLayout");
    for (int i =  values->size() - 1; i >= 0; i--)
    {
        QHBoxLayout* row = (QHBoxLayout*) dimensionsLayout->children().at(values->size() - 1 - i);
        ((QDoubleSpinBox*) row->itemAt(1)->widget())->setValue(values->at(i));
    }
}

void PolynomialMatrixNode::setDegree(int newDegree)
{
    degree = newDegree;

    QVBoxLayout* dimensionsLayout = this->layout()->findChild<QVBoxLayout*>("dimensionsLayout");

    while (dimensionsLayout->children().size() > newDegree + 1)
    {
        removeLayoutRow(dimensionsLayout, dimensionsLayout->children().size() - 1);
    }

    while (dimensionsLayout->children().size() < newDegree + 1)
    {
        addNewDimensionsLayoutRow(dimensionsLayout, dimensionsLayout->children().size());
    }
}

QList<double>* PolynomialMatrixNode::getValues()
{
    QList<double>* values = new QList<double>();

    QVBoxLayout* dimensionsLayout = this->layout()->findChild<QVBoxLayout*>("dimensionsLayout");

    for (int i = 0; i < dimensionsLayout->children().size(); i++)
    {
        QHBoxLayout* row = (QHBoxLayout*) dimensionsLayout->children().at(i);
        double value = ((QDoubleSpinBox*) row->itemAt(1)->widget())->value();
        values->append(value);
    }
    return values;
}

void PolynomialMatrixNode::addNewDimensionsLayoutRow(QVBoxLayout* dimensionsLayout, int index)
{
    QHBoxLayout* row = new QHBoxLayout();

    // add a label that contains the exponential
    QString exponentialText;
    if (index == 0)
        exponentialText = "1";
    else
        exponentialText = inputs->at(0) + "^" + QString::number(index);
    QLabel* exponential = new QLabel(exponentialText);
    exponential->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
    row->addWidget(exponential);

    // add a case that will contain the value that has to be multiplied with the exponential input
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

void PolynomialMatrixNode::updateLayout()
{
    QVBoxLayout* dimensionsLayout = this->layout()->findChild<QVBoxLayout*>("dimensionsLayout");
    QObjectList rows = dimensionsLayout->children();

    if (inputs == nullptr)
    {
        while (rows.size() > 0)
        {
            removeLayoutRow(dimensionsLayout, rows.size() - 1);
        }
        return;
    }

    if (rows.size() < 2)
        return;

    // retrieve old input
    QHBoxLayout* firstRow = (QHBoxLayout*) rows.at(1);
    QString oldInput = ((QLabel*) firstRow->itemAt(0)->widget())->text();
    if (oldInput == inputs->at(0))
        return;

    for (int i = 1; i < rows.size(); i++)
    {
        QHBoxLayout* row = (QHBoxLayout*) rows.at(i);
        ((QLabel*) row->itemAt(0)->widget())->setText(inputs->at(0) + "^" + QString::number(i));
    }
}

void PolynomialMatrixNode::saveNodeContent(YAML::Emitter* out)
{
    saveValues(out);
}

void PolynomialMatrixNode::saveValues(YAML::Emitter* out)
{
    QList<double>* values = getValues();

    *out << YAML::Flow << YAML::BeginSeq;
    for (int i = values->size() - 1; i >= 0; i--)
        *out << values->at(i);
    *out << YAML::EndSeq << YAML::Block;

    delete values;
}

void PolynomialMatrixNode::removeDimensionsLayoutRowRequested(bool clicked)
{
    Q_UNUSED(clicked);
    QVBoxLayout* dimensionsLayout = this->layout()->findChild<QVBoxLayout*>("dimensionsLayout");
    removeLayoutRow(dimensionsLayout, dimensionsLayout->children().size() - 1);
    performResize();
}
