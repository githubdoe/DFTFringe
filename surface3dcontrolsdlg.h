#ifndef SURFACE3DCONTROLSDLG_H
#define SURFACE3DCONTROLSDLG_H

#include <QDialog>
#include "surfacegraph.h"

extern double maxHeightSelections[];
namespace Ui {
class Surface3dControlsDlg;
}

class Surface3dControlsDlg : public QDialog
{
    Q_OBJECT
    SurfaceGraph *m_surf;
public:
    explicit Surface3dControlsDlg(SurfaceGraph *surfP);
    ~Surface3dControlsDlg();
    void setYoffsetValue(double value);

private slots:
    void on_ambientSlider_valueChanged(int value);

    void on_lightXSlider_valueChanged(int value);

    void on_lightZSlider_valueChanged(int value);

    void on_lightYSlider_valueChanged(int value);

    void on_yScaleSlider_valueChanged(int value);

    void on_yOffsetSlider_valueChanged(int value);

    void on_yshiftAuto_clicked(bool checked);

    void on_range_valueChanged(int value);

    void on_defaultRange_clicked();

private:
    Ui::Surface3dControlsDlg *ui;
};

#endif // SURFACE3DCONTROLSDLG_H
