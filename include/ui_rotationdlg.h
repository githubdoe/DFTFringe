/********************************************************************************
** Form generated from reading UI file 'rotationdlg.ui'
**
** Created by: Qt User Interface Compiler version 5.3.2
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_ROTATIONDLG_H
#define UI_ROTATIONDLG_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QCheckBox>
#include <QtWidgets/QDialog>
#include <QtWidgets/QDialogButtonBox>
#include <QtWidgets/QDoubleSpinBox>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLabel>
#include <QtWidgets/QSpacerItem>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_RotationDlg
{
public:
    QWidget *widget;
    QVBoxLayout *verticalLayout;
    QHBoxLayout *horizontalLayout;
    QLabel *label;
    QDoubleSpinBox *angleSB;
    QHBoxLayout *horizontalLayout_2;
    QSpacerItem *horizontalSpacer_2;
    QCheckBox *CCWCB;
    QDialogButtonBox *buttonBox;

    void setupUi(QDialog *RotationDlg)
    {
        if (RotationDlg->objectName().isEmpty())
            RotationDlg->setObjectName(QStringLiteral("RotationDlg"));
        RotationDlg->resize(207, 164);
        widget = new QWidget(RotationDlg);
        widget->setObjectName(QStringLiteral("widget"));
        widget->setGeometry(QRect(1, 50, 195, 90));
        verticalLayout = new QVBoxLayout(widget);
        verticalLayout->setObjectName(QStringLiteral("verticalLayout"));
        verticalLayout->setContentsMargins(0, 0, 0, 0);
        horizontalLayout = new QHBoxLayout();
        horizontalLayout->setObjectName(QStringLiteral("horizontalLayout"));
        label = new QLabel(widget);
        label->setObjectName(QStringLiteral("label"));
        label->setAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);

        horizontalLayout->addWidget(label);

        angleSB = new QDoubleSpinBox(widget);
        angleSB->setObjectName(QStringLiteral("angleSB"));
        angleSB->setMaximum(360);

        horizontalLayout->addWidget(angleSB);


        verticalLayout->addLayout(horizontalLayout);

        horizontalLayout_2 = new QHBoxLayout();
        horizontalLayout_2->setObjectName(QStringLiteral("horizontalLayout_2"));
        horizontalSpacer_2 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout_2->addItem(horizontalSpacer_2);

        CCWCB = new QCheckBox(widget);
        CCWCB->setObjectName(QStringLiteral("CCWCB"));

        horizontalLayout_2->addWidget(CCWCB);


        verticalLayout->addLayout(horizontalLayout_2);

        buttonBox = new QDialogButtonBox(widget);
        buttonBox->setObjectName(QStringLiteral("buttonBox"));
        buttonBox->setOrientation(Qt::Horizontal);
        buttonBox->setStandardButtons(QDialogButtonBox::Cancel|QDialogButtonBox::Ok);

        verticalLayout->addWidget(buttonBox);


        retranslateUi(RotationDlg);
        QObject::connect(buttonBox, SIGNAL(accepted()), RotationDlg, SLOT(accept()));
        QObject::connect(buttonBox, SIGNAL(rejected()), RotationDlg, SLOT(reject()));

        QMetaObject::connectSlotsByName(RotationDlg);
    } // setupUi

    void retranslateUi(QDialog *RotationDlg)
    {
        RotationDlg->setWindowTitle(QApplication::translate("RotationDlg", "Rotate Wave Front", 0));
        label->setText(QApplication::translate("RotationDlg", "Rotation Angle:", 0));
        CCWCB->setText(QApplication::translate("RotationDlg", "CCW", 0));
    } // retranslateUi

};

namespace Ui {
    class RotationDlg: public Ui_RotationDlg {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_ROTATIONDLG_H
