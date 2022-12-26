#include "arbitrarywavefrontdlg.h"
#include "ui_arbitrarywavefrontdlg.h"
#include <QtGui>
#include <QDebug>
#include "qboxlayout.h"
#include "arbitrarywavwidget.h"
#include "arbitrarywavefronthelp.h"
#include "qpushbutton.h"
#include <QLineEdit>



ArbitraryWavefrontDlg::ArbitraryWavefrontDlg(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ArbitraryWavefrontDlg)

{

    pww = new ArbitraryWavWidget(this);
    QHBoxLayout * layoutAll = new QHBoxLayout();
    QHBoxLayout * layoutSize = new QHBoxLayout();

    QVBoxLayout * layoutRight = new QVBoxLayout();
    QPushButton * butClose = new QPushButton();
    QPushButton * butOk = new QPushButton();
    QPushButton * butHelp = new QPushButton();

    QSpacerItem * spacer1 = new QSpacerItem(25,25);

    QLabel * lblSize = new QLabel("Size:");
    QLineEdit * txtSize = new QLineEdit("600");

    layoutRight->setAlignment(Qt::AlignTop);

    butHelp->setText("Help/Tips");
    layoutRight->addWidget(butHelp);

    layoutRight->addLayout(layoutSize);
    layoutSize->addWidget(lblSize);
    layoutSize->addWidget(txtSize);

    cmbMode = new QComboBox();
    cmbMode->addItem("Bezier",0);
    cmbMode->addItem("Cubic",1);
    cmbMode->setCurrentIndex(0);
    layoutRight->addWidget(cmbMode);
    cmbMode->setVisible(false);

    cmbUnit = new QComboBox();
    cmbUnit->addItem("inch",0);
    cmbUnit->addItem("cm",1);
    cmbUnit->addItem("mm",2);
    cmbUnit->setCurrentIndex(0); // default
    layoutRight->addWidget(cmbUnit);
    cmbUnit->show();

    layoutRight->addSpacerItem(spacer1);

    butOk->setText("Ok");
    layoutRight->addWidget(butOk);
    butOk->show();

    butClose->setText("Cancel");
    layoutRight->addWidget(butClose);
    butClose->show();

    pww->ww_unit = ArbitraryWavWidget::in; // default unit
    layoutAll->addWidget(pww);
    layoutAll->addLayout(layoutRight);
    pww->show();

    layoutAll->setSizeConstraint(QLayout::SetMinimumSize);

    setLayout(layoutAll);
    setMinimumSize(800,300); // does nothing!

    setWindowTitle("Arbitrary Wavefront Simulation");

    connect(butOk, &QPushButton::pressed, this, &ArbitraryWavefrontDlg::onOkPressed );
    connect(butClose, &QPushButton::pressed, this, &ArbitraryWavefrontDlg::onCancelPressed );
    connect(butHelp, &QPushButton::pressed, this, &ArbitraryWavefrontDlg::onHelpPressed );
    connect(cmbMode, &QComboBox::currentTextChanged, this, &ArbitraryWavefrontDlg::onModeChange );
    connect(cmbUnit, &QComboBox::currentTextChanged, this, &ArbitraryWavefrontDlg::onUnitChange );

}

QSize ArbitraryWavefrontDlg::sizeHint() const {
    return QSize(800,400);
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
    delete ui;
}

void ArbitraryWavefrontDlg::onCancelPressed() {
    bOkPressed=false;
    close();
}

void ArbitraryWavefrontDlg::onOkPressed() {
    bOkPressed=true;
    close();
}

void ArbitraryWavefrontDlg::onHelpPressed() {
    ArbitraryWavefrontHelp dlg;
    dlg.setModal(true);
    dlg.exec();
}



void ArbitraryWavefrontDlg::onModeChange() {
    pww->setMode(cmbMode->currentIndex());
}

void ArbitraryWavefrontDlg::onUnitChange() {
    int unit = cmbUnit->currentIndex();
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
    pww->mirror_radius = _mirror_diam/2;
}
