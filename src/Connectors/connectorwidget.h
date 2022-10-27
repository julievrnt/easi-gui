#ifndef CONNECTORWIDGET_H
#define CONNECTORWIDGET_H

#include <QWidget>

class ConnectorWidget : public QWidget
{
    Q_OBJECT
public:
    explicit ConnectorWidget(QStringList* outputs = nullptr, QWidget *parent = nullptr);

    QStringList* getOutputs() const;

signals:

private:
    QStringList* outputs;

    // QWidget interface
protected:
    void paintEvent(QPaintEvent *event);
};

#endif // CONNECTORWIDGET_H
