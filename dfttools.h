#ifndef DFTTOOLS_H
#define DFTTOOLS_H

#include <QDockWidget>

namespace Ui {
class DFTTools;
}

class DFTTools : public QDockWidget
{
    Q_OBJECT

public:
    explicit DFTTools(QWidget *parent = 0);
    void connectTo(QWidget *view);
    ~DFTTools();
signals:
    void doDFT();
    void resizeIgram();
    void setFilter(double);
    void dftChannel(const QString&);
    void dftSizeChanged(const QString&);
    void dftSizeVal(int);
    void dftCenterFilter(double);
    void makeSurface();


private slots:

    void setCenterFilterValue(int);
    void on_colorChannelCB_activated(const QString &arg1);
    void on_filterAdjuster_valueChanged(double arg1);

    void on_pushButton_clicked();

    void on_computeDFT_pressed();

private:
    Ui::DFTTools *ui;
};

#endif // DFTTOOLS_H
