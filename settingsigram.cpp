/******************************************************************************
**
**  Copyright 2016 Dale Eason
**  This file is part of DFTFringe
**  is free software: you can redistribute it and/or modify
** it under the terms of the GNU General Public License as published by
** the Free Software Foundation version 3 of the License

** DFTFringe is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
** GNU General Public License for more details.
**
** You should have received a copy of the GNU General Public License
** along with DFTFringe.  If not, see <http://www.gnu.org/licenses/>.

****************************************************************************/
#include "settingsigram.h"
#include "ui_settingsigram.h"
#include <QSettings>
#include <QColorDialog>
#include <QPen>
#include <QPainter>
#include <QMenu>
#include "spdlog/spdlog.h"
#include "settingsigramimportconfig.h"
static inline QString colorButtonStyleSheet(const QColor &bgColor)
{
    if (bgColor.isValid()) {
        QString rc = QLatin1String("border: 2px solid black; border-radius: 2px; background:");
        rc += bgColor.name();
        return rc;
    }
    return QLatin1String("border: 2px dotted black; border-radius: 2px;");
}
settingsIGram::settingsIGram(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::settingsIGram)
{
    ui->setupUi(this);
    QSettings set;
    edgeColor = QColor(set.value("igramEdgeLineColor", "white").toString());
    m_autoSaveOutline = set.value("AutoSaveOutline", true).toBool();
    ui->SaveOutlines->setChecked(m_autoSaveOutline);
    ui->edgeLineColorPb->setStyleSheet(colorButtonStyleSheet(edgeColor));
    centerColor = QColor(set.value("igramCenterLineColor", "yellow").toString());
    ui->centerLineColorPb->setStyleSheet(colorButtonStyleSheet(centerColor));
    ui->opacitySB->setValue(set.value("igramLineOpacity", 65.).toDouble());
    edgeWidth = set.value("igramEdgeWidth",5).toInt();
    centerWidth = set.value("igramCenterWidth",5).toInt();
    ui->spinBox->setValue(edgeWidth);
    ui->centerSpinBox->setValue(centerWidth);
    ui->zoomBoxWidthSb->setValue(set.value("igramZoomBoxWidth", 200).toDouble());

    connect(ui->buttonBox->button(QDialogButtonBox::Apply), SIGNAL(clicked()), SLOT(on_buttonBox_accepted()));
    ui->styleCB->setEditable(false);
    ui->styleCB->setIconSize(QSize(80,14));
    ui->styleCB->setMinimumWidth(80);
    ui->outsideX->setValue(set.value("autoOutlineXOffset", 0).toInt());
    ui->outsideY->setValue(set.value("autoOutlineYOffset", 0).toInt());
    ui->mirrorRadiusSB->setValue(set.value("autoOutlineRadOffset", 0).toInt());
    ui->holeX->setValue(set.value("autoholeXOffset", 0).toInt());
    ui->holeY->setValue(set.value("autoholeYOffset", 0).toInt());
    ui->holeRadius->setValue(set.value("autoHoleRadOffset",0).toInt());
    for (int aaa = Qt::SolidLine; aaa < Qt::CustomDashLine; aaa++)
    {
        QPixmap pix(80,14);
        pix.fill(Qt::white);

        QPen pen(Qt::black ,2.5,(Qt::PenStyle)aaa);

        QPainter painter(&pix);
        painter.setPen(pen);
        painter.drawLine(2,7,78,7);

        ui->styleCB->addItem(QIcon(pix),"");
    }
    int style = set.value("igramLineStyle", 1).toInt();
    ui->styleCB->setCurrentIndex(style-1);
    m_removeDistortion = set.value("removeLensDistortion", false).toBool();
    ui->removeDistortion->setChecked(m_removeDistortion);
    lensesModel = new LenseTableModel(this);
    QStringList lenses = set.value("Lenses", "").toString().split("\n");
    m_lenseParms = set.value("currentLense","").toString().split(",");

    int currentLensNdx = -1;
    m_lensData.clear();
    int ndx = 0;
    foreach(QString l, lenses){
        if (l == "")
            continue;
        m_lensData.push_back(l.split(","));
        if (m_lensData.back()[0] == m_lenseParms[0])
            currentLensNdx = ndx;
        ++ndx;
    }
    lensesModel->setLensData(&m_lensData);
    ui->lenseTableView->setModel(lensesModel);
    m_lenseParms = set.value("currentLense","").toString().split(",");
    if (!m_removeDistortion)
        ui->lenseTableView->hide();
    else {
        ui->currentlense->setText(m_lenseParms[0]);
    }
    ui->lenseTableView->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(ui->lenseTableView, SIGNAL(customContextMenuRequested(QPoint)), this, SLOT(showContextMenu(QPoint)));
    ui->lenseTableView->selectRow(currentLensNdx);
    ui->lenseTableView->horizontalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);
    selectionModel = ui->lenseTableView->selectionModel();

    QPalette palette;// = new QPalette();
    palette.setColor(QPalette::Highlight,"lightCyan");
    palette.setColor(QPalette::HighlightedText,"black");
    ui->lenseTableView->setPalette(palette);
}

settingsIGram::~settingsIGram()
{
    spdlog::get("logger")->trace("settingsIGram::~settingsIGram");
    delete lensesModel;
    delete ui;
}


void settingsIGram::on_edgeLineColorPb_clicked()
{
    edgeColor = QColorDialog::getColor(edgeColor, 0);
    ui->edgeLineColorPb->setStyleSheet(colorButtonStyleSheet(edgeColor));
}

void settingsIGram::on_spinBox_valueChanged(int arg1)
{
    edgeWidth = arg1;
}

void settingsIGram::on_centerSpinBox_valueChanged(int arg1)
{
    centerWidth = arg1;
}

void settingsIGram::on_centerLineColorPb_clicked()
{
    centerColor = QColorDialog::getColor(centerColor, 0);
    ui->centerLineColorPb->setStyleSheet(colorButtonStyleSheet(centerColor));
}

void settingsIGram::on_buttonBox_accepted()
{
    QSettings set;
    set.setValue("igramCenterLineColor", centerColor.name());
    set.setValue("igramEdgeLineColor", edgeColor.name());
    set.setValue("igramEdgeWidth", edgeWidth);
    set.setValue("igramCenterWidth", centerWidth);
    set.setValue("igramLineOpacity", ui->opacitySB->value());
    set.setValue("igramLineStyle", ui->styleCB->currentIndex() + 1);
    set.setValue("igramZoomBoxWidth", ui->zoomBoxWidthSb->value());
    set.setValue("AutoSaveOutline", m_autoSaveOutline);

    emit igramLinesChanged(outlineParms(edgeWidth,centerWidth, edgeColor, centerColor, ui->opacitySB->value(),
                           ui->styleCB->currentIndex()+1, ui->zoomBoxWidthSb->value(),m_autoSaveOutline));
}

void settingsIGram::on_removeDistortion_clicked(bool checked)
{
    m_removeDistortion = checked;
    QSettings set;
    set.setValue("removeLensDistortion", checked);
    if (checked){
        ui->lenseTableView->show();
        QSettings set;

    }
    else {
        ui->lenseTableView->hide();
    }
}


#include <QDebug>

void settingsIGram::eraseItem()
{
    qDebug() << "erasing" << currentNdx;
    lensesModel->removeRow(currentNdx.row());
    saveLensData();
}
void settingsIGram::showContextMenu(const QPoint &pos)
{
    // Handle global position
    QPoint globalPos = ui->lenseTableView->mapToGlobal(pos);

    // Create menu and insert some actions
    QMenu myMenu;
    myMenu.addAction("Erase",  this, SLOT(eraseItem()));

    // Show context menu at handling position
    myMenu.exec(globalPos);
}
void settingsIGram::saveLensData(){
    QSettings set;
    QStringList v;
    foreach(QStringList l, m_lensData){
        v.push_back(l.join(","));
    }
    set.setValue("Lenses", v.join("\n"));
}

void settingsIGram::updateLenses(QString str){
    m_lensData.push_back(str.split(","));
    saveLensData();
    lensesModel->insertRow(m_lensData.size() -2);

}

void settingsIGram::on_lenseTableView_clicked(const QModelIndex &index)
{
    currentNdx = index;
    const QAbstractItemModel * model = index.model();
    ui->currentlense->setText(model->data(model->index(index.row(), 0, index.parent()), Qt::DisplayRole).toString());
    m_lenseParms = m_lensData[index.row()];
    lensesModel->setCurrentRow(index.row());

    ui->lenseTableView->selectionModel()->select( index,
                      QItemSelectionModel::ClearAndSelect|QItemSelectionModel::Rows);
    QSettings set;
    set.setValue("currentLense", m_lensData[index.row()].join(","));
}


void settingsIGram::on_SaveOutlines_clicked(bool checked)
{
    m_autoSaveOutline = checked;
    QSettings set;
    set.setValue("AutoSaveOutline", m_autoSaveOutline);
}



void settingsIGram::on_mirrorRadiusSB_valueChanged(int arg1)
{
    QSettings set;
    set.setValue("autoOutlineRadOffset", arg1);
}

void settingsIGram::on_outsideX_valueChanged(int arg1)
{
    QSettings set;
    set.setValue("autoOutlineXOffset", arg1);
}

void settingsIGram::on_outsideY_valueChanged(int arg1)
{
    QSettings set;
    set.setValue("autoOutlineYOffset", arg1);
}

void settingsIGram::on_holeRadius_valueChanged(int arg1)
{
    QSettings set;
    set.setValue("autoHoleRadOffset", arg1);
}

void settingsIGram::on_holeX_valueChanged(int arg1)
{
    QSettings set;
    set.setValue("autoholeXOffset", arg1);
}

void settingsIGram::on_holeY_valueChanged(int arg1)
{
    QSettings set;
    set.setValue("autoholeYOffset", arg1);
}

void settingsIGram::on_importConfig_clicked()
{
    // set the path to igrams that "I key" should look at
    // set select most recent
    // set just open path
    // set open camera stream

    settingsIgramImportConfig dlg;
    dlg.exec();
}

