/********************************************************************************
** Form generated from reading UI file 'mirrordlg.ui'
**
** Created by: Qt User Interface Compiler version 5.3.2
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_MIRRORDLG_H
#define UI_MIRRORDLG_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QCheckBox>
#include <QtWidgets/QDialog>
#include <QtWidgets/QDialogButtonBox>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QGroupBox>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_mirrorDlg
{
public:
    QDialogButtonBox *buttonBox;
    QGroupBox *groupBox;
    QGridLayout *gridLayout;
    QLineEdit *obs;
    QCheckBox *unitsCB;
    QLabel *label_6;
    QLineEdit *cc;
    QLabel *label_2;
    QLabel *label_3;
    QLineEdit *FNumber;
    QLineEdit *diameter;
    QLineEdit *roc;
    QLabel *label_4;
    QLabel *label_5;
    QLabel *label_7;
    QLineEdit *lambda;
    QCheckBox *nullCB;
    QLineEdit *z8;
    QPushButton *ReadBtn;
    QPushButton *saveBtn;
    QWidget *layoutWidget;
    QHBoxLayout *horizontalLayout;
    QLabel *label;
    QLineEdit *name;
    QCheckBox *flipH;
    QWidget *widget;
    QHBoxLayout *horizontalLayout_2;
    QLabel *label_8;
    QLineEdit *fringeSpacingEdit;

    void setupUi(QDialog *mirrorDlg)
    {
        if (mirrorDlg->objectName().isEmpty())
            mirrorDlg->setObjectName(QStringLiteral("mirrorDlg"));
        mirrorDlg->resize(462, 300);
        buttonBox = new QDialogButtonBox(mirrorDlg);
        buttonBox->setObjectName(QStringLiteral("buttonBox"));
        buttonBox->setGeometry(QRect(80, 260, 341, 32));
        buttonBox->setOrientation(Qt::Horizontal);
        buttonBox->setStandardButtons(QDialogButtonBox::Cancel|QDialogButtonBox::Ok);
        groupBox = new QGroupBox(mirrorDlg);
        groupBox->setObjectName(QStringLiteral("groupBox"));
        groupBox->setGeometry(QRect(20, 50, 181, 205));
        gridLayout = new QGridLayout(groupBox);
        gridLayout->setObjectName(QStringLiteral("gridLayout"));
        obs = new QLineEdit(groupBox);
        obs->setObjectName(QStringLiteral("obs"));

        gridLayout->addWidget(obs, 4, 3, 1, 2);

        unitsCB = new QCheckBox(groupBox);
        unitsCB->setObjectName(QStringLiteral("unitsCB"));

        gridLayout->addWidget(unitsCB, 0, 0, 1, 4);

        label_6 = new QLabel(groupBox);
        label_6->setObjectName(QStringLiteral("label_6"));

        gridLayout->addWidget(label_6, 6, 0, 1, 4);

        cc = new QLineEdit(groupBox);
        cc->setObjectName(QStringLiteral("cc"));

        gridLayout->addWidget(cc, 6, 4, 1, 1);

        label_2 = new QLabel(groupBox);
        label_2->setObjectName(QStringLiteral("label_2"));

        gridLayout->addWidget(label_2, 1, 2, 1, 1);

        label_3 = new QLabel(groupBox);
        label_3->setObjectName(QStringLiteral("label_3"));
        label_3->setAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);

        gridLayout->addWidget(label_3, 2, 2, 1, 1);

        FNumber = new QLineEdit(groupBox);
        FNumber->setObjectName(QStringLiteral("FNumber"));

        gridLayout->addWidget(FNumber, 3, 3, 1, 2);

        diameter = new QLineEdit(groupBox);
        diameter->setObjectName(QStringLiteral("diameter"));

        gridLayout->addWidget(diameter, 1, 3, 1, 2);

        roc = new QLineEdit(groupBox);
        roc->setObjectName(QStringLiteral("roc"));

        gridLayout->addWidget(roc, 2, 3, 1, 2);

        label_4 = new QLabel(groupBox);
        label_4->setObjectName(QStringLiteral("label_4"));
        label_4->setAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);

        gridLayout->addWidget(label_4, 3, 2, 1, 1);

        label_5 = new QLabel(groupBox);
        label_5->setObjectName(QStringLiteral("label_5"));

        gridLayout->addWidget(label_5, 4, 1, 1, 2);

        label_7 = new QLabel(mirrorDlg);
        label_7->setObjectName(QStringLiteral("label_7"));
        label_7->setGeometry(QRect(220, 60, 101, 31));
        lambda = new QLineEdit(mirrorDlg);
        lambda->setObjectName(QStringLiteral("lambda"));
        lambda->setGeometry(QRect(330, 71, 41, 21));
        nullCB = new QCheckBox(mirrorDlg);
        nullCB->setObjectName(QStringLiteral("nullCB"));
        nullCB->setGeometry(QRect(220, 110, 101, 20));
        z8 = new QLineEdit(mirrorDlg);
        z8->setObjectName(QStringLiteral("z8"));
        z8->setEnabled(false);
        z8->setGeometry(QRect(320, 110, 113, 22));
        ReadBtn = new QPushButton(mirrorDlg);
        ReadBtn->setObjectName(QStringLiteral("ReadBtn"));
        ReadBtn->setGeometry(QRect(200, 210, 121, 28));
        saveBtn = new QPushButton(mirrorDlg);
        saveBtn->setObjectName(QStringLiteral("saveBtn"));
        saveBtn->setGeometry(QRect(330, 210, 121, 28));
        layoutWidget = new QWidget(mirrorDlg);
        layoutWidget->setObjectName(QStringLiteral("layoutWidget"));
        layoutWidget->setGeometry(QRect(10, 10, 421, 24));
        horizontalLayout = new QHBoxLayout(layoutWidget);
        horizontalLayout->setObjectName(QStringLiteral("horizontalLayout"));
        horizontalLayout->setContentsMargins(0, 0, 0, 0);
        label = new QLabel(layoutWidget);
        label->setObjectName(QStringLiteral("label"));

        horizontalLayout->addWidget(label);

        name = new QLineEdit(layoutWidget);
        name->setObjectName(QStringLiteral("name"));

        horizontalLayout->addWidget(name);

        flipH = new QCheckBox(mirrorDlg);
        flipH->setObjectName(QStringLiteral("flipH"));
        flipH->setGeometry(QRect(220, 180, 221, 17));
        widget = new QWidget(mirrorDlg);
        widget->setObjectName(QStringLiteral("widget"));
        widget->setGeometry(QRect(220, 150, 188, 22));
        horizontalLayout_2 = new QHBoxLayout(widget);
        horizontalLayout_2->setObjectName(QStringLiteral("horizontalLayout_2"));
        horizontalLayout_2->setContentsMargins(0, 0, 0, 0);
        label_8 = new QLabel(widget);
        label_8->setObjectName(QStringLiteral("label_8"));

        horizontalLayout_2->addWidget(label_8);

        fringeSpacingEdit = new QLineEdit(widget);
        fringeSpacingEdit->setObjectName(QStringLiteral("fringeSpacingEdit"));

        horizontalLayout_2->addWidget(fringeSpacingEdit);


        retranslateUi(mirrorDlg);
        QObject::connect(buttonBox, SIGNAL(accepted()), mirrorDlg, SLOT(accept()));
        QObject::connect(buttonBox, SIGNAL(rejected()), mirrorDlg, SLOT(reject()));

        QMetaObject::connectSlotsByName(mirrorDlg);
    } // setupUi

    void retranslateUi(QDialog *mirrorDlg)
    {
        mirrorDlg->setWindowTitle(QApplication::translate("mirrorDlg", "Dialog", 0));
        groupBox->setTitle(QApplication::translate("mirrorDlg", "Mirror ", 0));
        unitsCB->setText(QApplication::translate("mirrorDlg", "Units in mm", 0));
        label_6->setText(QApplication::translate("mirrorDlg", "Conic Constant:", 0));
        label_2->setText(QApplication::translate("mirrorDlg", "Diameter:", 0));
        label_3->setText(QApplication::translate("mirrorDlg", "Roc:", 0));
        label_4->setText(QApplication::translate("mirrorDlg", "FNumber:", 0));
        label_5->setText(QApplication::translate("mirrorDlg", "Obstruction:", 0));
        label_7->setText(QApplication::translate("mirrorDlg", "Interferogram\n"
"Wave Length nm", 0));
        nullCB->setText(QApplication::translate("mirrorDlg", "Artificial Null", 0));
        ReadBtn->setText(QApplication::translate("mirrorDlg", "Read Existing File", 0));
        saveBtn->setText(QApplication::translate("mirrorDlg", "Save in a File", 0));
        label->setText(QApplication::translate("mirrorDlg", "Name", 0));
        flipH->setText(QApplication::translate("mirrorDlg", " Interferogram left to right on loading ", 0));
        label_8->setText(QApplication::translate("mirrorDlg", "Fringe Spacing in waves:", 0));
    } // retranslateUi

};

namespace Ui {
    class mirrorDlg: public Ui_mirrorDlg {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_MIRRORDLG_H
