/********************************************************************************
** Form generated from reading UI file 'usercolormapdlg.ui'
**
** Created by: Qt User Interface Compiler version 5.3.2
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_USERCOLORMAPDLG_H
#define UI_USERCOLORMAPDLG_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QCheckBox>
#include <QtWidgets/QDialog>
#include <QtWidgets/QDialogButtonBox>
#include <QtWidgets/QFormLayout>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QWidget>
#include <qwt_plot.h>

QT_BEGIN_NAMESPACE

class Ui_userColorMapDlg
{
public:
    QDialogButtonBox *buttonBox;
    QwtPlot *plot;
    QPushButton *apply;
    QPushButton *savePb;
    QPushButton *loadPb;
    QWidget *layoutWidget;
    QFormLayout *formLayout;
    QCheckBox *cb10;
    QPushButton *pb10;
    QCheckBox *cb09;
    QPushButton *pb09;
    QCheckBox *cb08;
    QPushButton *pb08;
    QCheckBox *cb07;
    QPushButton *pb07;
    QCheckBox *cb06;
    QPushButton *pb06;
    QCheckBox *cb05;
    QPushButton *pb05;
    QCheckBox *cb04;
    QPushButton *pb04;
    QCheckBox *cb03;
    QPushButton *pb03;
    QCheckBox *cb02;
    QPushButton *pb02;
    QCheckBox *cb01;
    QPushButton *pb01;
    QCheckBox *cb00;
    QPushButton *pb00;
    QPushButton *pbLess;
    QPushButton *pbMore;
    QCheckBox *reverseCB;

    void setupUi(QDialog *userColorMapDlg)
    {
        if (userColorMapDlg->objectName().isEmpty())
            userColorMapDlg->setObjectName(QStringLiteral("userColorMapDlg"));
        userColorMapDlg->resize(917, 547);
        buttonBox = new QDialogButtonBox(userColorMapDlg);
        buttonBox->setObjectName(QStringLiteral("buttonBox"));
        buttonBox->setGeometry(QRect(510, 490, 156, 23));
        buttonBox->setOrientation(Qt::Horizontal);
        buttonBox->setStandardButtons(QDialogButtonBox::Cancel|QDialogButtonBox::Ok);
        plot = new QwtPlot(userColorMapDlg);
        plot->setObjectName(QStringLiteral("plot"));
        plot->setGeometry(QRect(149, 29, 681, 441));
        apply = new QPushButton(userColorMapDlg);
        apply->setObjectName(QStringLiteral("apply"));
        apply->setGeometry(QRect(150, 500, 75, 23));
        savePb = new QPushButton(userColorMapDlg);
        savePb->setObjectName(QStringLiteral("savePb"));
        savePb->setGeometry(QRect(710, 490, 75, 23));
        loadPb = new QPushButton(userColorMapDlg);
        loadPb->setObjectName(QStringLiteral("loadPb"));
        loadPb->setGeometry(QRect(820, 490, 75, 23));
        layoutWidget = new QWidget(userColorMapDlg);
        layoutWidget->setObjectName(QStringLiteral("layoutWidget"));
        layoutWidget->setGeometry(QRect(10, 50, 116, 381));
        formLayout = new QFormLayout(layoutWidget);
        formLayout->setObjectName(QStringLiteral("formLayout"));
        formLayout->setFieldGrowthPolicy(QFormLayout::AllNonFixedFieldsGrow);
        formLayout->setContentsMargins(0, 0, 0, 0);
        cb10 = new QCheckBox(layoutWidget);
        cb10->setObjectName(QStringLiteral("cb10"));

        formLayout->setWidget(1, QFormLayout::LabelRole, cb10);

        pb10 = new QPushButton(layoutWidget);
        pb10->setObjectName(QStringLiteral("pb10"));

        formLayout->setWidget(1, QFormLayout::FieldRole, pb10);

        cb09 = new QCheckBox(layoutWidget);
        cb09->setObjectName(QStringLiteral("cb09"));

        formLayout->setWidget(2, QFormLayout::LabelRole, cb09);

        pb09 = new QPushButton(layoutWidget);
        pb09->setObjectName(QStringLiteral("pb09"));
        pb09->setAutoFillBackground(true);

        formLayout->setWidget(2, QFormLayout::FieldRole, pb09);

        cb08 = new QCheckBox(layoutWidget);
        cb08->setObjectName(QStringLiteral("cb08"));

        formLayout->setWidget(3, QFormLayout::LabelRole, cb08);

        pb08 = new QPushButton(layoutWidget);
        pb08->setObjectName(QStringLiteral("pb08"));

        formLayout->setWidget(3, QFormLayout::FieldRole, pb08);

        cb07 = new QCheckBox(layoutWidget);
        cb07->setObjectName(QStringLiteral("cb07"));

        formLayout->setWidget(4, QFormLayout::LabelRole, cb07);

        pb07 = new QPushButton(layoutWidget);
        pb07->setObjectName(QStringLiteral("pb07"));

        formLayout->setWidget(4, QFormLayout::FieldRole, pb07);

        cb06 = new QCheckBox(layoutWidget);
        cb06->setObjectName(QStringLiteral("cb06"));

        formLayout->setWidget(5, QFormLayout::LabelRole, cb06);

        pb06 = new QPushButton(layoutWidget);
        pb06->setObjectName(QStringLiteral("pb06"));

        formLayout->setWidget(5, QFormLayout::FieldRole, pb06);

        cb05 = new QCheckBox(layoutWidget);
        cb05->setObjectName(QStringLiteral("cb05"));

        formLayout->setWidget(6, QFormLayout::LabelRole, cb05);

        pb05 = new QPushButton(layoutWidget);
        pb05->setObjectName(QStringLiteral("pb05"));

        formLayout->setWidget(6, QFormLayout::FieldRole, pb05);

        cb04 = new QCheckBox(layoutWidget);
        cb04->setObjectName(QStringLiteral("cb04"));

        formLayout->setWidget(7, QFormLayout::LabelRole, cb04);

        pb04 = new QPushButton(layoutWidget);
        pb04->setObjectName(QStringLiteral("pb04"));

        formLayout->setWidget(7, QFormLayout::FieldRole, pb04);

        cb03 = new QCheckBox(layoutWidget);
        cb03->setObjectName(QStringLiteral("cb03"));

        formLayout->setWidget(8, QFormLayout::LabelRole, cb03);

        pb03 = new QPushButton(layoutWidget);
        pb03->setObjectName(QStringLiteral("pb03"));

        formLayout->setWidget(8, QFormLayout::FieldRole, pb03);

        cb02 = new QCheckBox(layoutWidget);
        cb02->setObjectName(QStringLiteral("cb02"));

        formLayout->setWidget(9, QFormLayout::LabelRole, cb02);

        pb02 = new QPushButton(layoutWidget);
        pb02->setObjectName(QStringLiteral("pb02"));

        formLayout->setWidget(9, QFormLayout::FieldRole, pb02);

        cb01 = new QCheckBox(layoutWidget);
        cb01->setObjectName(QStringLiteral("cb01"));

        formLayout->setWidget(10, QFormLayout::LabelRole, cb01);

        pb01 = new QPushButton(layoutWidget);
        pb01->setObjectName(QStringLiteral("pb01"));

        formLayout->setWidget(10, QFormLayout::FieldRole, pb01);

        cb00 = new QCheckBox(layoutWidget);
        cb00->setObjectName(QStringLiteral("cb00"));
        cb00->setChecked(true);

        formLayout->setWidget(11, QFormLayout::LabelRole, cb00);

        pb00 = new QPushButton(layoutWidget);
        pb00->setObjectName(QStringLiteral("pb00"));

        formLayout->setWidget(11, QFormLayout::FieldRole, pb00);

        pbLess = new QPushButton(layoutWidget);
        pbLess->setObjectName(QStringLiteral("pbLess"));

        formLayout->setWidget(12, QFormLayout::FieldRole, pbLess);

        pbMore = new QPushButton(layoutWidget);
        pbMore->setObjectName(QStringLiteral("pbMore"));

        formLayout->setWidget(0, QFormLayout::FieldRole, pbMore);

        reverseCB = new QCheckBox(userColorMapDlg);
        reverseCB->setObjectName(QStringLiteral("reverseCB"));
        reverseCB->setGeometry(QRect(369, 490, 111, 20));

        retranslateUi(userColorMapDlg);
        QObject::connect(buttonBox, SIGNAL(accepted()), userColorMapDlg, SLOT(accept()));
        QObject::connect(buttonBox, SIGNAL(rejected()), userColorMapDlg, SLOT(reject()));

        QMetaObject::connectSlotsByName(userColorMapDlg);
    } // setupUi

    void retranslateUi(QDialog *userColorMapDlg)
    {
        userColorMapDlg->setWindowTitle(QApplication::translate("userColorMapDlg", "Dialog", 0));
        apply->setText(QApplication::translate("userColorMapDlg", "Apply", 0));
        savePb->setText(QApplication::translate("userColorMapDlg", "Save", 0));
        loadPb->setText(QApplication::translate("userColorMapDlg", "Load", 0));
        cb10->setText(QApplication::translate("userColorMapDlg", "1", 0));
        pb10->setText(QString());
        cb09->setText(QApplication::translate("userColorMapDlg", ".9", 0));
        pb09->setText(QString());
        cb08->setText(QApplication::translate("userColorMapDlg", ".8", 0));
        pb08->setText(QString());
        cb07->setText(QApplication::translate("userColorMapDlg", ".7", 0));
        pb07->setText(QString());
        cb06->setText(QApplication::translate("userColorMapDlg", ".6", 0));
        pb06->setText(QString());
        cb05->setText(QApplication::translate("userColorMapDlg", ".5", 0));
        pb05->setText(QString());
        cb04->setText(QApplication::translate("userColorMapDlg", ".4", 0));
        pb04->setText(QString());
        cb03->setText(QApplication::translate("userColorMapDlg", ".3", 0));
        pb03->setText(QString());
        cb02->setText(QApplication::translate("userColorMapDlg", ".2", 0));
        pb02->setText(QString());
        cb01->setText(QApplication::translate("userColorMapDlg", ".1", 0));
        pb01->setText(QString());
        cb00->setText(QApplication::translate("userColorMapDlg", "0", 0));
        pb00->setText(QString());
        pbLess->setText(QString());
        pbMore->setText(QString());
        reverseCB->setText(QApplication::translate("userColorMapDlg", "Reverse Order", 0));
    } // retranslateUi

};

namespace Ui {
    class userColorMapDlg: public Ui_userColorMapDlg {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_USERCOLORMAPDLG_H
