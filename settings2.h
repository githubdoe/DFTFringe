#ifndef SETTINGS2_H
#define SETTINGS2_H

#include <QDialog>
#include "settingsigram.h"
#include "settingsdft.h"
#include "settingsdebug.h"
#include "settingsprofile.h"
namespace Ui {
class Settings2;
}

class Settings2 : public QDialog
{
    Q_OBJECT

public:
    explicit Settings2(QWidget *parent = 0);
    static Settings2 *getInstance();
    ~Settings2();
    static settingsIGram *m_igram;
    static settingsDFT *m_dft;
    static settingsDebug *m_debug;
    static settingsProfile *m_profile;
    static bool showDFT();
    static int dftSize();
    static bool showMask();
    static bool shouldHflipIgram();
signals:
    void igramOutlineParmsChanged(int,int,QColor,QColor);
private slots:
    void on_listWidget_clicked(const QModelIndex &index);

private:
    Ui::Settings2 *ui;
    static Settings2 * m_Instance;
};

#endif // SETTINGS2_H
