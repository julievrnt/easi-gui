#ifndef COMPONENTSWIDGET_H
#define COMPONENTSWIDGET_H

#include <QWidget>
#include <QVBoxLayout>
#include "src/Nodes/node.h"
#include "src/Connectors/connectorwidget.h"

class ComponentsWidget : public QWidget
{
    Q_OBJECT
public:
    ComponentsWidget(Node* parentNode, QWidget* parent = nullptr);
    ~ComponentsWidget();

    QStringList* getOutputs() const;
    int getNumberComponents() const;

signals:
    void componentsChanged(int);

private:
    QStringList* outputs;
    QList<ConnectorWidget*> components;
    QVBoxLayout* connectorsLayout;

    void createLayout();
    void addComponent();
    bool deleteComponent();

private slots:
    void addButtonClicked(bool clicked);
    void deleteButtonClicked(bool clicked);

};

#endif // COMPONENTSWIDGET_H
