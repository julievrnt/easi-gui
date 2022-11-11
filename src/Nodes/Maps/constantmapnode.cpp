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

ConstantMapNode::ConstantMapNode(QGraphicsScene* nodeScene)
{
    typeOfNode = CONSTANTMAPNODE;
    this->outputs = nullptr;
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
        QString name = ((QLabel*) layout->itemAt(0)->widget())->text();
        double value = ((QDoubleSpinBox*) layout->itemAt(2)->widget())->value();
        values->insert(name, value);
    }

    return values;
}

void ConstantMapNode::createLayout()
{
    QVBoxLayout* globalLayout = new QVBoxLayout(this);
    addTitleLayout(globalLayout);

    // add an empty layout that will contain the outputs
    QVBoxLayout* outputsLayout = new QVBoxLayout();
    outputsLayout->setObjectName("outputsLayout");

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

void ConstantMapNode::removeOldOutputsLayoutRow(QVBoxLayout* outputsLayout, int index)
{
    QObjectList children = outputsLayout->children();
    QHBoxLayout* layoutToRemove = (QHBoxLayout*) children.at(index);
    clearLayout(layoutToRemove, true);
    outputsLayout->removeItem(layoutToRemove);
    delete layoutToRemove;
}

void ConstantMapNode::updateLayout()
{
    QVBoxLayout* outputsLayout = this->layout()->findChild<QVBoxLayout*>("outputsLayout");
    QStringList oldOutputs = QStringList();

    if (!outputsLayout->children().isEmpty())
    {
        foreach (QObject* child, outputsLayout->children())
        {
            oldOutputs.append(((QLabel*) ((QHBoxLayout*) child)->itemAt(0)->widget())->text());
        }
    }

    for (int i = 0; i < oldOutputs.size(); i++)
    {
        if (!outputs->contains(oldOutputs.at(i)))
        {
            removeOldOutputsLayoutRow(outputsLayout, i);
            oldOutputs.remove(i);
            i--;
        }
    }

    for (int i = 0; i < oldOutputs.size(); i++)
    {
        if (oldOutputs.at(i) != outputs->at(i))
        {
            if (!oldOutputs.contains(outputs->at(i)))
            {
                addNewOutputsLayoutRow(outputsLayout, i);
                oldOutputs.insert(i, outputs->at(i));
            }
        }
    }

    for (int i = oldOutputs.size(); i < outputs->size(); i++)
    {
        addNewOutputsLayoutRow(outputsLayout, i);
    }
}

void ConstantMapNode::saveNodeContent(YAML::Emitter* out)
{
    *out << YAML::LocalTag("ConstantMap");
    *out << YAML::BeginMap;
    saveValues(out);
    saveComponents(out);
    *out << YAML::EndMap;
}

void ConstantMapNode::saveValues(YAML::Emitter* out)
{
    QMap<QString, double>* values = getValues();

    *out << YAML::Key << "map";
    *out << YAML::Value << YAML::BeginMap;
    foreach (QString key, values->keys())
    {
        *out << YAML::Key << key.toStdString();
        *out << YAML::Value << values->value(key);
    }
    *out << YAML::EndMap;

    delete values;
}
