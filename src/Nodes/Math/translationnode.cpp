#include "translationnode.h"
#include <QDoubleSpinBox>
#include "float.h"


TranslationNode::TranslationNode() : NodeBase(nullptr)
{
    this->typeOfNode = MATHNODE;
    setWindowTitle("Translation");
    createLayout();

    setGeometry(QRect(0, 0, sizeHint().width(), sizeHint().height()));
}

TranslationNode::~TranslationNode()
{

}

void TranslationNode::setValue(double value)
{
    QVBoxLayout* translationLayout = this->layout()->findChild<QVBoxLayout*>("translationLayout");
    ((QDoubleSpinBox*) translationLayout->itemAt(0)->widget())->setValue(value);
}

void TranslationNode::createLayout()
{
    QVBoxLayout* globalLayout = new QVBoxLayout(this);
    addTitleLayout(globalLayout, true);

    // add a layout containing only one dimension
    QVBoxLayout* translationLayout = new QVBoxLayout();
    translationLayout->setObjectName("translationLayout");

    QDoubleSpinBox* valueField = new QDoubleSpinBox();
    valueField->setObjectName("value");
    valueField->setButtonSymbols(QAbstractSpinBox::NoButtons);
    valueField->setRange(-DBL_MAX, DBL_MAX);
    valueField->setDecimals(3);
    valueField->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    valueField->setMaximumWidth(100);
    valueField->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
    translationLayout->addWidget(valueField);

    globalLayout->addLayout(translationLayout);
    this->setLayout(globalLayout);
}

void TranslationNode::saveNodeContent(YAML::Emitter* out)
{
    *out << YAML::Key;
    saveValues(out);
}

void TranslationNode::saveValues(YAML::Emitter* out)
{
    QVBoxLayout* translationLayout = this->layout()->findChild<QVBoxLayout*>("translationLayout");
    double value = ((QDoubleSpinBox*) translationLayout->itemAt(0)->widget())->value();
    *out << value;
}
