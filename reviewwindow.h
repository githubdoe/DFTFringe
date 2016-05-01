#ifndef REVIEWWINDOW_H
#define REVIEWWINDOW_H

#include <QWidget>
#include <QSplitter>
namespace Ui {
class reviewWindow;
}

class reviewWindow : public QWidget
{
    Q_OBJECT

public:
    explicit reviewWindow(QWidget *parent = 0);
    ~reviewWindow();
    QSplitter * s1;
    QSplitter * s2;


private:
    Ui::reviewWindow *ui;
};

#endif // REVIEWWINDOW_H
