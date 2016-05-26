/********************************************************************************
** Form generated from reading UI file 'simigramdlg.ui'
**
** Created by: Qt User Interface Compiler version 5.3.2
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_SIMIGRAMDLG_H
#define UI_SIMIGRAMDLG_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QDialog>
#include <QtWidgets/QDialogButtonBox>
#include <QtWidgets/QDoubleSpinBox>
#include <QtWidgets/QFormLayout>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLabel>
#include <QtWidgets/QSpinBox>
#include <QtWidgets/QVBoxLayout>

QT_BEGIN_NAMESPACE

class Ui_simIgramDlg
{
public:
    QVBoxLayout *verticalLayout;
    QFormLayout *formLayout;
    QDoubleSpinBox *correctionSB;
    QDoubleSpinBox *defocusSb;
    QDoubleSpinBox *xAstigSb;
    QDoubleSpinBox *yAstigSb;
    QDoubleSpinBox *starPatternSb;
    QDoubleSpinBox *ringPatterSb;
    QLabel *label;
    QLabel *label_2;
    QLabel *label_3;
    QLabel *label_4;
    QLabel *label_5;
    QLabel *label_6;
    QLabel *label_7;
    QLabel *label_8;
    QLabel *label_9;
    QDoubleSpinBox *noiseSb;
    QSpinBox *sizeSB;
    QLabel *label_10;
    QSpinBox *zernikeSb;
    QDoubleSpinBox *zernValue;
    QDoubleSpinBox *xTiltSb;
    QDoubleSpinBox *yTiltSb;
    QDialogButtonBox *buttonBox;

    void setupUi(QDialog *simIgramDlg)
    {
        if (simIgramDlg->objectName().isEmpty())
            simIgramDlg->setObjectName(QStringLiteral("simIgramDlg"));
        simIgramDlg->resize(215, 371);
        verticalLayout = new QVBoxLayout(simIgramDlg);
        verticalLayout->setObjectName(QStringLiteral("verticalLayout"));
        formLayout = new QFormLayout();
        formLayout->setObjectName(QStringLiteral("formLayout"));
        correctionSB = new QDoubleSpinBox(simIgramDlg);
        correctionSB->setObjectName(QStringLiteral("correctionSB"));
        correctionSB->setMinimum(-200);
        correctionSB->setMaximum(200);
        correctionSB->setSingleStep(1);
        correctionSB->setValue(100);

        formLayout->setWidget(4, QFormLayout::FieldRole, correctionSB);

        defocusSb = new QDoubleSpinBox(simIgramDlg);
        defocusSb->setObjectName(QStringLiteral("defocusSb"));
        defocusSb->setMinimum(-30);
        defocusSb->setMaximum(30);
        defocusSb->setValue(3.5);

        formLayout->setWidget(3, QFormLayout::FieldRole, defocusSb);

        xAstigSb = new QDoubleSpinBox(simIgramDlg);
        xAstigSb->setObjectName(QStringLiteral("xAstigSb"));
        xAstigSb->setDecimals(3);
        xAstigSb->setMinimum(-10);
        xAstigSb->setMaximum(10);

        formLayout->setWidget(5, QFormLayout::FieldRole, xAstigSb);

        yAstigSb = new QDoubleSpinBox(simIgramDlg);
        yAstigSb->setObjectName(QStringLiteral("yAstigSb"));
        yAstigSb->setDecimals(3);

        formLayout->setWidget(6, QFormLayout::FieldRole, yAstigSb);

        starPatternSb = new QDoubleSpinBox(simIgramDlg);
        starPatternSb->setObjectName(QStringLiteral("starPatternSb"));

        formLayout->setWidget(7, QFormLayout::FieldRole, starPatternSb);

        ringPatterSb = new QDoubleSpinBox(simIgramDlg);
        ringPatterSb->setObjectName(QStringLiteral("ringPatterSb"));

        formLayout->setWidget(8, QFormLayout::FieldRole, ringPatterSb);

        label = new QLabel(simIgramDlg);
        label->setObjectName(QStringLiteral("label"));

        formLayout->setWidget(1, QFormLayout::LabelRole, label);

        label_2 = new QLabel(simIgramDlg);
        label_2->setObjectName(QStringLiteral("label_2"));

        formLayout->setWidget(2, QFormLayout::LabelRole, label_2);

        label_3 = new QLabel(simIgramDlg);
        label_3->setObjectName(QStringLiteral("label_3"));

        formLayout->setWidget(3, QFormLayout::LabelRole, label_3);

        label_4 = new QLabel(simIgramDlg);
        label_4->setObjectName(QStringLiteral("label_4"));

        formLayout->setWidget(4, QFormLayout::LabelRole, label_4);

        label_5 = new QLabel(simIgramDlg);
        label_5->setObjectName(QStringLiteral("label_5"));

        formLayout->setWidget(5, QFormLayout::LabelRole, label_5);

        label_6 = new QLabel(simIgramDlg);
        label_6->setObjectName(QStringLiteral("label_6"));

        formLayout->setWidget(6, QFormLayout::LabelRole, label_6);

        label_7 = new QLabel(simIgramDlg);
        label_7->setObjectName(QStringLiteral("label_7"));

        formLayout->setWidget(7, QFormLayout::LabelRole, label_7);

        label_8 = new QLabel(simIgramDlg);
        label_8->setObjectName(QStringLiteral("label_8"));

        formLayout->setWidget(8, QFormLayout::LabelRole, label_8);

        label_9 = new QLabel(simIgramDlg);
        label_9->setObjectName(QStringLiteral("label_9"));

        formLayout->setWidget(9, QFormLayout::LabelRole, label_9);

        noiseSb = new QDoubleSpinBox(simIgramDlg);
        noiseSb->setObjectName(QStringLiteral("noiseSb"));

        formLayout->setWidget(9, QFormLayout::FieldRole, noiseSb);

        sizeSB = new QSpinBox(simIgramDlg);
        sizeSB->setObjectName(QStringLiteral("sizeSB"));
        sizeSB->setMinimum(2);
        sizeSB->setMaximum(2000);
        sizeSB->setSingleStep(0);
        sizeSB->setValue(800);

        formLayout->setWidget(0, QFormLayout::FieldRole, sizeSB);

        label_10 = new QLabel(simIgramDlg);
        label_10->setObjectName(QStringLiteral("label_10"));

        formLayout->setWidget(0, QFormLayout::LabelRole, label_10);

        zernikeSb = new QSpinBox(simIgramDlg);
        zernikeSb->setObjectName(QStringLiteral("zernikeSb"));
        zernikeSb->setMaximum(48);

        formLayout->setWidget(10, QFormLayout::LabelRole, zernikeSb);

        zernValue = new QDoubleSpinBox(simIgramDlg);
        zernValue->setObjectName(QStringLiteral("zernValue"));
        zernValue->setDecimals(3);

        formLayout->setWidget(10, QFormLayout::FieldRole, zernValue);

        xTiltSb = new QDoubleSpinBox(simIgramDlg);
        xTiltSb->setObjectName(QStringLiteral("xTiltSb"));
        xTiltSb->setMinimum(-99);

        formLayout->setWidget(1, QFormLayout::FieldRole, xTiltSb);

        yTiltSb = new QDoubleSpinBox(simIgramDlg);
        yTiltSb->setObjectName(QStringLiteral("yTiltSb"));

        formLayout->setWidget(2, QFormLayout::FieldRole, yTiltSb);


        verticalLayout->addLayout(formLayout);

        buttonBox = new QDialogButtonBox(simIgramDlg);
        buttonBox->setObjectName(QStringLiteral("buttonBox"));
        buttonBox->setOrientation(Qt::Horizontal);
        buttonBox->setStandardButtons(QDialogButtonBox::Cancel|QDialogButtonBox::Ok);

        verticalLayout->addWidget(buttonBox);


        retranslateUi(simIgramDlg);
        QObject::connect(buttonBox, SIGNAL(accepted()), simIgramDlg, SLOT(accept()));
        QObject::connect(buttonBox, SIGNAL(rejected()), simIgramDlg, SLOT(reject()));

        QMetaObject::connectSlotsByName(simIgramDlg);
    } // setupUi

    void retranslateUi(QDialog *simIgramDlg)
    {
        simIgramDlg->setWindowTitle(QApplication::translate("simIgramDlg", "Wavefront values", 0));
        label->setText(QApplication::translate("simIgramDlg", "x Tilt", 0));
        label_2->setText(QApplication::translate("simIgramDlg", "y Tilt", 0));
        label_3->setText(QApplication::translate("simIgramDlg", "Defocus", 0));
        label_4->setText(QApplication::translate("simIgramDlg", "Correction %", 0));
        label_5->setText(QApplication::translate("simIgramDlg", "X Astig", 0));
        label_6->setText(QApplication::translate("simIgramDlg", "Y Astig", 0));
        label_7->setText(QApplication::translate("simIgramDlg", "Star Pattern PV", 0));
        label_8->setText(QApplication::translate("simIgramDlg", "Ring Pattern PV", 0));
        label_9->setText(QApplication::translate("simIgramDlg", "Noise", 0));
        label_10->setText(QApplication::translate("simIgramDlg", "Size", 0));
    } // retranslateUi

};

namespace Ui {
    class simIgramDlg: public Ui_simIgramDlg {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_SIMIGRAMDLG_H
