/********************************************************************************
** Form generated from reading UI file 'standastigwizard.ui'
**
** Created by: Qt User Interface Compiler version 5.3.2
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_STANDASTIGWIZARD_H
#define UI_STANDASTIGWIZARD_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QWizard>

QT_BEGIN_NAMESPACE

class Ui_standAstigWizard
{
public:

    void setupUi(QWizard *standAstigWizard)
    {
        if (standAstigWizard->objectName().isEmpty())
            standAstigWizard->setObjectName(QStringLiteral("standAstigWizard"));
        standAstigWizard->resize(700, 600);
        QSizePolicy sizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(standAstigWizard->sizePolicy().hasHeightForWidth());
        standAstigWizard->setSizePolicy(sizePolicy);
        standAstigWizard->setMinimumSize(QSize(700, 400));
        standAstigWizard->setMaximumSize(QSize(524287, 524287));
        standAstigWizard->setWizardStyle(QWizard::ModernStyle);
        standAstigWizard->setOptions(QWizard::HaveHelpButton);

        retranslateUi(standAstigWizard);

        QMetaObject::connectSlotsByName(standAstigWizard);
    } // setupUi

    void retranslateUi(QWizard *standAstigWizard)
    {
        Q_UNUSED(standAstigWizard);
    } // retranslateUi

};

namespace Ui {
    class standAstigWizard: public Ui_standAstigWizard {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_STANDASTIGWIZARD_H
