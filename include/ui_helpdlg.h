/********************************************************************************
** Form generated from reading UI file 'helpdlg.ui'
**
** Created by: Qt User Interface Compiler version 5.3.2
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_HELPDLG_H
#define UI_HELPDLG_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QDialog>
#include <QtWidgets/QDialogButtonBox>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QListWidget>

QT_BEGIN_NAMESPACE

class Ui_HelpDlg
{
public:
    QDialogButtonBox *buttonBox;
    QListWidget *movieList;

    void setupUi(QDialog *HelpDlg)
    {
        if (HelpDlg->objectName().isEmpty())
            HelpDlg->setObjectName(QStringLiteral("HelpDlg"));
        HelpDlg->resize(400, 300);
        buttonBox = new QDialogButtonBox(HelpDlg);
        buttonBox->setObjectName(QStringLiteral("buttonBox"));
        buttonBox->setGeometry(QRect(30, 240, 341, 32));
        buttonBox->setOrientation(Qt::Horizontal);
        buttonBox->setStandardButtons(QDialogButtonBox::Cancel|QDialogButtonBox::Ok);
        movieList = new QListWidget(HelpDlg);
        movieList->setObjectName(QStringLiteral("movieList"));
        movieList->setGeometry(QRect(10, 10, 381, 221));

        retranslateUi(HelpDlg);
        QObject::connect(buttonBox, SIGNAL(accepted()), HelpDlg, SLOT(accept()));
        QObject::connect(buttonBox, SIGNAL(rejected()), HelpDlg, SLOT(reject()));

        QMetaObject::connectSlotsByName(HelpDlg);
    } // setupUi

    void retranslateUi(QDialog *HelpDlg)
    {
        HelpDlg->setWindowTitle(QApplication::translate("HelpDlg", "Help", 0));
    } // retranslateUi

};

namespace Ui {
    class HelpDlg: public Ui_HelpDlg {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_HELPDLG_H
