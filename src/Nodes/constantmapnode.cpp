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
#include "src/Widgets/componentswidget.h"

ConstantMapNode::ConstantMapNode(QStringList* outputs)
{
    typeOfNode = CONSTANTMAPNODE;
    this->outputs = outputs;

    int height = (outputs->size() + 3 + getNumberOfComponents()) * 32 ;
    setGeometry(QRect(0, 0, 250, height));

    //this->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    setWindowTitle("Output Map");
    createLayout();

    //getValues();
}

ConstantMapNode::~ConstantMapNode()
{
    /// TODO: check if necessary
    // delete outputs;
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
    // add title
    QVBoxLayout* globalLayout = new QVBoxLayout(this);
    QLabel* title = new QLabel(this);
    title->setText(this->windowTitle());
    title->setAlignment(Qt::AlignCenter);
    title->setStyleSheet("QLabel { background-color : rgb(70,70,70);}");
    title->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    title->setFixedHeight(30);
    globalLayout->addWidget(title);

    // add line to separate title from the rest
    QFrame* line = new QFrame(this);
    line->setFrameShape(QFrame::HLine);
    line->setFrameShadow(QFrame::Sunken);
    globalLayout->addWidget(line);

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

    ComponentsWidget* components = new ComponentsWidget(this);
    connect(components, SIGNAL(componentsChanged(int)), this, SLOT(componentRemovedOrAdded(int)));
    globalLayout->addWidget(components);
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

void ConstantMapNode::insertNewOutputAtIndex(int index)
{
    performResize();
    addNewOutputsLayoutRow(this->layout()->findChild<QVBoxLayout*>("outputsLayout"), index);
}

void ConstantMapNode::performResize()
{
    int height = (outputs->size() + 3 + getNumberOfComponents()) * 32 ;
    setGeometry(QRect(0, 0, 250, height));
    emit resized(this->geometry());
}
