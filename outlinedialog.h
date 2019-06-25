#ifndef OUTLINEDIALOG_H
#define OUTLINEDIALOG_H

#include <QDialog>

namespace Ui {
class outlineDialog;
}

class outlineDialog : public QDialog
{
    Q_OBJECT

public:
    explicit outlineDialog(QWidget *parent = 0);
    ~outlineDialog();

private:
    Ui::outlineDialog *ui;
};

#endif // OUTLINEDIALOG_H
