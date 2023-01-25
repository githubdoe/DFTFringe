#ifndef PDFCALIBRATIONDLG_H
#define PDFCALIBRATIONDLG_H

#include <QDialog>

namespace Ui {
class PdfCalibrationDLg;
}

class PdfCalibrationDLg : public QDialog
{
    Q_OBJECT

public:
    int m_rightCrop;
    explicit PdfCalibrationDLg(QWidget *parent = nullptr);
    ~PdfCalibrationDLg();

private slots:
    void on_spinBox_valueChanged(int arg1);

    void on_RIghtSideNumber_editingFinished();

private:
    Ui::PdfCalibrationDLg *ui;
};

#endif // PDFCALIBRATIONDLG_H
