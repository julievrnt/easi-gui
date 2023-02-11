#include "asaginode.h"
#include <QFileDialog>
#include <QLabel>
#include <QPushButton>
#include <QComboBox>

ASAGINode::ASAGINode(QStringList* outputs, QString filePath, QString var, QString interpolation) : NodeBase(nullptr, outputs)
{
    typeOfNode = ASAGINODE;
    localTag = "ASAGI";
    setWindowTitle("ASAGI");
    removeButtonIndex = 1;
    dimensionLineEditIndex = 0;

    if (filePath == "")
        this->filePath = NO_FILE_SELECTED;
    else
        this->filePath = filePath;

    this->var = var;

    if (interpolation != "linear" && interpolation != "nearest")
        this->interpolation = "linear";
    else
        this->interpolation = interpolation;

    createLayout();

    setGeometry(QRect(0, 0, sizeHint().width(), sizeHint().height()));
}

void ASAGINode::createLayout()
{
    QVBoxLayout* globalLayout = new QVBoxLayout();
    addTitleLayout(globalLayout, true);

    addFileLayout(globalLayout);
    addParametersLayout(globalLayout);
    addVarLayout(globalLayout);
    addInterpolationLayout(globalLayout);

    addComponentsLayout(globalLayout);
    setLayout(globalLayout);
}

void ASAGINode::addFileLayout(QVBoxLayout* globalLayout)
{
    QHBoxLayout* fileLayout = new QHBoxLayout();
    fileLayout->setObjectName("fileLayout");

    addLabel(fileLayout, "file");

    QLabel* filePath = addLabel(fileLayout,this->filePath);
    filePath->setFixedHeight(30);
    filePath->setStyleSheet("QLabel { background-color : #242424; }");

    QPushButton* selectFileButton = new QPushButton("select file");
    connect(selectFileButton, SIGNAL(clicked(bool)), this, SLOT(selectFile(bool)));
    fileLayout->addWidget(selectFileButton);
    globalLayout->addLayout(fileLayout);
}

void ASAGINode::addParametersLayout(QVBoxLayout* globalLayout)
{
    QVBoxLayout* parametersLayout = new QVBoxLayout();
    addSeparatorLineInLayout(parametersLayout);

    addLabel(parametersLayout, "parameters");
    addDimensionLayout(parametersLayout, true);

    addSeparatorLineInLayout(parametersLayout);
    globalLayout->addLayout(parametersLayout);
}

void ASAGINode::addVarLayout(QVBoxLayout* globalLayout)
{
    QHBoxLayout* varLayout = new QHBoxLayout();
    varLayout->setObjectName("varLayout");

    addLabel(varLayout, "var");

    QLineEdit* var = new QLineEdit(this->var);
    var->setPlaceholderText("data");
    var->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    var->setAlignment(Qt::AlignCenter);
    connect(var, SIGNAL(textChanged(QString)), this, SLOT(setVar(QString)));
    varLayout->addWidget(var);

    globalLayout->addLayout(varLayout);
}

void ASAGINode::addInterpolationLayout(QVBoxLayout* globalLayout)
{
    QHBoxLayout* interpolationLayout = new QHBoxLayout();
    interpolationLayout->setObjectName("interpolationLayout");

    addLabel(interpolationLayout, "interpolation");

    QComboBox* interpolation = new QComboBox();
    interpolation->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    interpolation->addItem("linear");
    interpolation->addItem("nearest");
    interpolation->setCurrentText(this->interpolation);
    connect(interpolation, SIGNAL(currentTextChanged(QString)), this, SLOT(setInterpolation(QString)));
    interpolationLayout->addWidget(interpolation);

    globalLayout->addLayout(interpolationLayout);
}

QString ASAGINode::getFilePath()
{
    QString filePath = QFileDialog::getOpenFileName(this, tr("Open File"), "/home/juliehbr", tr("NetCDF files (*.nc *.cdf)"));
    return filePath;
}

QStringList* ASAGINode::getParameters()
{
    QStringList* parameters = new QStringList();

    QVBoxLayout* dimensionsLayout = this->layout()->findChild<QVBoxLayout*>("dimensionsLayout");
    for (int i = 0; i < dimensionsLayout->children().size(); i++)
    {
        QHBoxLayout* row = (QHBoxLayout*) dimensionsLayout->children().at(i);
        QString parameter = ((QLineEdit*) row->itemAt(0)->widget())->text();
        parameters->append(parameter);
    }

    return parameters;
}

void ASAGINode::selectFile(bool clicked)
{
    Q_UNUSED(clicked);
    QString filePath = getFilePath();
    QHBoxLayout* fileLayout = this->layout()->findChild<QHBoxLayout*>("fileLayout");
    if (!filePath.isEmpty())
    {
        ((QLabel*) fileLayout->itemAt(1)->widget())->setText(filePath);
        this->filePath = filePath;
    }
}

void ASAGINode::setVar(const QString& newVar)
{
    var = newVar;
}

void ASAGINode::setInterpolation(const QString& newInterpolation)
{
    interpolation = newInterpolation;
}

void ASAGINode::addNewDimensionsLayoutRow(QVBoxLayout* dimensionsLayout, int index)
{
    QHBoxLayout* row = new QHBoxLayout();

    QLineEdit* parameter = new QLineEdit();
    parameter->setPlaceholderText("Parameter " + QString::number(index));
    parameter->setObjectName(QString::number(index));
    if (outputs->size() > index)
        parameter->setText(outputs->at(index));
    else
        outputs->append("");
    parameter->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    connect(parameter, SIGNAL(textChanged(QString)), this, SLOT(dimensionNameChanged(QString)));
    row->addWidget(parameter);

    addRemoveButton(row, index);
    dimensionsLayout->insertLayout(index, row);
}

void ASAGINode::saveValues(YAML::Emitter* out)
{
    *out << YAML::Key << "file" << YAML::Value << filePath.toStdString();

    QStringList* parameters = getParameters();
    *out << YAML::Key << "parameters";
    *out << YAML::Flow << YAML::BeginSeq;
    for (int i = 0; i < parameters->size(); i++)
        *out << parameters->at(i).toStdString();
    *out << YAML::Block << YAML::EndSeq;
    delete parameters;

    *out << YAML::Key << "var" << YAML::Value << var.toStdString();

    *out << YAML::Key << "interpolation" << YAML::Value << interpolation.toStdString();
}
