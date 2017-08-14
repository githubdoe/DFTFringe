#include "testplotclass.h"

testPlotClass::testPlotClass(QObject *parent) :QwtPlot()
{

}
void testPlotClass::t(QString m){
    emit waveSelected(m);
}
