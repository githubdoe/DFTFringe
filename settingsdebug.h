#ifndef SETTINGSDEBUG_H
#define SETTINGSDEBUG_H

#include <QDialog>

namespace Ui {
class settingsDebug;
}

class settingsDebug : public QDialog
{
    Q_OBJECT

public:
    explicit settingsDebug(QWidget *parent = 0);
    ~settingsDebug();
    bool showMask();
private slots:
    void on_checkBox_clicked(bool checked);

private:
    Ui::settingsDebug *ui;
};

#endif // SETTINGSDEBUG_H
