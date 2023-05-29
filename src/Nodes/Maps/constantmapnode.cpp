#include "constantmapnode.h"
#include <QHBoxLayout>
#include <QComboBox>
#include <QDoubleSpinBox>
#include <QPushButton>
#include <QFrame>
#include <QSpacerItem>
#include <QLineEdit>
#include <QMap>
#include "float.h"

ConstantMapNode::ConstantMapNode(QSharedPointer<QStringList> outputs) : NodeBase(nullptr, outputs)
{
    typeOfNode = CONSTANTMAPNODE;
    localTag = "ConstantMap";
    removeButtonIndex = 2;
    dimensionLineEditIndex = 0;
    setWindowTitle("Constant Map");
    createLayout();

    setGeometry(QRect(0, 0, sizeHint().width(), sizeHint().height()));
}

ConstantMapNode::ConstantMapNode(QSharedPointer<QStringList> outputs, QList<double>* values) : ConstantMapNode(outputs)
{
    if (values == nullptr)
        return;

    QObjectList dimensionsLayout = this->layout()->findChild<QVBoxLayout*>("dimensionsLayout")->children();
    foreach (QObject* row, dimensionsLayout)
    {
        QHBoxLayout* layout = static_cast<QHBoxLayout*>(row);
        int index = outputs->indexOf((static_cast<QLineEdit*>(layout->itemAt(0)->widget())->text()));
        static_cast<QDoubleSpinBox*>(layout->itemAt(1)->widget())->setValue(values->at(index));
    }
    delete values;
}

QMap<QString, double>*  ConstantMapNode::getValues()
{
    QMap<QString, double>* values = new QMap<QString, double>();

    QObjectList dimensionsLayout = this->layout()->findChild<QVBoxLayout*>("dimensionsLayout")->children();
    foreach (QObject* row, dimensionsLayout)
    {
        QHBoxLayout* layout = static_cast<QHBoxLayout*>(row);
        QString name = static_cast<QLineEdit*>(layout->itemAt(0)->widget())->text();
        double value = static_cast<QDoubleSpinBox*>(layout->itemAt(1)->widget())->value();
        values->insert(name, value);
    }

    return values;
}

void ConstantMapNode::addNewDimensionsLayoutRow(QVBoxLayout* dimensionsLayout, int index)
{
    QHBoxLayout* row = new QHBoxLayout();

    // add label to display the name of the outputs
    QLineEdit* dimension = new QLineEdit();
    dimension->setObjectName(QString::number(index));
    dimension->setPlaceholderText("Dimension " + QString::number(index));
    if (outputs->size() > index)
        dimension->setText(outputs->at(index));
    else
        outputs->append("");
    dimension->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Preferred);
    dimension->setFixedWidth(100);
    connect(dimension, SIGNAL(textChanged(QString)), this, SLOT(dimensionNameChanged(QString)));
    row->addWidget(dimension);

    // add input field to get the value of the parameter
    QDoubleSpinBox* valueField = new QDoubleSpinBox();
    valueField->setButtonSymbols(QAbstractSpinBox::NoButtons);
    valueField->setRange(-DBL_MAX, DBL_MAX);
    valueField->setDecimals(3);
    valueField->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    valueField->setMaximumWidth(100);
    valueField->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
    row->addWidget(valueField);

    // add button to remove dimension
    addRemoveButton(row, index);
    dimensionsLayout->insertLayout(index, row);
}

void ConstantMapNode::saveValues(YAML::Emitter* out)
{
    QMap<QString, double>* values = getValues();

    *out << YAML::Key << "map";
    *out << YAML::Value << YAML::BeginMap;
    QStringList sortKeys = values->keys();
    foreach (QString key, sortKeys)
    {
        *out << YAML::Key << key.toStdString();
        *out << YAML::Value << values->value(key);
    }
    *out << YAML::EndMap;

    delete values;
}
