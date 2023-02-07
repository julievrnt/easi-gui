#ifndef INCLUDENODE_H
#define INCLUDENODE_H

#include "src/Nodes/nodebase.h"

#define NO_FILE_SELECTED "No file selected"

class IncludeNode : public NodeBase
{
    Q_OBJECT
public:
    IncludeNode(QString filePath = NO_FILE_SELECTED);

private:
    QString filePath;

    void createLayout();
    void addFileLayout(QVBoxLayout* globalLayout);

    QString getFilePath();

private slots:
    void selectFile(bool clicked);

    // NodeBase interface
protected:
    void saveNodeContent(YAML::Emitter* out);
};

#endif // INCLUDENODE_H
