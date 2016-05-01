#ifndef SETTINGSPROFILE_H
#define SETTINGSPROFILE_H

#include <QDialog>

namespace Ui {
class settingsProfile;
}

class settingsProfile : public QDialog
{
    Q_OBJECT

public:
    explicit settingsProfile(QWidget *parent = 0);
    ~settingsProfile();
    void setColor(int num);
    QColor getColor(int num);
private slots:
    void on_pushButton_1_pressed();

    void on_pushButton_2_pressed();

    void on_pushButton_9_pressed();

    void on_pushButton_8_pressed();

    void on_pushButton_7_pressed();

    void on_pushButton_6_pressed();

    void on_pushButton_5_pressed();

    void on_pushButton_4_pressed();

    void on_pushButton_3_pressed();

private:
    Ui::settingsProfile *ui;
};

#endif // SETTINGSPROFILE_H
