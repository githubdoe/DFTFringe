/********************************************************************************
** Form generated from reading UI file 'savewavedlg.ui'
**
** Created by: Qt User Interface Compiler version 5.3.2
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_SAVEWAVEDLG_H
#define UI_SAVEWAVEDLG_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QDialog>
#include <QtWidgets/QDialogButtonBox>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QRadioButton>

QT_BEGIN_NAMESPACE

class Ui_SaveWaveDlg
{
public:
    QDialogButtonBox *buttonBox;
    QLineEdit *lineEdit;
    QPushButton *browsePB;
    QRadioButton *normalRb;
    QRadioButton *radioButton_2;

    void setupUi(QDialog *SaveWaveDlg)
    {
        if (SaveWaveDlg->objectName().isEmpty())
            SaveWaveDlg->setObjectName(QStringLiteral("SaveWaveDlg"));
        SaveWaveDlg->resize(587, 182);
        buttonBox = new QDialogButtonBox(SaveWaveDlg);
        buttonBox->setObjectName(QStringLiteral("buttonBox"));
        buttonBox->setGeometry(QRect(110, 90, 341, 32));
        buttonBox->setOrientation(Qt::Horizontal);
        buttonBox->setStandardButtons(QDialogButtonBox::Cancel|QDialogButtonBox::Ok);
        lineEdit = new QLineEdit(SaveWaveDlg);
        lineEdit->setObjectName(QStringLiteral("lineEdit"));
        lineEdit->setGeometry(QRect(50, 50, 431, 20));
        browsePB = new QPushButton(SaveWaveDlg);
        browsePB->setObjectName(QStringLiteral("browsePB"));
        browsePB->setGeometry(QRect(500, 50, 75, 23));
        normalRb = new QRadioButton(SaveWaveDlg);
        normalRb->setObjectName(QStringLiteral("normalRb"));
        normalRb->setGeometry(QRect(60, 100, 82, 17));
        normalRb->setChecked(true);
        radioButton_2 = new QRadioButton(SaveWaveDlg);
        radioButton_2->setObjectName(QStringLiteral("radioButton_2"));
        radioButton_2->setGeometry(QRect(60, 120, 82, 17));

        retranslateUi(SaveWaveDlg);
        QObject::connect(buttonBox, SIGNAL(accepted()), SaveWaveDlg, SLOT(accept()));
        QObject::connect(buttonBox, SIGNAL(rejected()), SaveWaveDlg, SLOT(reject()));

        QMetaObject::connectSlotsByName(SaveWaveDlg);
    } // setupUi

    void retranslateUi(QDialog *SaveWaveDlg)
    {
        SaveWaveDlg->setWindowTitle(QApplication::translate("SaveWaveDlg", "Dialog", 0));
        browsePB->setText(QApplication::translate("SaveWaveDlg", "Browse", 0));
        normalRb->setText(QApplication::translate("SaveWaveDlg", "Normal", 0));
        radioButton_2->setText(QApplication::translate("SaveWaveDlg", "Nulled", 0));
    } // retranslateUi

};

namespace Ui {
    class SaveWaveDlg: public Ui_SaveWaveDlg {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_SAVEWAVEDLG_H
