#ifndef AFFINEMAPNODE_H
#define AFFINEMAPNODE_H

#include "../nodebase.h"

class AffineMapNode : public NodeBase
{
    Q_OBJECT
public:
    AffineMapNode(QStringList* outputs = nullptr);
    AffineMapNode(QStringList* outputs, QList<double>* values);

private:
    void createLayout();
    void addNewDimensionsLayoutRow(QVBoxLayout* dimensionsLayout, int index);
    void removeDimensionsLayoutLastRow(QVBoxLayout* dimensionsLayout, int index);
    // QMap<QString, double>* getValues();

    void updateLayout();

private slots:
    void addDimensionsLayoutRowRequested(bool clicked);
    void removeDimensionsLayoutRowRequested(bool clicked);

protected:
    void saveNodeContent(YAML::Emitter* out);
    void saveValues(YAML::Emitter* out);
};

#endif // AFFINEMAPNODE_H
