/********************************************************************************
** Form generated from reading UI file 'prefsdlg.ui'
**
** Created by: Qt User Interface Compiler version 5.3.2
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_PREFSDLG_H
#define UI_PREFSDLG_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QDialog>
#include <QtWidgets/QDialogButtonBox>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QPushButton>

QT_BEGIN_NAMESPACE

class Ui_prefsDlg
{
public:
    QDialogButtonBox *buttonBox;
    QPushButton *pushButton;

    void setupUi(QDialog *prefsDlg)
    {
        if (prefsDlg->objectName().isEmpty())
            prefsDlg->setObjectName(QStringLiteral("prefsDlg"));
        prefsDlg->resize(400, 300);
        buttonBox = new QDialogButtonBox(prefsDlg);
        buttonBox->setObjectName(QStringLiteral("buttonBox"));
        buttonBox->setGeometry(QRect(170, 240, 201, 32));
        buttonBox->setOrientation(Qt::Horizontal);
        buttonBox->setStandardButtons(QDialogButtonBox::Cancel|QDialogButtonBox::Ok);
        pushButton = new QPushButton(prefsDlg);
        pushButton->setObjectName(QStringLiteral("pushButton"));
        pushButton->setGeometry(QRect(20, 250, 141, 41));

        retranslateUi(prefsDlg);
        QObject::connect(buttonBox, SIGNAL(accepted()), prefsDlg, SLOT(accept()));
        QObject::connect(buttonBox, SIGNAL(rejected()), prefsDlg, SLOT(reject()));

        QMetaObject::connectSlotsByName(prefsDlg);
    } // setupUi

    void retranslateUi(QDialog *prefsDlg)
    {
        prefsDlg->setWindowTitle(QApplication::translate("prefsDlg", "Dialog", 0));
        pushButton->setText(QApplication::translate("prefsDlg", "Save Current Window \n"
"state", 0));
    } // retranslateUi

};

namespace Ui {
    class prefsDlg: public Ui_prefsDlg {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_PREFSDLG_H
