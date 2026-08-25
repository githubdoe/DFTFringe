#ifndef JITTEROUTLINEDLG_H
#define JITTEROUTLINEDLG_H

#include <QDialog>
#include <QProgressBar>
namespace Ui {
class jitterOutlineDlg;
}

class jitterOutlineDlg : public QDialog
{
    Q_OBJECT

public:
    explicit jitterOutlineDlg(QWidget *parent = 0);
    QProgressBar *getProgressBar();

    ~jitterOutlineDlg();
    int getStep();
    int getStart();
    int getEnd();
    int getType();
    void status(const QString &status);

signals:
    void startRequested();
    void stopRequested();

private slots:

private:
    Ui::jitterOutlineDlg *ui;
};

#endif // JITTEROUTLINEDLG_H
