#include "includenode.h"
#include <QFileDialog>

IncludeNode::IncludeNode(QString filePath)
{
    typeOfNode = INCLUDENODE;
    localTag = "Include";
    setWindowTitle("Include");

    if (filePath == "")
        this->filePath = NO_FILE_SELECTED;
    else
        this->filePath = filePath;

    createLayout();

    setGeometry(QRect(0, 0, sizeHint().width(), sizeHint().height()));
}

void IncludeNode::createLayout()
{
    QVBoxLayout* globalLayout = new QVBoxLayout();
    addTitleLayout(globalLayout, true);
    addFileLayout(globalLayout);
    setLayout(globalLayout);
}

void IncludeNode::addFileLayout(QVBoxLayout* globalLayout)
{
    QHBoxLayout* fileLayout = new QHBoxLayout();
    fileLayout->setObjectName("fileLayout");

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

QString IncludeNode::getFilePath()
{
    QString filePath = QFileDialog::getOpenFileName(this, tr("Open File"), "/home/juliehbr", tr("YAML files (*.yaml)"));
    return filePath;
}

void IncludeNode::selectFile(bool clicked)
{
    Q_UNUSED(clicked);
    QString filePath = getFilePath();
    QHBoxLayout* fileLayout = this->layout()->findChild<QHBoxLayout*>("fileLayout");
    if (!filePath.isEmpty())
    {
        ((QLabel*) fileLayout->itemAt(0)->widget())->setText(filePath);
        this->filePath = filePath;
    }
}

void IncludeNode::saveNodeContent(YAML::Emitter* out)
{
    *out << YAML::LocalTag(localTag.toStdString()) << filePath.toStdString();
}
