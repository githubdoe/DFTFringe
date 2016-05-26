/********************************************************************************
** Form generated from reading UI file 'settingsdft.ui'
**
** Created by: Qt User Interface Compiler version 5.3.2
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_SETTINGSDFT_H
#define UI_SETTINGSDFT_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QCheckBox>
#include <QtWidgets/QDialog>
#include <QtWidgets/QDialogButtonBox>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLabel>
#include <QtWidgets/QSpinBox>

QT_BEGIN_NAMESPACE

class Ui_settingsDFT
{
public:
    QDialogButtonBox *buttonBox;
    QCheckBox *ShowDFTTHumbCB;
    QLabel *label;
    QSpinBox *DFTSizeSB;

    void setupUi(QDialog *settingsDFT)
    {
        if (settingsDFT->objectName().isEmpty())
            settingsDFT->setObjectName(QStringLiteral("settingsDFT"));
        settingsDFT->resize(371, 156);
        buttonBox = new QDialogButtonBox(settingsDFT);
        buttonBox->setObjectName(QStringLiteral("buttonBox"));
        buttonBox->setGeometry(QRect(-20, 120, 181, 32));
        buttonBox->setOrientation(Qt::Horizontal);
        buttonBox->setStandardButtons(QDialogButtonBox::Ok);
        ShowDFTTHumbCB = new QCheckBox(settingsDFT);
        ShowDFTTHumbCB->setObjectName(QStringLiteral("ShowDFTTHumbCB"));
        ShowDFTTHumbCB->setGeometry(QRect(10, 40, 281, 31));
        label = new QLabel(settingsDFT);
        label->setObjectName(QStringLiteral("label"));
        label->setGeometry(QRect(10, 70, 251, 20));
        DFTSizeSB = new QSpinBox(settingsDFT);
        DFTSizeSB->setObjectName(QStringLiteral("DFTSizeSB"));
        DFTSizeSB->setGeometry(QRect(270, 70, 71, 22));
        DFTSizeSB->setMinimum(100);
        DFTSizeSB->setMaximum(2000);
        DFTSizeSB->setValue(640);

        retranslateUi(settingsDFT);
        QObject::connect(buttonBox, SIGNAL(accepted()), settingsDFT, SLOT(accept()));
        QObject::connect(buttonBox, SIGNAL(rejected()), settingsDFT, SLOT(reject()));

        QMetaObject::connectSlotsByName(settingsDFT);
    } // setupUi

    void retranslateUi(QDialog *settingsDFT)
    {
        settingsDFT->setWindowTitle(QApplication::translate("settingsDFT", "Dialog", 0));
        ShowDFTTHumbCB->setText(QApplication::translate("settingsDFT", "Show DFT Thumnail once Mirror is outlined in Igram.", 0));
        label->setText(QApplication::translate("settingsDFT", "Resize mirror portion of igram  down to DFT size of:", 0));
#ifndef QT_NO_TOOLTIP
        DFTSizeSB->setToolTip(QApplication::translate("settingsDFT", "Width and Height of DFT in Pixels", 0));
#endif // QT_NO_TOOLTIP
#ifndef QT_NO_STATUSTIP
        DFTSizeSB->setStatusTip(QString());
#endif // QT_NO_STATUSTIP
    } // retranslateUi

};

namespace Ui {
    class settingsDFT: public Ui_settingsDFT {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_SETTINGSDFT_H
