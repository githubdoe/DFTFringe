#ifndef CONTOURTOOLS_H
#define CONTOURTOOLS_H

#include <QDockWidget>
#include <QTimer>
namespace Ui {
class ContourTools;
}

class ContourTools : public QDockWidget
{
    Q_OBJECT

public:
    explicit ContourTools(QWidget *parent = 0);
    ~ContourTools();
    void connectTo(QWidget *view);
    double m_interval;
    void enablTools(bool b);
    double m_min;
    double m_max;
signals:
    void ContourMapColorChanged(int);
    void showContoursChanged(double);
    void contourZeroOffsetChanged(const QString &);
    void contourColorRangeChanged(const QString &);
    void contourIntervalChanged(double);
    void contourWaveRangeChanged(double);
    void newDisplayErrorRange(double, double);
    void lineColorChanged(QColor);
    void fillChanged(int );
private slots:
    void updateMinMax();
    void setMinMaxValues(double min,double max);
    void setWaveRange(double val);
    void on_ColorMapCB_activated(int index);

    void on_checkBox_stateChanged(int arg1);

    void on_zeroOffsetCB_activated(const QString &arg1);

    void on_colorRangeCB_activated(const QString &arg1);

    void on_contoursteps_editingFinished();

    void on_contoursteps_valueChanged(double arg1);

    void on_errorRangeSpin_valueChanged(double arg1);

    void on_LineColorBtn_clicked();

    void on_FilledContourChk_stateChanged(int arg1);

    void on_max_textEdited(const QString &);

    void on_newMin_textEdited(const QString &);

private:
    Ui::ContourTools *ui;
    QTimer *m_minmaxEditTimer;
};

#endif // CONTOURTOOLS_H
