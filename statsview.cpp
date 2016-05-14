#include "statsview.h"
#include "ui_statsview.h"
#include <QVBoxLayout>
statsView::statsView(QWidget *w1, QWidget *w2, QWidget *w3, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::statsView)
{
    ui->setupUi(this);

   QVBoxLayout *layout = new QVBoxLayout;

    m_vs = new QSplitter(ui->frame);

    m_vs->setOrientation(Qt::Vertical);
    m_vs->setHandleWidth(6);
    m_hs = new QSplitter();
    m_hs->addWidget(w1);
    m_hs->addWidget(w2);
    m_vs->setHandleWidth(6);
    m_vs->setStyleSheet("QSplitter::handle {background-color: lightblue; border: 2px solid #777; border-radius: 4px;}");
    m_hs->setStyleSheet("QSplitter::handle {background-color: lightblue; border: 2px solid #777; border-radius: 4px;}");

    m_hs->setOrientation(Qt::Horizontal);
    QList<int> sizes;
    w2->setMinimumWidth(200);
    sizes << 800;
    sizes << 200;
    m_hs->setSizes(sizes);
    m_vs->addWidget(m_hs);
    m_vs->addWidget(w3);

    layout->addWidget(m_vs);
    ui->frame->setLayout(layout);

}

statsView::~statsView()
{
    delete ui;
}



void statsView::on_showWftNames_toggled(bool checked)
{

}

void statsView::on_includeInputWavefronts_toggled(bool checked)
{

}

void statsView::on_removeOutliers_toggled(bool checked)
{

}
