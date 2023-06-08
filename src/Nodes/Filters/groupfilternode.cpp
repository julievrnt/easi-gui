#include "groupfilternode.h"
#include <QSpinBox>
#include <limits.h>

GroupFilterNode::GroupFilterNode(QSharedPointer<QStringList> inputs) : NodeBase(inputs)
{
    typeOfNode = GROUPFILTERNODE;
    localTag = "GroupFilter";
    setWindowTitle("Group Filter");
    removeButtonIndex = 1;
    createLayout();

    setGeometry(QRect(0, 0, sizeHint().width(), sizeHint().height()));
}

GroupFilterNode::GroupFilterNode(QSharedPointer<QStringList> inputs, QList<int>* values) : GroupFilterNode(inputs)
{
    if (values == nullptr)
        return;

    QVBoxLayout* dimensionsLayout = this->layout()->findChild<QVBoxLayout*>("dimensionsLayout");
    for (int i = 0; i < values->size(); i++)
    {
        addNewDimensionsLayoutRow(dimensionsLayout, i);
        QHBoxLayout* row = static_cast<QHBoxLayout*>(dimensionsLayout->itemAt(i));
        static_cast<QSpinBox*>(row->itemAt(0)->widget())->setValue(values->at(i));
    }

    delete values;
}

QList<int>* GroupFilterNode::getValues()
{
    QList<int>* values = new QList<int>();

    QObjectList dimensionsLayout = this->layout()->findChild<QVBoxLayout*>("dimensionsLayout")->children();
    foreach (QObject* row, dimensionsLayout)
    {
        QHBoxLayout* layout = static_cast<QHBoxLayout*>(row);
        int value = static_cast<QSpinBox*>(layout->itemAt(0)->widget())->value();
        values->append(value);
    }

    return values;
}

void GroupFilterNode::addNewDimensionsLayoutRow(QVBoxLayout* dimensionsLayout, int index)
{
    QHBoxLayout* row = new QHBoxLayout();

    QSpinBox* valueField = new QSpinBox();
    valueField->setButtonSymbols(QAbstractSpinBox::NoButtons);
    valueField->setRange(INT_MIN, INT_MAX);
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
    QList<int>* values = getValues();

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
