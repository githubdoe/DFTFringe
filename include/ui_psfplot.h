/********************************************************************************
** Form generated from reading UI file 'psfplot.ui'
**
** Created by: Qt User Interface Compiler version 5.3.2
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_PSFPLOT_H
#define UI_PSFPLOT_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_psfPlot
{
public:

    void setupUi(QWidget *psfPlot)
    {
        if (psfPlot->objectName().isEmpty())
            psfPlot->setObjectName(QStringLiteral("psfPlot"));
        psfPlot->resize(400, 300);

        retranslateUi(psfPlot);

        QMetaObject::connectSlotsByName(psfPlot);
    } // setupUi

    void retranslateUi(QWidget *psfPlot)
    {
        psfPlot->setWindowTitle(QApplication::translate("psfPlot", "Form", 0));
    } // retranslateUi

};

namespace Ui {
    class psfPlot: public Ui_psfPlot {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_PSFPLOT_H
