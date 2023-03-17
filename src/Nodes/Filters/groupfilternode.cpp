#include "groupfilternode.h"
#include <QDoubleSpinBox>
#include "float.h"

GroupFilterNode::GroupFilterNode(QSharedPointer<QStringList> inputs) : NodeBase(inputs)
{
    typeOfNode = GROUPFILTERNODE;
    localTag = "GroupFilter";
    setWindowTitle("Group Filter");
    removeButtonIndex = 1;
    createLayout();

    setGeometry(QRect(0, 0, sizeHint().width(), sizeHint().height()));
}

GroupFilterNode::GroupFilterNode(QSharedPointer<QStringList> inputs, QList<double>* values) : GroupFilterNode(inputs)
{
    if (values == nullptr)
        return;

    QVBoxLayout* dimensionsLayout = this->layout()->findChild<QVBoxLayout*>("dimensionsLayout");
    for (int i = 0; i < values->size(); i++)
    {
        addNewDimensionsLayoutRow(dimensionsLayout, i);
        QHBoxLayout* row = (QHBoxLayout*) dimensionsLayout->itemAt(i);
        ((QDoubleSpinBox*) row->itemAt(0)->widget())->setValue(values->at(i));
    }

    delete values;
}

QList<double>* GroupFilterNode::getValues()
{
    QList<double>* values = new QList<double>();

    QObjectList dimensionsLayout = this->layout()->findChild<QVBoxLayout*>("dimensionsLayout")->children();
    foreach (QObject* row, dimensionsLayout)
    {
        QHBoxLayout* layout = (QHBoxLayout*) row;
        double value = ((QDoubleSpinBox*) layout->itemAt(0)->widget())->value();
        values->append(value);
    }

    return values;
}

void GroupFilterNode::addNewDimensionsLayoutRow(QVBoxLayout* dimensionsLayout, int index)
{
    QHBoxLayout* row = new QHBoxLayout();

    QDoubleSpinBox* valueField = new QDoubleSpinBox();
    valueField->setButtonSymbols(QAbstractSpinBox::NoButtons);
    valueField->setRange(-DBL_MAX, DBL_MAX);
    valueField->setDecimals(3);
    valueField->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    valueField->setMaximumWidth(100);
    valueField->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
    row->addWidget(valueField);

    // add button to remove dimension
    addRemoveButton(row, index);
    dimensionsLayout->insertLayout(index, row);
}

void GroupFilterNode::updateLayout()
{
    outputs = inputs;
    emit transferOutputsRequested(outputs);
}

void GroupFilterNode::saveValues(YAML::Emitter* out)
{
    QList<double>* values = getValues();

    *out << YAML::Key << "groups";
    if (values->size() == 1)
        *out << YAML::Value << values->at(0);
    else
    {
        std::sort(values->begin(), values->end());
        *out << YAML::Value;
        *out << YAML::Flow << YAML::BeginSeq;
        for (int i = 0; i < values->size(); i++)
        {
            *out << values->at(i);
        }
        *out << YAML::EndSeq << YAML::Block;
    }

    delete values;
}
