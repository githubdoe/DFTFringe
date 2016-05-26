/********************************************************************************
** Form generated from reading UI file 'dftthumb.ui'
**
** Created by: Qt User Interface Compiler version 5.3.2
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_DFTTHUMB_H
#define UI_DFTTHUMB_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QDialog>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLabel>
#include <QtWidgets/QVBoxLayout>

QT_BEGIN_NAMESPACE

class Ui_dftThumb
{
public:
    QVBoxLayout *verticalLayout;
    QLabel *label;

    void setupUi(QDialog *dftThumb)
    {
        if (dftThumb->objectName().isEmpty())
            dftThumb->setObjectName(QStringLiteral("dftThumb"));
        dftThumb->resize(400, 300);
        verticalLayout = new QVBoxLayout(dftThumb);
        verticalLayout->setObjectName(QStringLiteral("verticalLayout"));
        label = new QLabel(dftThumb);
        label->setObjectName(QStringLiteral("label"));

        verticalLayout->addWidget(label);


        retranslateUi(dftThumb);

        QMetaObject::connectSlotsByName(dftThumb);
    } // setupUi

    void retranslateUi(QDialog *dftThumb)
    {
        dftThumb->setWindowTitle(QApplication::translate("dftThumb", "DFT Magnitude", 0));
        label->setText(QApplication::translate("dftThumb", "TextLabel", 0));
    } // retranslateUi

};

namespace Ui {
    class dftThumb: public Ui_dftThumb {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_DFTTHUMB_H
