#ifndef OUTLINETOOLS_H
#define OUTLINETOOLS_H

#include <QDockWidget>

namespace Ui {
class OutlineTools;
}

class OutlineTools : public QDockWidget
{
    Q_OBJECT

public:
    explicit OutlineTools(QWidget *parent = 0);
    ~OutlineTools();

private slots:

    void on_minus_clicked();

    void on_pluss_clicked();

private:
    Ui::OutlineTools *ui;
};

#endif // OUTLINETOOLS_H
