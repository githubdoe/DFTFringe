#ifndef PSITILTOPTIONS_H
#define PSITILTOPTIONS_H

#include <QDialog>

namespace Ui {
class PSITiltOptions;
}

class PSITiltOptions : public QDialog
{
    Q_OBJECT

public:
    int m_maxVaryndx;
    explicit PSITiltOptions(QWidget *parent = 0);
    ~PSITiltOptions();

private slots:
    void on_zernLimits_activated(int index);

private:
    Ui::PSITiltOptions *ui;
};

#endif // PSITILTOPTIONS_H
