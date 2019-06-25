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
    explicit PSITiltOptions(QWidget *parent = 0);
    ~PSITiltOptions();

private:
    Ui::PSITiltOptions *ui;
};

#endif // PSITILTOPTIONS_H
