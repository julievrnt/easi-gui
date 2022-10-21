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

ConstantMapNode::ConstantMapNode(QStringList* constants)
{
    this->constants = constants;
    // space between rows + row itself is 32
    // add 1 for the title
    this->setGeometry(QRect(0, 0, 250, (constants->size() + 2) * 32));
    //this->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    this->setWindowTitle("Constant Map");
    createLayout();

    getValues();
    /// FOR DEBUGGING
    /// TODO: when adding an object, check that the objects are added in ascending order
    /*this->constants->append("Test VALUE");
    this->constants->append("mama");
    this->constants->sort();
    insertNewConstantAtIndex(constants->indexOf("mama"));
    insertNewConstantAtIndex(constants->indexOf("Test VALUE"));*/
}

ConstantMapNode::~ConstantMapNode()
{
    /// TODO: check if necessary
    // delete constants;
}

QMap<QString, double>*  ConstantMapNode::getValues()
{
    /// TODO: check we Lukas if constantMapNode can have children
    QObjectList constantLayouts = this->layout()->findChild<QVBoxLayout*>("constantsLayout")->children();
    foreach (QObject* constantLayout, constantLayouts)
    {
        QHBoxLayout* layout = (QHBoxLayout*) constantLayout;
        QString name = layout->findChild<QLabel*> ("name")->text();
        double value =
    }

    return &values;
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
    QVBoxLayout* constantsLayout = new QVBoxLayout();
    constantsLayout->setObjectName("constantsLayout");

    /// We assume constants is sorted in MainWindow
    /// use insertRow to insert at the right place when adding a new output parameter!

    for (int i = 0; i < constants->size(); ++i)
    {
        addNewConstantsLayoutRow(constantsLayout, i);
    }
    globalLayout->addLayout(constantsLayout);

    this->setLayout(globalLayout);
}

void ConstantMapNode::addNewConstantsLayoutRow(QVBoxLayout* constantsLayout, int index)
{
    QHBoxLayout* constantLayout = new QHBoxLayout();

    // add label to display the name of the constants
    QLabel* constant = new QLabel();
    constant->setText(constants->at(index));
    constant->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
    constantLayout->addWidget(constant);

    // add spacer for esthetics
    QSpacerItem* spacer = new QSpacerItem(40, 20, QSizePolicy::MinimumExpanding);
    constantLayout->addItem(spacer);

    /// TODO: CHANGE TO QLineEdit coupled with QDoubleValidator?
    /// (can check for scientific notation)
    // add input field to get the value of the parameter
    QDoubleSpinBox* valueField = new QDoubleSpinBox();
    valueField->setButtonSymbols(QAbstractSpinBox::NoButtons);
    valueField->setRange(-DBL_MAX, DBL_MAX);
    valueField->setDecimals(3);
    valueField->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    valueField->setMaximumWidth(100);
    valueField->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
    constantLayout->addWidget(valueField);

    constantsLayout->insertLayout(index, constantLayout);
}

void ConstantMapNode::insertNewConstantAtIndex(int index)
{
    this->setGeometry(QRect(0, 0, 250, (constants->size() + 2) * 32));
    emit resized(this->geometry());
    addNewConstantsLayoutRow(this->layout()->findChild<QVBoxLayout*>("constantsLayout"), index);
}
