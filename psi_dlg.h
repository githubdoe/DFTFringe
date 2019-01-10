#ifndef PSI_DLG_H
#define PSI_DLG_H

#include <QDialog>
#include "opencv/cv.h"
namespace Ui {
class PSI_dlg;
}

class PSI_dlg : public QDialog
{
    Q_OBJECT

public:
    explicit PSI_dlg(QWidget *parent = 0);
    ~PSI_dlg();
    QStringList files();
    cv::Mat phases();
    bool m_knownPhase;

private slots:
    void on_browse_clicked();

    void on_help_clicked();

    void on_clear_clicked();

    void on_PhaseKnown_clicked(bool checked);

    void on_PhaseApproximate_clicked(bool checked);

    void on_PSIPhaseValue_valueChanged(const QString &arg1);

private:
    Ui::PSI_dlg *ui;
};

#endif // PSI_DLG_H
