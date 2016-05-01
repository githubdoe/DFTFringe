#ifndef COLORCHANNELDISPLAY_H
#define COLORCHANNELDISPLAY_H

#include <QDialog>
#include <QImage>
namespace Ui {
class ColorChannelDisplay;
}

class ColorChannelDisplay : public QDialog
{
    Q_OBJECT


public:
    explicit ColorChannelDisplay(QWidget *parent = 0);
    ~ColorChannelDisplay();
    void setImage(QImage img);

private:
    Ui::ColorChannelDisplay *ui;
};

#endif // COLORCHANNELDISPLAY_H
