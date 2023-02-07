#include "scecfilenode.h"
#include <QComboBox>
#include <QFileDialog>

SCECFileNode::SCECFileNode(QString filePath, QString interpolation)
{
    typeOfNode = SCECFILENODE;
    localTag = "SCECFile";
    setWindowTitle("SCEC File");

    outputs->append("cohesion");
    outputs->append("d_c");
    outputs->append("forced_rupture_time");
    outputs->append("mu_d");
    outputs->append("mu_s");
    outputs->append("s_dip");
    outputs->append("s_normal");
    outputs->append("s_strike");

    if (filePath == "")
        this->filePath = NO_FILE_SELECTED;
    else
        this->filePath = filePath;

    if (interpolation != "linear" && interpolation != "nearest")
        this->interpolation = "linear";
    else
        this->interpolation = interpolation;

    createLayout();

    setGeometry(QRect(0, 0, sizeHint().width(), sizeHint().height()));
}

void SCECFileNode::createLayout()
{
    QVBoxLayout* globalLayout = new QVBoxLayout();
    addTitleLayout(globalLayout, true);

    addFileLayout(globalLayout);
    addInterpolationLayout(globalLayout);

    addComponentsLayout(globalLayout);
    setLayout(globalLayout);
}

void SCECFileNode::addFileLayout(QVBoxLayout* globalLayout)
{
    QHBoxLayout* fileLayout = new QHBoxLayout();
    fileLayout->setObjectName("fileLayout");

    QLabel* fileLabel = new QLabel("file");
    fileLabel->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    fileLabel->setAlignment(Qt::AlignCenter);
    fileLayout->addWidget(fileLabel);

    QLabel* filePath = new QLabel(this->filePath);
    filePath->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    filePath->setFixedHeight(30);
    filePath->setAlignment(Qt::AlignCenter);
    filePath->setStyleSheet("QLabel { background-color : #242424; }");
    fileLayout->addWidget(filePath);

    QPushButton* selectFileButton = new QPushButton("select file");
    connect(selectFileButton, SIGNAL(clicked(bool)), this, SLOT(selectFile(bool)));
    fileLayout->addWidget(selectFileButton);
    globalLayout->addLayout(fileLayout);
}

void SCECFileNode::addInterpolationLayout(QVBoxLayout* globalLayout)
{
    QHBoxLayout* interpolationLayout = new QHBoxLayout();
    interpolationLayout->setObjectName("interpolationLayout");

    QLabel* interpolationLabel = new QLabel("interpolation");
    interpolationLabel->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    interpolationLabel->setAlignment(Qt::AlignCenter);
    interpolationLayout->addWidget(interpolationLabel);

    QComboBox* interpolation = new QComboBox();
    interpolation->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    interpolation->addItem("linear");
    interpolation->addItem("nearest");
    interpolation->setCurrentText(this->interpolation);
    connect(interpolation, SIGNAL(currentTextChanged(QString)), this, SLOT(setInterpolation(QString)));
    interpolationLayout->addWidget(interpolation);

    globalLayout->addLayout(interpolationLayout);
}

QString SCECFileNode::getFilePath()
{
    QString filePath = QFileDialog::getOpenFileName(this, tr("Open File"), "/home/juliehbr", tr("NetCDF files (*.nc *.cdf)"));
    return filePath;
}

void SCECFileNode::selectFile(bool clicked)
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

void SCECFileNode::setInterpolation(const QString& newInterpolation)
{
    interpolation = newInterpolation;
}

void SCECFileNode::saveValues(YAML::Emitter* out)
{
    *out << YAML::Key << "file" << YAML::Value << filePath.toStdString();
    *out << YAML::Key << "interpolation" << YAML::Value << interpolation.toStdString();
}
