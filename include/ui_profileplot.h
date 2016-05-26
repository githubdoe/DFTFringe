/********************************************************************************
** Form generated from reading UI file 'profileplot.ui'
**
** Created by: Qt User Interface Compiler version 5.3.2
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_PROFILEPLOT_H
#define UI_PROFILEPLOT_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_ProfilePlot
{
public:

    void setupUi(QWidget *ProfilePlot)
    {
        if (ProfilePlot->objectName().isEmpty())
            ProfilePlot->setObjectName(QStringLiteral("ProfilePlot"));
        ProfilePlot->resize(209, 61);
        QSizePolicy sizePolicy(QSizePolicy::Ignored, QSizePolicy::Ignored);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(ProfilePlot->sizePolicy().hasHeightForWidth());
        ProfilePlot->setSizePolicy(sizePolicy);

        retranslateUi(ProfilePlot);

        QMetaObject::connectSlotsByName(ProfilePlot);
    } // setupUi

    void retranslateUi(QWidget *ProfilePlot)
    {
        ProfilePlot->setWindowTitle(QApplication::translate("ProfilePlot", "Form", 0));
    } // retranslateUi

};

namespace Ui {
    class ProfilePlot: public Ui_ProfilePlot {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_PROFILEPLOT_H
