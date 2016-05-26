/********************************************************************************
** Form generated from reading UI file 'batchigramwizard.ui'
**
** Created by: Qt User Interface Compiler version 5.3.2
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_BATCHIGRAMWIZARD_H
#define UI_BATCHIGRAMWIZARD_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QWizard>

QT_BEGIN_NAMESPACE

class Ui_batchIgramWizard
{
public:

    void setupUi(QWizard *batchIgramWizard)
    {
        if (batchIgramWizard->objectName().isEmpty())
            batchIgramWizard->setObjectName(QStringLiteral("batchIgramWizard"));
        batchIgramWizard->resize(512, 408);
        batchIgramWizard->setMinimumSize(QSize(512, 300));
        batchIgramWizard->setWizardStyle(QWizard::ClassicStyle);

        retranslateUi(batchIgramWizard);

        QMetaObject::connectSlotsByName(batchIgramWizard);
    } // setupUi

    void retranslateUi(QWizard *batchIgramWizard)
    {
        batchIgramWizard->setWindowTitle(QApplication::translate("batchIgramWizard", "Batch process interferograms", 0));
    } // retranslateUi

};

namespace Ui {
    class batchIgramWizard: public Ui_batchIgramWizard {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_BATCHIGRAMWIZARD_H
