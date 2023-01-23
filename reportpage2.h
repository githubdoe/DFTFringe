#ifndef REPORTPAGE2_H
#define REPORTPAGE2_H

#include <QWidget>
#include <QLabel>
namespace Ui {
class ReportPage2;
}

class ReportPage2 : public QWidget
{
    Q_OBJECT

public:
    explicit ReportPage2(QWidget *parent = nullptr);
    ~ReportPage2();
    QLabel *Surface();
    QLabel *Contour();
    QLabel *Profile();
private:
    Ui::ReportPage2 *ui;
};

#endif // REPORTPAGE2_H
