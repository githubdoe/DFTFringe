/********************************************************************************
** Form generated from reading UI file 'renamewavefrontdlg.ui'
**
** Created by: Qt User Interface Compiler version 5.3.2
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_RENAMEWAVEFRONTDLG_H
#define UI_RENAMEWAVEFRONTDLG_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QDialog>
#include <QtWidgets/QDialogButtonBox>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QVBoxLayout>

QT_BEGIN_NAMESPACE

class Ui_renameWavefrontDlg
{
public:
    QVBoxLayout *verticalLayout;
    QLabel *label;
    QLineEdit *name;
    QDialogButtonBox *buttonBox;

    void setupUi(QDialog *renameWavefrontDlg)
    {
        if (renameWavefrontDlg->objectName().isEmpty())
            renameWavefrontDlg->setObjectName(QStringLiteral("renameWavefrontDlg"));
        renameWavefrontDlg->resize(400, 86);
        verticalLayout = new QVBoxLayout(renameWavefrontDlg);
        verticalLayout->setObjectName(QStringLiteral("verticalLayout"));
        label = new QLabel(renameWavefrontDlg);
        label->setObjectName(QStringLiteral("label"));

        verticalLayout->addWidget(label);

        name = new QLineEdit(renameWavefrontDlg);
        name->setObjectName(QStringLiteral("name"));

        verticalLayout->addWidget(name);

        buttonBox = new QDialogButtonBox(renameWavefrontDlg);
        buttonBox->setObjectName(QStringLiteral("buttonBox"));
        buttonBox->setOrientation(Qt::Horizontal);
        buttonBox->setStandardButtons(QDialogButtonBox::Cancel|QDialogButtonBox::Ok);

        verticalLayout->addWidget(buttonBox);


        retranslateUi(renameWavefrontDlg);
        QObject::connect(buttonBox, SIGNAL(accepted()), renameWavefrontDlg, SLOT(accept()));
        QObject::connect(buttonBox, SIGNAL(rejected()), renameWavefrontDlg, SLOT(reject()));

        QMetaObject::connectSlotsByName(renameWavefrontDlg);
    } // setupUi

    void retranslateUi(QDialog *renameWavefrontDlg)
    {
        renameWavefrontDlg->setWindowTitle(QApplication::translate("renameWavefrontDlg", "Rename Wavefront", 0));
        label->setText(QApplication::translate("renameWavefrontDlg", "Enter new wavefront name:", 0));
    } // retranslateUi

};

namespace Ui {
    class renameWavefrontDlg: public Ui_renameWavefrontDlg {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_RENAMEWAVEFRONTDLG_H
