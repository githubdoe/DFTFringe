#include "zernikeeditdlg.h"
#include "ui_zernikeeditdlg.h"
#include <QFileDialog>
#include <fstream>
#include "zernikeprocess.h"
#include "zernikepolar.h"
#include "mirrordlg.h"
#include "myutils.h"
zernikeEditDlg::zernikeEditDlg(SurfaceManager * sfm, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::zernikeEditDlg), m_sm(sfm), shouldEnableAll(false)
{
    ui->setupUi(this);
    zernikeProcess &zp = *zernikeProcess::get_Instance();
    m_noOfTerms = zp.getNumberOfTerms();
    m_zernEnables = std::vector<bool>(m_noOfTerms);
    tableModel = new ZernTableModel(parent, &m_zernEnables, true);
    std::vector<double> val(m_noOfTerms,0.);
    ui->tableView->setModel(tableModel);
    QSettings set;
    m_maxOrder = set.value("Zern maxOrder", 10).toInt();
    ui->maxOrder->setValue(m_maxOrder);
    ui->numberOfTerms->setText(QString("%1 Terms").arg(m_noOfTerms));

}

zernikeEditDlg::~zernikeEditDlg()
{
    delete ui;
}

void zernikeEditDlg::on_disable_clicked()
{
    int start = 0;
    if (shouldEnableAll)
        start = 8;

    for (unsigned int i = start; i < m_zernEnables.size(); ++i)
        m_zernEnables[i] = shouldEnableAll;

    m_zernEnables[4] = shouldEnableAll;
    m_zernEnables[5] = shouldEnableAll;
    shouldEnableAll = !shouldEnableAll;
    ui->disable->setText((shouldEnableAll) ? "Enable All":"Disable All");
    tableModel->update();
}

void zernikeEditDlg::on_createSurface_clicked()
{
    int size = ui->sizeSb->value();
    cv::Mat result = cv::Mat::zeros(size,size, numType);

    double rho;
    double xcen = (size -1)/2.;
    double ycen = xcen;
    double rad = xcen - 1;
    for (int y = 0; y <  size; ++y)
    {
        double uy = (double)(y - (ycen)) / rad;
        for (int x = 0; x < size; ++x)
        {
            double ux = (double)(x - (xcen )) /rad;
            rho = sqrt(ux * ux + uy * uy);

            if (rho <= 1.)
            {
                double theta = atan2(uy,ux);
                zernikePolar zpolar(rho, theta, tableModel->rowCount());
                double s1 = 0;
                for (int z = 0; z < tableModel->rowCount(); ++z){
                    double v = tableModel->values[z];
                    if (m_zernEnables[z]){
                        s1 += v * zpolar.zernike(z);
                    }
                }
                result.at<double>(y,x) = s1;

            }
            else    // outside mirror outline
            {

                    result.at<double>(y,x) = 0 ;
                }
            }
        }

    m_sm->createSurfaceFromPhaseMap(result, CircleOutline(QPointF(xcen,ycen),rad),
                                                CircleOutline(QPointF(0,0),0),
                                                QString("Zernike_Wavefront"), wfZernikes);
}

void zernikeEditDlg::on_clearAll_clicked()
{
    tableModel->blockSignals(true);
    for (unsigned int i = 0; i < Z_TERMS; ++i){
            tableModel->values[i] =0.;
    }
    tableModel->update();
    tableModel->blockSignals(false);
}

void zernikeEditDlg::on_read_clicked()
{
    QSettings settings;
    QString lastPath = settings.value("lastPath",".").toString();
    QString fileName = QFileDialog::getOpenFileName(this,
                        tr("Read zernike file"), lastPath,
                        tr("Zernike File (*.zrn)"));
    if (fileName.isEmpty())
        return;

    std::ifstream infile(fileName.toStdString().c_str());
    std::vector<double> z;
    while (infile)
    {
        double v;
        infile >> v;
        z.push_back(v);
    }
    for (int i = z.size(); i < Z_TERMS; ++i){
        z.push_back(0.);
    }
    tableModel->setValues(z, false);
    tableModel->update();

}

void zernikeEditDlg::on_save_clicked()
{
    QSettings settings;
    QString lastPath = settings.value("lastPath",".").toString();
    QString fileName = QFileDialog::getSaveFileName(this,
                        tr("Save zernike file"), lastPath,
                        tr("Zernike File (*.zrn)"));
    if (fileName.isEmpty())
        return;

    std::ofstream ofile(fileName.toStdString().c_str());

    for (unsigned int i = 0; i < tableModel->values.size(); ++i){
        ofile << tableModel->values[i] << std::endl;
    }
    ofile.close();
}
extern std::vector<bool> zernEnables;
void zernikeEditDlg::on_useCurrent_clicked()
{
    if (m_sm->m_wavefronts.size() ==0 ) {
        QMessageBox::warning(0,"No wave fronts available!", "First load or create a wave front");
        return;
    }
    m_zernEnables = zernEnables;
    tableModel->blockSignals(true);

    tableModel->setValues(m_sm->m_wavefronts[m_sm->m_currentNdx]->InputZerns,m_sm->m_wavefronts[m_sm->m_currentNdx]->useSANull );
    ui->sizeSb->setValue(m_sm->m_wavefronts[m_sm->m_currentNdx]->data.cols);
    tableModel->blockSignals(false);
    tableModel->update();
}



void zernikeEditDlg::on_maxOrder_valueChanged(int arg1)
{
    QSettings set;

    if (arg1 % 2 != 0)
        ++arg1;
    ui->maxOrder->setValue( arg1);
    m_maxOrder = arg1;
    set.setValue("Zern maxOrder", arg1);
    zernikeProcess &zp = *zernikeProcess::get_Instance();
    zp.setMaxOrder(arg1);
    m_noOfTerms = zp.getNumberOfTerms();
    ui->numberOfTerms->setText(QString("%1 Terms").arg(m_noOfTerms));
    tableModel->resizeRows(m_noOfTerms);
    emit termCountChanged(m_noOfTerms);
}

