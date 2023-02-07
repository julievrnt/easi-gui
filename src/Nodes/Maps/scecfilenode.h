#ifndef SCECFILENODE_H
#define SCECFILENODE_H

#include "src/Nodes/nodebase.h"

#define NO_FILE_SELECTED "No file selected"

class SCECFileNode : public NodeBase
{
    Q_OBJECT
public:
    SCECFileNode(QString filePath = NO_FILE_SELECTED, QString interpolation = "linear");

private:
    QString filePath;
    QString interpolation;

    void createLayout();
    void addFileLayout(QVBoxLayout* globalLayout);
    void addInterpolationLayout(QVBoxLayout* globalLayout);

    QString getFilePath();

private slots:
    void selectFile(bool clicked);
    void setInterpolation(const QString& newInterpolation);

    // NodeBase interface
protected:
    void saveValues(YAML::Emitter* out);
};

#endif // SCECFILENODE_H
