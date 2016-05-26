/********************************************************************************
** Form generated from reading UI file 'wftexaminer.ui'
**
** Created by: Qt User Interface Compiler version 5.3.2
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_WFTEXAMINER_H
#define UI_WFTEXAMINER_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QCheckBox>
#include <QtWidgets/QDialog>
#include <QtWidgets/QDialogButtonBox>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QSpinBox>

QT_BEGIN_NAMESPACE

class Ui_wftExaminer
{
public:
    QDialogButtonBox *buttonBox;
    QCheckBox *Datacb;
    QCheckBox *wrkDatacb;
    QSpinBox *spinBox;

    void setupUi(QDialog *wftExaminer)
    {
        if (wftExaminer->objectName().isEmpty())
            wftExaminer->setObjectName(QStringLiteral("wftExaminer"));
        wftExaminer->resize(400, 300);
        buttonBox = new QDialogButtonBox(wftExaminer);
        buttonBox->setObjectName(QStringLiteral("buttonBox"));
        buttonBox->setGeometry(QRect(30, 240, 341, 32));
        buttonBox->setOrientation(Qt::Horizontal);
        buttonBox->setStandardButtons(QDialogButtonBox::Cancel|QDialogButtonBox::Ok);
        Datacb = new QCheckBox(wftExaminer);
        Datacb->setObjectName(QStringLiteral("Datacb"));
        Datacb->setGeometry(QRect(30, 240, 70, 17));
        wrkDatacb = new QCheckBox(wftExaminer);
        wrkDatacb->setObjectName(QStringLiteral("wrkDatacb"));
        wrkDatacb->setGeometry(QRect(130, 240, 70, 17));
        spinBox = new QSpinBox(wftExaminer);
        spinBox->setObjectName(QStringLiteral("spinBox"));
        spinBox->setGeometry(QRect(121, 140, 211, 22));
        spinBox->setMaximum(700);
        spinBox->setValue(300);

        retranslateUi(wftExaminer);
        QObject::connect(buttonBox, SIGNAL(accepted()), wftExaminer, SLOT(accept()));
        QObject::connect(buttonBox, SIGNAL(rejected()), wftExaminer, SLOT(reject()));

        QMetaObject::connectSlotsByName(wftExaminer);
    } // setupUi

    void retranslateUi(QDialog *wftExaminer)
    {
        wftExaminer->setWindowTitle(QApplication::translate("wftExaminer", "Dialog", 0));
        Datacb->setText(QApplication::translate("wftExaminer", "Data", 0));
        wrkDatacb->setText(QApplication::translate("wftExaminer", "WrkData", 0));
    } // retranslateUi

};

namespace Ui {
    class wftExaminer: public Ui_wftExaminer {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_WFTEXAMINER_H
