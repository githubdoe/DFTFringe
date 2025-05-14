#ifndef SETTINGSIGRAMIMPORTCONFIG_H
#define SETTINGSIGRAMIMPORTCONFIG_H

#include <QDialog>

namespace Ui {
class settingsIgramImportConfig;
}

class settingsIgramImportConfig : public QDialog
{
    Q_OBJECT

public:
    explicit settingsIgramImportConfig(QWidget *parent = nullptr);
    ~settingsIgramImportConfig();

private slots:

    void on_browse_clicked();

    void on_buttonBox_accepted();

private:
    Ui::settingsIgramImportConfig *ui;
};

#endif // SETTINGSIGRAMIMPORTCONFIG_H
