#ifndef SAVEWAVEDLG_H
#define SAVEWAVEDLG_H

#include <QDialog>

namespace Ui {
class SaveWaveDlg;
}

class SaveWaveDlg : public QDialog
{
    Q_OBJECT

public:
    explicit SaveWaveDlg(QString lastPath, QWidget *parent = 0);
    ~SaveWaveDlg();
    QString fileName;
    QString lastPath;

private slots:
    void on_normalRb_clicked(bool);

    void on_browsePB_clicked();

private:
    Ui::SaveWaveDlg *ui;
};

#endif // SAVEWAVEDLG_H
