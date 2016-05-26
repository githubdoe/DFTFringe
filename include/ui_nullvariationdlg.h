/********************************************************************************
** Form generated from reading UI file 'nullvariationdlg.ui'
**
** Created by: Qt User Interface Compiler version 5.3.2
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_NULLVARIATIONDLG_H
#define UI_NULLVARIATIONDLG_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QDialog>
#include <QtWidgets/QDialogButtonBox>
#include <QtWidgets/QDoubleSpinBox>
#include <QtWidgets/QGroupBox>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QRadioButton>

QT_BEGIN_NAMESPACE

class Ui_nullVariationDlg
{
public:
    QDialogButtonBox *buttonBox;
    QLabel *label;
    QRadioButton *useInch;
    QRadioButton *useMm;
    QLineEdit *roc;
    QLineEdit *diameter;
    QLabel *label_3;
    QLabel *label_4;
    QDoubleSpinBox *rocDelta;
    QDoubleSpinBox *diameterDelta;
    QGroupBox *groupBox;
    QLabel *label_2;
    QLineEdit *result;

    void setupUi(QDialog *nullVariationDlg)
    {
        if (nullVariationDlg->objectName().isEmpty())
            nullVariationDlg->setObjectName(QStringLiteral("nullVariationDlg"));
        nullVariationDlg->resize(590, 342);
        buttonBox = new QDialogButtonBox(nullVariationDlg);
        buttonBox->setObjectName(QStringLiteral("buttonBox"));
        buttonBox->setGeometry(QRect(-30, 310, 341, 32));
        buttonBox->setOrientation(Qt::Horizontal);
        buttonBox->setStandardButtons(QDialogButtonBox::Cancel|QDialogButtonBox::Ok);
        label = new QLabel(nullVariationDlg);
        label->setObjectName(QStringLiteral("label"));
        label->setGeometry(QRect(20, 20, 471, 101));
        label->setWordWrap(true);
        useInch = new QRadioButton(nullVariationDlg);
        useInch->setObjectName(QStringLiteral("useInch"));
        useInch->setGeometry(QRect(110, 110, 82, 17));
        useMm = new QRadioButton(nullVariationDlg);
        useMm->setObjectName(QStringLiteral("useMm"));
        useMm->setGeometry(QRect(250, 110, 82, 17));
        useMm->setChecked(true);
        roc = new QLineEdit(nullVariationDlg);
        roc->setObjectName(QStringLiteral("roc"));
        roc->setGeometry(QRect(20, 170, 113, 20));
        diameter = new QLineEdit(nullVariationDlg);
        diameter->setObjectName(QStringLiteral("diameter"));
        diameter->setGeometry(QRect(20, 250, 113, 20));
        label_3 = new QLabel(nullVariationDlg);
        label_3->setObjectName(QStringLiteral("label_3"));
        label_3->setGeometry(QRect(140, 170, 101, 16));
        label_4 = new QLabel(nullVariationDlg);
        label_4->setObjectName(QStringLiteral("label_4"));
        label_4->setGeometry(QRect(140, 250, 131, 16));
        rocDelta = new QDoubleSpinBox(nullVariationDlg);
        rocDelta->setObjectName(QStringLiteral("rocDelta"));
        rocDelta->setGeometry(QRect(260, 170, 62, 22));
        rocDelta->setDecimals(4);
        rocDelta->setSingleStep(0.5);
        rocDelta->setValue(1.5748);
        diameterDelta = new QDoubleSpinBox(nullVariationDlg);
        diameterDelta->setObjectName(QStringLiteral("diameterDelta"));
        diameterDelta->setGeometry(QRect(260, 250, 62, 22));
        diameterDelta->setDecimals(4);
        diameterDelta->setSingleStep(0.5);
        diameterDelta->setValue(1.5748);
        groupBox = new QGroupBox(nullVariationDlg);
        groupBox->setObjectName(QStringLiteral("groupBox"));
        groupBox->setGeometry(QRect(370, 160, 201, 111));
        label_2 = new QLabel(groupBox);
        label_2->setObjectName(QStringLiteral("label_2"));
        label_2->setGeometry(QRect(10, 10, 181, 71));
        label_2->setWordWrap(true);
        result = new QLineEdit(groupBox);
        result->setObjectName(QStringLiteral("result"));
        result->setGeometry(QRect(30, 80, 113, 20));
        result->setReadOnly(true);

        retranslateUi(nullVariationDlg);
        QObject::connect(buttonBox, SIGNAL(accepted()), nullVariationDlg, SLOT(accept()));
        QObject::connect(buttonBox, SIGNAL(rejected()), nullVariationDlg, SLOT(reject()));

        QMetaObject::connectSlotsByName(nullVariationDlg);
    } // setupUi

    void retranslateUi(QDialog *nullVariationDlg)
    {
        nullVariationDlg->setWindowTitle(QApplication::translate("nullVariationDlg", "Artificial Null error Margins", 0));
        label->setText(QApplication::translate("nullVariationDlg", "The Artificial Null accuracy depends on the  mirror diameter and ROC measurment  accuracy.  Below you can enter your measurment accuracy and this will calculate the PV difference based on the measurment uncertainty.\n"
"", 0));
        useInch->setText(QApplication::translate("nullVariationDlg", "inch", 0));
        useMm->setText(QApplication::translate("nullVariationDlg", "mm", 0));
        label_3->setText(QApplication::translate("nullVariationDlg", "ROC Tolerance +-", 0));
        label_4->setText(QApplication::translate("nullVariationDlg", "Diameter Tolerance +-", 0));
        diameterDelta->setPrefix(QApplication::translate("nullVariationDlg", "0", 0));
        groupBox->setTitle(QApplication::translate("nullVariationDlg", "Error margin", 0));
        label_2->setText(QApplication::translate("nullVariationDlg", "<html><head/><body><p>\302\261Null Uncertainty PV in waves of the measurment frequency.  This will affect the Z8 (spherical term) and the computed Concic constant  by  up to the amount shown below</p><p><br/></p></body></html>", 0));
    } // retranslateUi

};

namespace Ui {
    class nullVariationDlg: public Ui_nullVariationDlg {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_NULLVARIATIONDLG_H
