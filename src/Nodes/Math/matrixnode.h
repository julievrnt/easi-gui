#ifndef MATRIXNODE_H
#define MATRIXNODE_H

#include "src/Nodes/nodebase.h"
#include <QVBoxLayout>

class MatrixNode : public NodeBase
{
    Q_OBJECT
public:
    MatrixNode(QStringList* outputs = nullptr);
    MatrixNode(QStringList* outputs, QList<double>* values);
    ~MatrixNode();

private:
    void createLayout();
    void addNewOutputRowLayout(QVBoxLayout* matrixLayout, int index);

    // NodeBase interface
protected:
    void updateLayout();
};

#endif // MATRIXNODE_H
