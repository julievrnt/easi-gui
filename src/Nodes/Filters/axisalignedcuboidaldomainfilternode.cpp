#include "axisalignedcuboidaldomainfilternode.h"
#include <QDoubleSpinBox>
#include <QBitArray>
#include "float.h"

AxisAlignedCuboidalDomainFilterNode::AxisAlignedCuboidalDomainFilterNode(QStringList* inputs) : NodeBase(inputs, inputs)
{
    typeOfNode = AXISALIGNEDCUBOIDALDOMAINFILTERNODE;
    dimensionLineEditIndex = 1;
    setWindowTitle("Axis Aligned Cuboidal Domain Filter");
    if (inputs != nullptr)
        outputs = new QStringList(*inputs);
    createLayout();

    QLabel* title = (QLabel*) this->layout()->itemAt(0)->widget();
    title->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    title->setFixedWidth(260);

    setGeometry(QRect(0, 0, sizeHint().width(), sizeHint().height()));
}

AxisAlignedCuboidalDomainFilterNode::AxisAlignedCuboidalDomainFilterNode(QStringList* inputs, QList<double>* values) : AxisAlignedCuboidalDomainFilterNode(inputs)
{
    if (values == nullptr)
        return;

    QList<QHBoxLayout*> lowerLimitLayout = this->layout()->findChildren<QHBoxLayout*>("lowerLimitLayout");
    QList<QHBoxLayout*> upperLimitLayout = this->layout()->findChildren<QHBoxLayout*>("upperLimitLayout");

    for (int i = 0; i < outputs->size(); i++)
    {
        ((QDoubleSpinBox*) lowerLimitLayout.at(i)->itemAt(1)->widget())->setValue(values->at(i * 2));
        ((QDoubleSpinBox*) upperLimitLayout.at(i)->itemAt(1)->widget())->setValue(values->at(i * 2 + 1));
    }

    delete values;
}

QList<double>* AxisAlignedCuboidalDomainFilterNode::getValues()
{
    QList<double>* values = new QList<double>();

    QList<QHBoxLayout*> lowerLimitLayout = this->layout()->findChildren<QHBoxLayout*>("lowerLimitLayout");
    QList<QHBoxLayout*> upperLimitLayout = this->layout()->findChildren<QHBoxLayout*>("upperLimitLayout");

    for (int i = 0; i < outputs->size(); i++)
    {
        double lowerLimitValue = ((QDoubleSpinBox*) lowerLimitLayout.at(i)->itemAt(1)->widget())->value();
        double upperLimitValue = ((QDoubleSpinBox*) upperLimitLayout.at(i)->itemAt(1)->widget())->value();
        values->append(lowerLimitValue * 1000.0 / 1000.0);
        values->append(upperLimitValue * 1000.0 / 1000.0);
    }

    return values;
}

void AxisAlignedCuboidalDomainFilterNode::addNewDimensionsLayoutRow(QVBoxLayout* dimensionsLayout, int index)
{
    QHBoxLayout* row = new QHBoxLayout();

    QLabel* dimension = new QLabel();
    dimension->setText(outputs->at(index));
    dimension->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    dimension->setAlignment(Qt::AlignCenter);
    dimension->setFixedHeight(40);
    row->addWidget(dimension);

    QVBoxLayout* limitsLayout = new QVBoxLayout();

    QHBoxLayout* lowerLimitLayout = new QHBoxLayout();
    lowerLimitLayout->setObjectName("lowerLimitLayout");

    QLabel* lowerLimit = new QLabel("lower limit");
    lowerLimit->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Fixed);
    lowerLimitLayout->addWidget(lowerLimit);

    QDoubleSpinBox* lowerLimitValue = new QDoubleSpinBox();
    lowerLimitValue->setButtonSymbols(QAbstractSpinBox::NoButtons);
    lowerLimitValue->setRange(-DBL_MAX, DBL_MAX);
    lowerLimitValue->setDecimals(3);
    lowerLimitValue->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    lowerLimitValue->setMaximumWidth(100);
    lowerLimitValue->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
    lowerLimitLayout->addWidget(lowerLimitValue);

    limitsLayout->addLayout(lowerLimitLayout);

    QHBoxLayout* upperLimitLayout = new QHBoxLayout();
    upperLimitLayout->setObjectName("upperLimitLayout");

    QLabel* upperLimit = new QLabel("upper limit");
    upperLimit->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Fixed);
    upperLimitLayout->addWidget(upperLimit);

    QDoubleSpinBox* upperLimitValue = new QDoubleSpinBox();
    upperLimitValue->setButtonSymbols(QAbstractSpinBox::NoButtons);
    upperLimitValue->setRange(-DBL_MAX, DBL_MAX);
    upperLimitValue->setDecimals(3);
    upperLimitValue->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    upperLimitValue->setMaximumWidth(100);
    upperLimitValue->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
    upperLimitLayout->addWidget(upperLimitValue);

    limitsLayout->addLayout(upperLimitLayout);

    row->addLayout(limitsLayout);
    dimensionsLayout->insertLayout(index, row);
}

void AxisAlignedCuboidalDomainFilterNode::updateLayout()
{
    QVBoxLayout* dimensionsLayout = this->layout()->findChild<QVBoxLayout*>("dimensionsLayout");

    for (int i = 0; i < outputs->size(); i++)
    {
        if (!inputs->contains(outputs->at(i)))
        {
            removeLayoutRow(dimensionsLayout, i);
            outputs->removeAt(i);
            i--;
        }
    }

    for (int i = 0; i < inputs->size(); i++)
    {
        if (!outputs->contains(inputs->at(i)))
        {
            outputs->append(inputs->at(i));
            addNewDimensionsLayoutRow(dimensionsLayout, dimensionsLayout->children().size());
        }
    }

    emit transferOutputsRequested(outputs);
}

void AxisAlignedCuboidalDomainFilterNode::saveNodeContent(YAML::Emitter* out)
{
    *out << YAML::LocalTag("AxisAlignedCuboidalDomainFilter");
    *out << YAML::BeginMap;
    saveValues(out);
    saveComponents(out);
    *out << YAML::EndMap;
}

void AxisAlignedCuboidalDomainFilterNode::saveValues(YAML::Emitter* out)
{
    QList<double>* values = getValues();

    *out << "limits" << YAML::BeginMap;

    for (int i = 0; i < outputs->size(); i++)
    {
        *out << YAML::Key << outputs->at(i).toStdString();
        *out << YAML::Value;
        *out << YAML::Flow;
        *out << YAML::BeginSeq;
        *out << values->at(i * 2) << values->at(i * 2 + 1);
        *out << YAML::EndSeq;
        *out << YAML::Block;
    }

    *out << YAML::EndMap;

    delete values;
}
