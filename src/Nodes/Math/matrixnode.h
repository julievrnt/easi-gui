#ifndef MATRIXNODE_H
#define MATRIXNODE_H

#include "src/Nodes/nodebase.h"
#include <QVBoxLayout>

class MatrixNode : public NodeBase
{
    Q_OBJECT
public:
    MatrixNode(QStringList* inputs = nullptr);
    ~MatrixNode();

    void setValues(QStringList* inputs, QList<double>* values);

private:
    void createLayout();
    void addNewOutputRowLayout(QVBoxLayout* matrixLayout, int index);
    QList<double> getValues();

    // NodeBase interface
protected:
    void updateLayout();
    void saveNodeContent(YAML::Emitter* out);
    void saveValues(YAML::Emitter* out);
};

#endif // MATRIXNODE_H
