#ifndef JITTEROUTLINEDLG_H
#define JITTEROUTLINEDLG_H

#include <QDialog>
#include <QProgressBar>
class MainWindow;
namespace Ui {
class jitterOutlineDlg;
}

class jitterOutlineDlg : public QDialog
{
    Q_OBJECT

public:
    static jitterOutlineDlg *m_instance;
    explicit jitterOutlineDlg(QWidget *parent = 0);
    static jitterOutlineDlg *getInstance(MainWindow * mw);
    QProgressBar *getProgressBar();

    ~jitterOutlineDlg();
    int getStep();
    int getStart();
    int getEnd();
    int getType();
    void status(const QString &status);

private slots:

private:
    Ui::jitterOutlineDlg *ui;
};

#endif // JITTEROUTLINEDLG_H
