#ifndef SETTINGSIGRAM_H
#define SETTINGSIGRAM_H

#include <QDialog>

namespace Ui {
class settingsIGram;
}

class settingsIGram : public QDialog
{
    Q_OBJECT

public:
    explicit settingsIGram(QWidget *parent = 0);
    ~settingsIGram();
    bool shouldHFlipIgram();
signals:
    void igramLinesChanged(int,int,QColor,QColor, double, int);
private slots:
    void on_edgeLineColorPb_clicked();

    void on_spinBox_valueChanged(int arg1);

    void on_centerSpinBox_valueChanged(int arg1);

    void on_centerLineColorPb_clicked();

    void on_buttonBox_accepted();


    void on_hflipCb_clicked(bool checked);

private:
    Ui::settingsIGram *ui;
    QColor edgeColor;
    QColor centerColor;
    int edgeWidth;
    int centerWidth;
};

#endif // SETTINGSIGRAM_H
