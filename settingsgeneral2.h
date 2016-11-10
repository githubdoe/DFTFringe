#ifndef SETTINGSGENERAL2_H
#define SETTINGSGENERAL2_H

#include <QDialog>

namespace Ui {
class SettingsGeneral2;
}

class SettingsGeneral2 : public QDialog
{
    Q_OBJECT

public:
    explicit SettingsGeneral2(QWidget *parent = 0);
    ~SettingsGeneral2();
    bool useSVD();
    bool showConditionNumbers();
    bool useStarTestMake(){
        return m_useStarTestMakeOnly;
    }
private slots:
    void on_checkBox_clicked(bool checked);
    void on_starTestMakeCb_clicked(bool checked);
    void on_showConditionNumbersCb_clicked(bool checked);

private:
    Ui::SettingsGeneral2 *ui;
    bool m_useSVD;
    bool m_showConditionNumbers;
    bool m_useStarTestMakeOnly;
};

#endif // SETTINGSGENERAL2_H
