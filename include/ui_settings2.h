/********************************************************************************
** Form generated from reading UI file 'settings2.ui'
**
** Created by: Qt User Interface Compiler version 5.3.2
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_SETTINGS2_H
#define UI_SETTINGS2_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QDialog>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QListWidget>
#include <QtWidgets/QStackedWidget>

QT_BEGIN_NAMESPACE

class Ui_Settings2
{
public:
    QGridLayout *gridLayout;
    QListWidget *listWidget;
    QStackedWidget *stackedWidget;

    void setupUi(QDialog *Settings2)
    {
        if (Settings2->objectName().isEmpty())
            Settings2->setObjectName(QStringLiteral("Settings2"));
        Settings2->resize(712, 307);
        gridLayout = new QGridLayout(Settings2);
        gridLayout->setObjectName(QStringLiteral("gridLayout"));
        listWidget = new QListWidget(Settings2);
        listWidget->setObjectName(QStringLiteral("listWidget"));
        QSizePolicy sizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
        sizePolicy.setHorizontalStretch(1);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(listWidget->sizePolicy().hasHeightForWidth());
        listWidget->setSizePolicy(sizePolicy);

        gridLayout->addWidget(listWidget, 0, 0, 1, 1);

        stackedWidget = new QStackedWidget(Settings2);
        stackedWidget->setObjectName(QStringLiteral("stackedWidget"));
        QSizePolicy sizePolicy1(QSizePolicy::Preferred, QSizePolicy::Preferred);
        sizePolicy1.setHorizontalStretch(3);
        sizePolicy1.setVerticalStretch(0);
        sizePolicy1.setHeightForWidth(stackedWidget->sizePolicy().hasHeightForWidth());
        stackedWidget->setSizePolicy(sizePolicy1);

        gridLayout->addWidget(stackedWidget, 0, 1, 1, 1);


        retranslateUi(Settings2);

        QMetaObject::connectSlotsByName(Settings2);
    } // setupUi

    void retranslateUi(QDialog *Settings2)
    {
        Settings2->setWindowTitle(QApplication::translate("Settings2", "DFTFring Settings", 0));
    } // retranslateUi

};

namespace Ui {
    class Settings2: public Ui_Settings2 {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_SETTINGS2_H
