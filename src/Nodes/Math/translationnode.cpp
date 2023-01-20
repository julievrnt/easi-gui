#include "translationnode.h"
#include <QDoubleSpinBox>
#include "float.h"


TranslationNode::TranslationNode(QStringList* outputs) : NodeBase(outputs)
{
    this->typeOfNode = MATH;
    setWindowTitle("Translation");
    createLayout();

    setGeometry(QRect(0, 0, sizeHint().width(), sizeHint().height()));
}

TranslationNode::TranslationNode(double value) : TranslationNode()
{
    QVBoxLayout* translationLayout = this->layout()->findChild<QVBoxLayout*>("translationLayout");
    ((QDoubleSpinBox*) translationLayout->itemAt(0)->widget())->setValue(value);
}

TranslationNode::~TranslationNode()
{

}

void TranslationNode::createLayout()
{
    QVBoxLayout* globalLayout = new QVBoxLayout(this);
    addTitleLayout(globalLayout);

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
