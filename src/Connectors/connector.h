#ifndef CONNECTORWIDGET_H
#define CONNECTORWIDGET_H

#include <QWidget>

class Connector : public QWidget
{
    Q_OBJECT
public:
    explicit Connector(QStringList* outputs = nullptr, QWidget* parent = nullptr);
    ~Connector();
    QStringList* getOutputs() const;

signals:

private:
    QStringList* outputs;

    // QWidget interface
protected:
    void paintEvent(QPaintEvent* event);
};

#endif // CONNECTORWIDGET_H
