#ifndef BATCHIGRAMPROCESSINGDLG_H
#define BATCHIGRAMPROCESSINGDLG_H

#include <QDialog>

namespace Ui {
class batchIgramProcessingDlg;
}

class batchIgramProcessingDlg : public QDialog
{
    Q_OBJECT

public:
    explicit batchIgramProcessingDlg(QWidget *parent = 0);
    ~batchIgramProcessingDlg();

private:
    Ui::batchIgramProcessingDlg *ui;
};

#endif // BATCHIGRAMPROCESSINGDLG_H
