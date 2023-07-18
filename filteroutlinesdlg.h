#ifndef FILTEROUTLINESDLG_H
#define FILTEROUTLINESDLG_H

#include <QDialog>

namespace Ui {
class FilterOutlinesDlg;
}
enum outlineFilterAction{oFLOAD,oFAVERAGE,oFDELETE, oFRESIZE};
enum outlineFilterOperation{FOLESSTHAN, FOEQUAL, FOGREATERTHAN, FOANY};
class FilterOutlinesDlg : public QDialog
{
    Q_OBJECT

public:
    outlineFilterAction actionType;
    outlineFilterOperation filterOperation;
    double radiusTarget;
    double resizeValue;
    explicit FilterOutlinesDlg(QWidget *parent = 0);
    ~FilterOutlinesDlg();
    void setRadius(double rad);

private slots:
    void on_actionLoad_clicked(bool checked);

    void on_actionAverae_clicked(bool checked);

    void on_actionDelete_clicked(bool checked);

    void on_actionResize_clicked(bool checked);

    void on_less_clicked(bool checked);

    void on_equal_clicked(bool checked);

    void on_greator_clicked(bool checked);

    void on_radiusValue_valueChanged(double arg1);

    void on_resizeValue_valueChanged(double arg1);

    void on_any_clicked(bool checked);

private:
    Ui::FilterOutlinesDlg *ui;
};

#endif // FILTEROUTLINESDLG_H
