#ifndef OGLRENDERED_H
#define OGLRENDERED_H

#include <QWidget>
#include <QLabel>
namespace Ui {
class oglRendered;
}

class oglRendered : public QWidget
{
    Q_OBJECT

public:
    explicit oglRendered(QWidget *parent = nullptr);
    ~oglRendered();
    QLabel *getModel();
    QLabel *getLegend();
public:
    Ui::oglRendered *ui;
};

#endif // OGLRENDERED_H
