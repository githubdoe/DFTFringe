#ifndef OUTLINESTATSDLG_H
#define OUTLINESTATSDLG_H

#include <QDialog>
#include <QVector>
class outlineZoomer;
namespace Ui {
class outlineStatsDlg;
}

class outlineStatsDlg : public QDialog
{
    Q_OBJECT

public:
    explicit outlineStatsDlg(const QStringList &names, QWidget *parent = 0);
    ~outlineStatsDlg();
    QStringList m_names;
    QVector<double> xvals;
    QVector<double> yvals;
    double mostFrequentRadius;
private slots:
    void on_pushButton_clicked();

private:
    Ui::outlineStatsDlg *ui;

    QVector<double> radVals;
    QVector<double> sn;
    void readFiles();
    void plot();
    outlineZoomer * zoomer;
    outlineZoomer *radZoomer;
};

#endif // OUTLINESTATSDLG_H
