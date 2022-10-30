#ifndef ROOTNODEDIALOG_H
#define ROOTNODEDIALOG_H

#include <QDialog>
#include <QStringListModel>
#include <QListView>

class RootNodeDialog : public QDialog
{
    Q_OBJECT
public:
    RootNodeDialog(QStringList* outputs, QWidget* parent = nullptr);
    ~RootNodeDialog();

private:
    QStringList* outputs;
    QStringListModel* outputsModel;
    QListView* outputsListView;
    void createLayout();
    void addOutput();
    void editOutput();
    void deleteOutput();
    void saveOutputs();

private slots:
    void addOutputButtonClicked(bool clicked);
    void editOutputButtonClicked(bool clicked);
    void deleteOutputButtonClicked(bool clicked);
    void saveOutputsButtonClicked(bool clicked);
};

#endif // ROOTNODEDIALOG_H
