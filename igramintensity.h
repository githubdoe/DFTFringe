#ifndef IGRAMINTENSITY_H
#define IGRAMINTENSITY_H

#include <QDialog>
#include <QImage>
namespace Ui {
class igramIntensity;
}

class igramIntensity : public QDialog
{
    Q_OBJECT

public:
    explicit igramIntensity(QWidget *parent = 0);
    ~igramIntensity();
    void setIgram(QImage img);
private slots:
    void on_showRed_clicked(bool checked);

    void on_showBlue_clicked(bool checked);

    void on_showGreen_clicked(bool checked);

    void on_pushButton_clicked();

    void on_checkBox_clicked(bool checked);

private:
    Ui::igramIntensity *ui;

};

#endif // IGRAMINTENSITY_H
