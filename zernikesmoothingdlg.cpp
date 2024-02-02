#include "zernikesmoothingdlg.h"
#include "ui_zernikesmoothingdlg.h"
#include <QSettings>
#include "myutils.h"

ZernikeSmoothingDlg::ZernikeSmoothingDlg(wavefront &wf, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ZernikeSmoothingDlg)
{
    ui->setupUi(this);
    p_wf = &wf;

    m_zp = new zernikeProcess;
    m_wf = wf;
    theZerns = wf.InputZerns;
    m_noOfTerms = m_zp->getNumberOfTerms();
    m_zernEnables = std::vector<bool>(m_noOfTerms);
    tableModel = new zTableModel(parent, &m_zernEnables, true);
    std::vector<double> val(m_noOfTerms,0.);
    ui->zernView->setModel(tableModel);
    QSettings set;
    m_maxOrder = set.value("Zern maxOrder", 12).toInt();
    ui->maxOrder->setValue(m_maxOrder);
    ui->termCnt->setText(QString("%1 Terms").arg(m_noOfTerms));
    connect(&m_timer, SIGNAL(timeout()), this, SLOT(intiZernTable()));
    tableModel->setValues(&m_wf.InputZerns);
    m_sm = SurfaceManager::get_instance();


    m_originalSize = wf.data.cols;
    m_xCen = wf.m_outside.m_center.x();
    m_yCen = wf.m_outside.m_center.y();
    m_rad = wf.m_outside.m_radius;
}

ZernikeSmoothingDlg::~ZernikeSmoothingDlg()
{
    delete ui;
    delete m_zp;
    delete tableModel;
}

void ZernikeSmoothingDlg::intiZernTable(){
    QApplication::setOverrideCursor(Qt::WaitCursor);
    m_zp->initGrid(m_wf, m_maxOrder);
    QApplication::restoreOverrideCursor();
    tableModel->setValues(&theZerns);
}

void ZernikeSmoothingDlg::on_maxOrder_valueChanged(int arg1)
{
    if (arg1 % 2 != 0)
        ++arg1;
    ui->maxOrder->setValue( arg1);
    m_maxOrder = arg1;


    m_zp->setMaxOrder(arg1);
    m_noOfTerms = m_zp->getNumberOfTerms();
    ui->termCnt->setText(QString("%1 Terms").arg(m_noOfTerms));
    theZerns.resize(m_noOfTerms);
    //tableModel->resizeRows(m_noOfTerms);

   // init zp and update zern table view
    m_timer.setSingleShot(true);
    m_timer.start(1000);

}
cv::Mat makeSurfaceFromZerns(int width, zernikeProcess &zp, std::vector<double> theZerns){
    int wx = width;

    cv::Mat result = cv::Mat::zeros(wx,wx,  numType);

    for (unsigned long long i = 0; i < zp.m_zerns.n_rows; ++i){
        double S1 = 0.0;
        for (unsigned int z = 0; z < theZerns.size(); ++z){
            double val = theZerns[z];
             S1 +=  val * zp.m_zerns(i,z);
            int x =  zp.m_col[i];
            int y =  zp.m_row[i];

            if (S1 == 0.0) S1 += .0000001;
            //if (rho < .5) S1 = 0.;
            result.at<double>(y,x) = S1;
        }
    }
    return result;

}

void ZernikeSmoothingDlg::on_createWaveFront_clicked()
{
    QApplication::setOverrideCursor(Qt::WaitCursor);
    m_wf = *p_wf;
    if (!ui->useCurrentZernySet->isChecked()){
        theZerns = m_zp->ZernFitWavefront(m_wf);
        qDebug() << "the zerns";
        for (int z = 0; z < 8; ++z){
            qDebug() << z << theZerns[z];
        }
    }

   tableModel->setValues(&theZerns);

    cv::Mat result = makeSurfaceFromZerns(m_wf.data.cols, *m_zp, theZerns);

    QStringList l = m_wf.name.split("/");
    l.back().replace(".wft","");
    l.back().append(QString("_sm%1").arg(m_noOfTerms));
    m_sm->createSurfaceFromPhaseMap(result, m_wf.m_outside,
                                                CircleOutline(QPointF(0,0),0),l.back());

    if (ui->showResidual->isChecked()){
        m_sm->subtract(&m_wf, m_sm->m_wavefronts.back(), false);
        m_sm->m_wavefronts.back()->name = QString("Residual_%1").arg(m_noOfTerms);
        m_sm->m_surfaceTools->nameChanged(m_sm->m_currentNdx,  m_sm->m_wavefronts.back()->name);
        m_sm->m_surfaceTools->currentNdxChanged(m_sm->m_currentNdx);
        m_sm->previous();
        m_sm->m_surfaceTools->currentNdxChanged(m_sm->m_currentNdx);
    }
    QApplication::restoreOverrideCursor();
  }
