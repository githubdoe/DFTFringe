/********************************************************************************
** Form generated from reading UI file 'profilearea.ui'
**
** Created by: Qt User Interface Compiler version 5.3.2
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_PROFILEAREA_H
#define UI_PROFILEAREA_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_profileArea
{
public:

    void setupUi(QWidget *profileArea)
    {
        if (profileArea->objectName().isEmpty())
            profileArea->setObjectName(QStringLiteral("profileArea"));
        profileArea->resize(400, 300);

        retranslateUi(profileArea);

        QMetaObject::connectSlotsByName(profileArea);
    } // setupUi

    void retranslateUi(QWidget *profileArea)
    {
        profileArea->setWindowTitle(QApplication::translate("profileArea", "Form", 0));
    } // retranslateUi

};

namespace Ui {
    class profileArea: public Ui_profileArea {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_PROFILEAREA_H
