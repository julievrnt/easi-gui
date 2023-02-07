#ifndef ASAGINODE_H
#define ASAGINODE_H

#include "src/Nodes/nodebase.h"

#define NO_FILE_SELECTED "No file selected"

class ASAGINode : public NodeBase
{
    Q_OBJECT
public:
    ASAGINode(QStringList* outputs = nullptr, QString filePath = NO_FILE_SELECTED, QString var = "data", QString interpolation = "linear");

private:
    QString filePath;
    QString var;
    QString interpolation;

    void createLayout();
    void addFileLayout(QVBoxLayout* globalLayout);
    void addParametersLayout(QVBoxLayout* globalLayout);
    void addVarLayout(QVBoxLayout* globalLayout);
    void addInterpolationLayout(QVBoxLayout* globalLayout);

    QString getFilePath();
    QStringList* getParameters();
    void setParameters(QStringList* parameters);

private slots:
    void selectFile(bool clicked);
    void setVar(const QString& newVar);
    void setInterpolation(const QString& newInterpolation);

    // NodeBase interface
protected:
    void addNewDimensionsLayoutRow(QVBoxLayout* dimensionsLayout, int index);
    void saveValues(YAML::Emitter* out);
};

#endif // ASAGINODE_H
