#include "translationnode.h"
#include <QDoubleSpinBox>
#include "float.h"


TranslationNode::TranslationNode() : NodeBase(nullptr)
{
    this->typeOfNode = AFFINEMATHNODE;
    setWindowTitle("Translation");
    createLayout();

    QVBoxLayout* dimensionsLayout = this->layout()->findChild<QVBoxLayout*>("dimensionsLayout");
    QDoubleSpinBox* valueField = new QDoubleSpinBox();
    valueField->setObjectName("value");
    valueField->setButtonSymbols(QAbstractSpinBox::NoButtons);
    valueField->setRange(-DBL_MAX, DBL_MAX);
    valueField->setDecimals(3);
    valueField->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    valueField->setMaximumWidth(100);
    valueField->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
    dimensionsLayout->addWidget(valueField);

    setGeometry(QRect(0, 0, sizeHint().width(), sizeHint().height()));
}

void TranslationNode::setValue(double value)
{
    QVBoxLayout* translationLayout = this->layout()->findChild<QVBoxLayout*>("dimensionsLayout");
    ((QDoubleSpinBox*) translationLayout->itemAt(0)->widget())->setValue(value);
}

void TranslationNode::saveNodeContent(YAML::Emitter* out)
{
    saveValues(out);
}

void TranslationNode::saveValues(YAML::Emitter* out)
{
    QVBoxLayout* translationLayout = this->layout()->findChild<QVBoxLayout*>("dimensionsLayout");
    double value = ((QDoubleSpinBox*) translationLayout->itemAt(0)->widget())->value();
    *out << YAML::Key << value;
}
