#ifndef POLYNOMIALMATRIXNODE_H
#define POLYNOMIALMATRIXNODE_H

#include "src/Nodes/nodebase.h"

class PolynomialMatrixNode : public NodeBase
{
    Q_OBJECT
public:
    PolynomialMatrixNode(QStringList* inputs = nullptr, int degree = 0);

    void setValues(QStringList* inputs, QList<double>* values);

    void setDegree(int newDegree);

private:
    int degree = 0;
    QList<double>* getValues();

protected:
    void addNewDimensionsLayoutRow(QVBoxLayout *dimensionsLayout, int index);
    void updateLayout();
    void saveNodeContent(YAML::Emitter *out);
    void saveValues(YAML::Emitter *out);

protected slots:
    void removeDimensionsLayoutRowRequested(bool clicked);
};

#endif // POLYNOMIALMATRIXNODE_H
