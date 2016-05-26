/********************************************************************************
** Form generated from reading UI file 'settingsdebug.ui'
**
** Created by: Qt User Interface Compiler version 5.3.2
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_SETTINGSDEBUG_H
#define UI_SETTINGSDEBUG_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QCheckBox>
#include <QtWidgets/QDialog>
#include <QtWidgets/QDialogButtonBox>
#include <QtWidgets/QHeaderView>

QT_BEGIN_NAMESPACE

class Ui_settingsDebug
{
public:
    QDialogButtonBox *buttonBox;
    QCheckBox *checkBox;

    void setupUi(QDialog *settingsDebug)
    {
        if (settingsDebug->objectName().isEmpty())
            settingsDebug->setObjectName(QStringLiteral("settingsDebug"));
        settingsDebug->resize(400, 300);
        buttonBox = new QDialogButtonBox(settingsDebug);
        buttonBox->setObjectName(QStringLiteral("buttonBox"));
        buttonBox->setGeometry(QRect(30, 240, 151, 32));
        buttonBox->setOrientation(Qt::Horizontal);
        buttonBox->setStandardButtons(QDialogButtonBox::Ok);
        checkBox = new QCheckBox(settingsDebug);
        checkBox->setObjectName(QStringLiteral("checkBox"));
        checkBox->setGeometry(QRect(30, 30, 291, 21));

        retranslateUi(settingsDebug);
        QObject::connect(buttonBox, SIGNAL(accepted()), settingsDebug, SLOT(accept()));
        QObject::connect(buttonBox, SIGNAL(rejected()), settingsDebug, SLOT(reject()));

        QMetaObject::connectSlotsByName(settingsDebug);
    } // setupUi

    void retranslateUi(QDialog *settingsDebug)
    {
        settingsDebug->setWindowTitle(QApplication::translate("settingsDebug", "Dialog", 0));
        checkBox->setText(QApplication::translate("settingsDebug", "Show Mask used for analysis", 0));
    } // retranslateUi

};

namespace Ui {
    class settingsDebug: public Ui_settingsDebug {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_SETTINGSDEBUG_H
