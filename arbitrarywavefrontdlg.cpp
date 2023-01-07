#include "arbitrarywavefrontdlg.h"
#include <QtGui>
#include <QDebug>
#include "qboxlayout.h"
#include "arbitrarywavwidget.h"
#include "arbitrarywavefronthelp.h"
#include "qpushbutton.h"
#include <QLineEdit>



ArbitraryWavefrontDlg *ArbitraryWavefrontDlg::m_instance = 0;

ArbitraryWavefrontDlg::ArbitraryWavefrontDlg(QWidget *parent) :
    QDialog(parent)

{

    pww = new ArbitraryWavWidget(this);
    QHBoxLayout * layoutAll = new QHBoxLayout();
    QHBoxLayout * layoutSize = new QHBoxLayout();

    QVBoxLayout * layoutRight = new QVBoxLayout();
    QPushButton * butClose = new QPushButton();
    QPushButton * butOk = new QPushButton();
    QPushButton * butHelp = new QPushButton();

    QSpacerItem * spacer1 = new QSpacerItem(25,25);

    //QLabel * lblSize = new QLabel("Size:");
    m_txtSize = new QLineEdit("600");

    layoutRight->setAlignment(Qt::AlignTop);

    butHelp->setText("Help/Tips");
    layoutRight->addWidget(butHelp);

    layoutRight->addLayout(layoutSize);
    //layoutSize->addWidget(lblSize);
    //layoutSize->addWidget(m_txtSize);

    m_cmbMode = new QComboBox();
    m_cmbMode->addItem("Bezier",0);
    m_cmbMode->addItem("Cubic",1);
    m_cmbMode->setCurrentIndex(0);
    layoutRight->addWidget(m_cmbMode);
    m_cmbMode->setVisible(false);

    m_cmbUnit = new QComboBox();
    m_cmbUnit->addItem("inch",0);
    m_cmbUnit->addItem("cm",1);
    m_cmbUnit->addItem("mm",2);
    m_cmbUnit->setCurrentIndex(0); // default
    layoutRight->addWidget(m_cmbUnit);
    m_cmbUnit->show();

    layoutRight->addSpacerItem(spacer1);

    butOk->setText("Ok");
    layoutRight->addWidget(butOk);
    butOk->show();

    butClose->setText("Cancel");
    layoutRight->addWidget(butClose);
    butClose->show();

    pww->ww_unit = ArbitraryWavWidget::in; // default unit
    layoutAll->addWidget(pww,10);
    layoutAll->addLayout(layoutRight,1);
    QRect rec = QGuiApplication::screens()[0]->geometry();
    resize(rec.width()/2,rec.height()/2);
    pww->show();

    setLayout(layoutAll);

    setWindowTitle("Arbitrary Wavefront Simulation");

    connect(butOk, &QPushButton::pressed, this, &ArbitraryWavefrontDlg::onOkPressed );
    connect(butClose, &QPushButton::pressed, this, &ArbitraryWavefrontDlg::onCancelPressed );
    connect(butHelp, &QPushButton::pressed, this, &ArbitraryWavefrontDlg::onHelpPressed );
    connect(m_cmbMode, &QComboBox::currentTextChanged, this, &ArbitraryWavefrontDlg::onModeChange );
    connect(m_cmbUnit, &QComboBox::currentTextChanged, this, &ArbitraryWavefrontDlg::onUnitChange );

}

QSize ArbitraryWavefrontDlg::sizeHint() const {
    return QSize(800,400);
}

int ArbitraryWavefrontDlg::getSize() {
    return atoi( m_txtSize->text().toStdString().c_str());
}
    
ArbitraryWavefrontDlg* ArbitraryWavefrontDlg::get_instance(){
    if (m_instance == 0){
        m_instance = new ArbitraryWavefrontDlg();
    }
    return m_instance;
}    


void ArbitraryWavefrontDlg::showEvent(QShowEvent * event) {
    QDialog::showEvent(event);
/*
    qDebug("show event.  layout: ");
    QLayout * pLayout = this->layout();
    qDebug() << pLayout; // is null
    qDebug(" event: ");
    qDebug() << event->type();
    qDebug() << "children: " << children().size();
    */
}

ArbitraryWavefrontDlg::~ArbitraryWavefrontDlg()
{

}

void ArbitraryWavefrontDlg::onCancelPressed() {
    bOkPressed=false;
    close();
}

void ArbitraryWavefrontDlg::onOkPressed() {
    bOkPressed=true;
    //prepare(1000); // test
    //pww->showPrepare(); // test
    close();
}

void ArbitraryWavefrontDlg::onHelpPressed() {
    ArbitraryWavefrontHelp dlg;
    dlg.setModal(true);
    dlg.exec();
}



void ArbitraryWavefrontDlg::onModeChange() {
    pww->setMode(m_cmbMode->currentIndex());
}

void ArbitraryWavefrontDlg::onUnitChange() {
    int unit = m_cmbUnit->currentIndex();
    switch (unit) {
    case 0:
        pww->ww_unit = ArbitraryWavWidget::in;
        break;
    case 1:
        pww->ww_unit = ArbitraryWavWidget::cm;
        break;
    case 2:
    default:
        pww->ww_unit = ArbitraryWavWidget::mm;
        break;
    }
    pww->repaint();
}

void ArbitraryWavefrontDlg::setNull(double _mirror_null){
    pww->mirror_null = _mirror_null;
}
void ArbitraryWavefrontDlg::setDiameter(double _mirror_diam){
    pww->setRadius(_mirror_diam/2);
}

void ArbitraryWavefrontDlg::prepare(int size) {
    pww->prepare(size);
}
double ArbitraryWavefrontDlg::getValue(double rho) {
    return pww->getValue(rho);
}
