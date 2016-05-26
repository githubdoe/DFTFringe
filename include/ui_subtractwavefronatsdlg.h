/********************************************************************************
** Form generated from reading UI file 'subtractwavefronatsdlg.ui'
**
** Created by: Qt User Interface Compiler version 5.3.2
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_SUBTRACTWAVEFRONATSDLG_H
#define UI_SUBTRACTWAVEFRONATSDLG_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QDialog>
#include <QtWidgets/QDialogButtonBox>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLabel>
#include <QtWidgets/QListWidget>
#include <QtWidgets/QVBoxLayout>

QT_BEGIN_NAMESPACE

class Ui_subtractWavefronatsDlg
{
public:
    QVBoxLayout *verticalLayout;
    QLabel *label;
    QListWidget *listWidget;
    QDialogButtonBox *buttonBox;

    void setupUi(QDialog *subtractWavefronatsDlg)
    {
        if (subtractWavefronatsDlg->objectName().isEmpty())
            subtractWavefronatsDlg->setObjectName(QStringLiteral("subtractWavefronatsDlg"));
        subtractWavefronatsDlg->resize(487, 300);
        verticalLayout = new QVBoxLayout(subtractWavefronatsDlg);
        verticalLayout->setObjectName(QStringLiteral("verticalLayout"));
        label = new QLabel(subtractWavefronatsDlg);
        label->setObjectName(QStringLiteral("label"));
        label->setWordWrap(true);

        verticalLayout->addWidget(label);

        listWidget = new QListWidget(subtractWavefronatsDlg);
        listWidget->setObjectName(QStringLiteral("listWidget"));

        verticalLayout->addWidget(listWidget);

        buttonBox = new QDialogButtonBox(subtractWavefronatsDlg);
        buttonBox->setObjectName(QStringLiteral("buttonBox"));
        buttonBox->setOrientation(Qt::Horizontal);
        buttonBox->setStandardButtons(QDialogButtonBox::Cancel|QDialogButtonBox::Ok);

        verticalLayout->addWidget(buttonBox);


        retranslateUi(subtractWavefronatsDlg);
        QObject::connect(buttonBox, SIGNAL(accepted()), subtractWavefronatsDlg, SLOT(accept()));
        QObject::connect(buttonBox, SIGNAL(rejected()), subtractWavefronatsDlg, SLOT(reject()));

        QMetaObject::connectSlotsByName(subtractWavefronatsDlg);
    } // setupUi

    void retranslateUi(QDialog *subtractWavefronatsDlg)
    {
        subtractWavefronatsDlg->setWindowTitle(QApplication::translate("subtractWavefronatsDlg", "Subtract wavefronts", 0));
        label->setText(QApplication::translate("subtractWavefronatsDlg", "Select wavefront to be subtracted from the currently viewed wavefront.  Remember to disable the software null in the mirror configuration in order to view the result properly.", 0));
    } // retranslateUi

};

namespace Ui {
    class subtractWavefronatsDlg: public Ui_subtractWavefronatsDlg {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_SUBTRACTWAVEFRONATSDLG_H
