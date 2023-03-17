#include "sphericaldomainfilternode.h"
#include <QLabel>
#include <QDoubleSpinBox>
#include "float.h"

SphericalDomainFilterNode::SphericalDomainFilterNode(QSharedPointer<QStringList> inputs) : NodeBase(inputs, inputs)
{
    typeOfNode = SPHERICALDOMAINFILTERNODE;
    localTag = "SphericalDomainFilter";
    setWindowTitle("Spherical Domain Filter");
    if (inputs != nullptr)
        outputs = QSharedPointer<QStringList>(new QStringList(*inputs));
    createLayout(true, false);

    QVBoxLayout* globalLayout = (QVBoxLayout*) this->layout();

    QVBoxLayout* radiusAndLineLayout = new QVBoxLayout();
    QHBoxLayout* radiusLayout = new QHBoxLayout();
    radiusLayout->setObjectName("radiusLayout");

    QLabel* radius = new QLabel("radius");
    radius->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    radius->setAlignment(Qt::AlignCenter);
    radiusLayout->addWidget(radius);

    QDoubleSpinBox* radiusValue = new QDoubleSpinBox();
    radiusValue->setButtonSymbols(QAbstractSpinBox::NoButtons);
    radiusValue->setRange(-DBL_MAX, DBL_MAX);
    radiusValue->setDecimals(3);
    radiusValue->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    radiusValue->setMaximumWidth(100);
    radiusValue->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
    radiusLayout->addWidget(radiusValue);
    radiusAndLineLayout->addLayout(radiusLayout);

    QFrame* line = new QFrame(this);
    line->setFrameShape(QFrame::HLine);
    line->setFrameShadow(QFrame::Sunken);
    line->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    radiusAndLineLayout->addWidget(line);

    globalLayout->insertLayout(1, radiusAndLineLayout);

    setGeometry(QRect(0, 0, sizeHint().width(), sizeHint().height()));
}

SphericalDomainFilterNode::SphericalDomainFilterNode(QSharedPointer<QStringList> inputs, QList<double>* values) : SphericalDomainFilterNode(inputs)
{
    if (values == nullptr || values->size() == 0)
        return;

    QHBoxLayout* radiusLayout = this->layout()->findChild<QHBoxLayout*>("radiusLayout");
    ((QDoubleSpinBox*) radiusLayout->itemAt(1)->widget())->setValue(values->at(0));

    QVBoxLayout* dimensionsLayout = this->layout()->findChild<QVBoxLayout*>("dimensionsLayout");
    for (int i = 1; i < values->size(); i++)
    {
        QHBoxLayout* row = (QHBoxLayout*) dimensionsLayout->children().at(i - 1);
        ((QLabel*) row->itemAt(0)->widget())->setText(outputs->at(i - 1));
        ((QDoubleSpinBox*) row->itemAt(1)->widget())->setValue(values->at(i));
    }

    delete values;
}

QMap<QString, double>* SphericalDomainFilterNode::getCenterValues()
{
    QMap<QString, double>* centerValues = new QMap<QString, double>();

    QObjectList dimensionsLayout = this->layout()->findChild<QVBoxLayout*>("dimensionsLayout")->children();
    foreach (QObject* row, dimensionsLayout)
    {
        QHBoxLayout* layout = (QHBoxLayout*) row;
        QString name = ((QLabel*) layout->itemAt(0)->widget())->text();
        double value = ((QDoubleSpinBox*) layout->itemAt(1)->widget())->value();
        centerValues->insert(name, value);
    }

    return centerValues;
}

void SphericalDomainFilterNode::addNewDimensionsLayoutRow(QVBoxLayout* dimensionsLayout, int index)
{
    QHBoxLayout* row = new QHBoxLayout();

    QLabel* dimension = new QLabel();
    dimension->setText(outputs->at(index));
    dimension->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    dimension->setAlignment(Qt::AlignCenter);
    dimension->setFixedHeight(40);
    row->addWidget(dimension);

    QDoubleSpinBox* valueField = new QDoubleSpinBox();
    valueField->setButtonSymbols(QAbstractSpinBox::NoButtons);
    valueField->setRange(-DBL_MAX, DBL_MAX);
    valueField->setDecimals(3);
    valueField->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    valueField->setMaximumWidth(100);
    valueField->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
    row->addWidget(valueField);

    dimensionsLayout->insertLayout(index, row);
}

void SphericalDomainFilterNode::updateLayout()
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

void SphericalDomainFilterNode::saveValues(YAML::Emitter* out)
{
    QHBoxLayout* radiusLayout = this->layout()->findChild<QHBoxLayout*>("radiusLayout");
    double radiusValue = ((QDoubleSpinBox*) radiusLayout->itemAt(1)->widget())->value();

    QMap<QString, double>* centerValues = getCenterValues();
    QStringList centerNames(*outputs);
    centerNames.sort();

    (*out) << YAML::Key << "radius";
    (*out) << YAML::Value << radiusValue;

    (*out) << YAML::Key << "center";
    (*out) << YAML::BeginMap;

    for (int i = 0; i < centerNames.size(); i++)
    {
        (*out) << YAML::Key << centerNames[i].toStdString();
        (*out) << YAML::Value << centerValues->value(centerNames[i]);
    }

    (*out) << YAML::EndMap;

    delete centerValues;
}
