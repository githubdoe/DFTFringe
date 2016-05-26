/********************************************************************************
** Form generated from reading UI file 'dftarea.ui'
**
** Created by: Qt User Interface Compiler version 5.3.2
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_DFTAREA_H
#define UI_DFTAREA_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_DFTArea
{
public:
    QVBoxLayout *verticalLayout;

    void setupUi(QWidget *DFTArea)
    {
        if (DFTArea->objectName().isEmpty())
            DFTArea->setObjectName(QStringLiteral("DFTArea"));
        DFTArea->resize(800, 800);
        QSizePolicy sizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);
        sizePolicy.setHorizontalStretch(1);
        sizePolicy.setVerticalStretch(1);
        sizePolicy.setHeightForWidth(DFTArea->sizePolicy().hasHeightForWidth());
        DFTArea->setSizePolicy(sizePolicy);
        verticalLayout = new QVBoxLayout(DFTArea);
        verticalLayout->setObjectName(QStringLiteral("verticalLayout"));

        retranslateUi(DFTArea);

        QMetaObject::connectSlotsByName(DFTArea);
    } // setupUi

    void retranslateUi(QWidget *DFTArea)
    {
        DFTArea->setWindowTitle(QApplication::translate("DFTArea", "Form", 0));
    } // retranslateUi

};

namespace Ui {
    class DFTArea: public Ui_DFTArea {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_DFTAREA_H
