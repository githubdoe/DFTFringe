#ifndef DFTTHUMB_H
#define DFTTHUMB_H

#include <QDialog>

namespace Ui {
class dftThumb;
}

class dftThumb : public QDialog
{
    Q_OBJECT

public:
    explicit dftThumb(QWidget *parent = 0);
    ~dftThumb();
void setImage(QImage img);
private:
    Ui::dftThumb *ui;
    double scale;
    QImage m_img;
};

#endif // DFTTHUMB_H
