#include "constantmapnode.h"
#include <QHBoxLayout>
#include <QComboBox>
#include <QDoubleSpinBox>
#include <QLabel>
#include <QFrame>
#include <QSpacerItem>
#include <QLineEdit>
#include <QMap>
#include "float.h"

ConstantMapNode::ConstantMapNode(QGraphicsScene* nodeScene, QStringList* outputs)
{
    typeOfNode = CONSTANTMAPNODE;
    this->outputs = outputs;
    this->nodeScene = nodeScene;

    setWindowTitle("Constant Map");
    createLayout();

    setGeometry(QRect(0, 0, sizeHint().width(), sizeHint().height()));
    //getValues();
}

ConstantMapNode::~ConstantMapNode()
{
    delete outputs;
}

QMap<QString, double>*  ConstantMapNode::getValues()
{
    QMap<QString, double>* values = new QMap<QString, double>();

    QObjectList outputLayouts = this->layout()->findChild<QVBoxLayout*>("outputsLayout")->children();
    foreach (QObject* outputLayout, outputLayouts)
    {
        QHBoxLayout* layout = (QHBoxLayout*) outputLayout;
        QString name = layout->findChild<QLabel*> ("output")->text();
        double value = layout->findChild<QDoubleSpinBox*> ("value")->value();
        values->insert(name, value);
    }

    return values;
}

void ConstantMapNode::createLayout()
{
    QVBoxLayout* globalLayout = new QVBoxLayout(this);
    addTitleLayout(globalLayout);

    // add output parameters and field for value
    QVBoxLayout* outputsLayout = new QVBoxLayout();
    outputsLayout->setObjectName("outputsLayout");

    /// We assume outputs is sorted in MainWindow
    /// use insertRow to insert at the right place when adding a new output parameter!

    for (int i = 0; i < outputs->size(); ++i)
    {
        addNewOutputsLayoutRow(outputsLayout, i);
    }
    globalLayout->addLayout(outputsLayout);

    addComponentsLayout(globalLayout);
    this->setLayout(globalLayout);
}

void ConstantMapNode::addNewOutputsLayoutRow(QVBoxLayout* outputsLayout, int index)
{
    QHBoxLayout* outputLayout = new QHBoxLayout();

    // add label to display the name of the outputs
    QLabel* output = new QLabel();
    output->setObjectName("output");
    output->setText(outputs->at(index));
    output->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
    outputLayout->addWidget(output);

    // add spacer for esthetics
    QSpacerItem* spacer = new QSpacerItem(40, 20, QSizePolicy::MinimumExpanding);
    outputLayout->addItem(spacer);

    /// TODO: CHANGE TO QLineEdit coupled with QDoubleValidator?
    /// (can check for scientific notation)
    // add input field to get the value of the parameter
    QDoubleSpinBox* valueField = new QDoubleSpinBox();
    valueField->setObjectName("value");
    valueField->setButtonSymbols(QAbstractSpinBox::NoButtons);
    valueField->setRange(-DBL_MAX, DBL_MAX);
    valueField->setDecimals(3);
    valueField->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    valueField->setMaximumWidth(100);
    valueField->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
    outputLayout->addWidget(valueField);

    outputsLayout->insertLayout(index, outputLayout);
}

void ConstantMapNode::insertNewOutputAtIndex(QList<int> indexes)
{
    foreach (int index, indexes)
        addNewOutputsLayoutRow(this->layout()->findChild<QVBoxLayout*>("outputsLayout"), index);
    performResize(true);
}
