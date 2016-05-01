#ifndef OGLCONTROLS_H
#define OGLCONTROLS_H

#include <QDockWidget>

namespace Ui {
class OGLControls;
}

class OGLControls : public QDockWidget
{
    Q_OBJECT

public:
    explicit OGLControls(QWidget *parent = 0);
    ~OGLControls();
    void enablTools(bool b);
signals:
    void resolutionChanged(int);
    void fillGridChanged(bool);
    void ogheightMagValue(int);
private slots:

    void on_doPerspective_clicked();

    void on_checkBox_clicked(bool checked);

    void on_spinBox_valueChanged(int arg1);

    void on_surfaceMagnification_valueChanged(int arg1);

private:
    Ui::OGLControls *ui;
};

#endif // OGLCONTROLS_H
