/********************************************************************************
** Form generated from reading UI file 'settingsigram.ui'
**
** Created by: Qt User Interface Compiler version 5.3.2
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_SETTINGSIGRAM_H
#define UI_SETTINGSIGRAM_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QCheckBox>
#include <QtWidgets/QComboBox>
#include <QtWidgets/QDialog>
#include <QtWidgets/QDialogButtonBox>
#include <QtWidgets/QDoubleSpinBox>
#include <QtWidgets/QFormLayout>
#include <QtWidgets/QGroupBox>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLabel>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QSpinBox>

QT_BEGIN_NAMESPACE

class Ui_settingsIGram
{
public:
    QDialogButtonBox *buttonBox;
    QGroupBox *groupBox;
    QFormLayout *formLayout;
    QLabel *label;
    QSpinBox *spinBox;
    QLabel *label_2;
    QPushButton *edgeLineColorPb;
    QGroupBox *groupBox_2;
    QFormLayout *formLayout_2;
    QLabel *label_4;
    QSpinBox *centerSpinBox;
    QLabel *label_3;
    QPushButton *centerLineColorPb;
    QLabel *label_5;
    QDoubleSpinBox *opacitySB;
    QLabel *label_6;
    QComboBox *styleCB;
    QCheckBox *hflipCb;

    void setupUi(QDialog *settingsIGram)
    {
        if (settingsIGram->objectName().isEmpty())
            settingsIGram->setObjectName(QStringLiteral("settingsIGram"));
        settingsIGram->resize(367, 311);
        buttonBox = new QDialogButtonBox(settingsIGram);
        buttonBox->setObjectName(QStringLiteral("buttonBox"));
        buttonBox->setGeometry(QRect(10, 260, 341, 32));
        buttonBox->setOrientation(Qt::Horizontal);
        buttonBox->setStandardButtons(QDialogButtonBox::Apply|QDialogButtonBox::RestoreDefaults);
        buttonBox->setCenterButtons(true);
        groupBox = new QGroupBox(settingsIGram);
        groupBox->setObjectName(QStringLiteral("groupBox"));
        groupBox->setGeometry(QRect(50, 10, 117, 82));
        formLayout = new QFormLayout(groupBox);
        formLayout->setObjectName(QStringLiteral("formLayout"));
        label = new QLabel(groupBox);
        label->setObjectName(QStringLiteral("label"));

        formLayout->setWidget(0, QFormLayout::LabelRole, label);

        spinBox = new QSpinBox(groupBox);
        spinBox->setObjectName(QStringLiteral("spinBox"));
        spinBox->setMinimum(1);
        spinBox->setMaximum(40);
        spinBox->setValue(3);

        formLayout->setWidget(0, QFormLayout::FieldRole, spinBox);

        label_2 = new QLabel(groupBox);
        label_2->setObjectName(QStringLiteral("label_2"));

        formLayout->setWidget(1, QFormLayout::LabelRole, label_2);

        edgeLineColorPb = new QPushButton(groupBox);
        edgeLineColorPb->setObjectName(QStringLiteral("edgeLineColorPb"));

        formLayout->setWidget(1, QFormLayout::FieldRole, edgeLineColorPb);

        groupBox_2 = new QGroupBox(settingsIGram);
        groupBox_2->setObjectName(QStringLiteral("groupBox_2"));
        groupBox_2->setGeometry(QRect(50, 110, 151, 82));
        formLayout_2 = new QFormLayout(groupBox_2);
        formLayout_2->setObjectName(QStringLiteral("formLayout_2"));
        label_4 = new QLabel(groupBox_2);
        label_4->setObjectName(QStringLiteral("label_4"));

        formLayout_2->setWidget(0, QFormLayout::LabelRole, label_4);

        centerSpinBox = new QSpinBox(groupBox_2);
        centerSpinBox->setObjectName(QStringLiteral("centerSpinBox"));
        centerSpinBox->setMinimum(1);
        centerSpinBox->setMaximum(10);
        centerSpinBox->setValue(3);

        formLayout_2->setWidget(0, QFormLayout::FieldRole, centerSpinBox);

        label_3 = new QLabel(groupBox_2);
        label_3->setObjectName(QStringLiteral("label_3"));

        formLayout_2->setWidget(1, QFormLayout::LabelRole, label_3);

        centerLineColorPb = new QPushButton(groupBox_2);
        centerLineColorPb->setObjectName(QStringLiteral("centerLineColorPb"));

        formLayout_2->setWidget(1, QFormLayout::FieldRole, centerLineColorPb);

        label_5 = new QLabel(settingsIGram);
        label_5->setObjectName(QStringLiteral("label_5"));
        label_5->setGeometry(QRect(200, 20, 47, 13));
        opacitySB = new QDoubleSpinBox(settingsIGram);
        opacitySB->setObjectName(QStringLiteral("opacitySB"));
        opacitySB->setGeometry(QRect(250, 20, 62, 22));
        opacitySB->setMaximum(100);
        opacitySB->setValue(65);
        label_6 = new QLabel(settingsIGram);
        label_6->setObjectName(QStringLiteral("label_6"));
        label_6->setGeometry(QRect(200, 60, 31, 16));
        styleCB = new QComboBox(settingsIGram);
        styleCB->setObjectName(QStringLiteral("styleCB"));
        styleCB->setGeometry(QRect(250, 60, 91, 22));
        hflipCb = new QCheckBox(settingsIGram);
        hflipCb->setObjectName(QStringLiteral("hflipCb"));
        hflipCb->setGeometry(QRect(50, 210, 261, 21));

        retranslateUi(settingsIGram);
        QObject::connect(buttonBox, SIGNAL(accepted()), settingsIGram, SLOT(accept()));
        QObject::connect(buttonBox, SIGNAL(rejected()), settingsIGram, SLOT(reject()));

        QMetaObject::connectSlotsByName(settingsIGram);
    } // setupUi

    void retranslateUi(QDialog *settingsIGram)
    {
        settingsIGram->setWindowTitle(QApplication::translate("settingsIGram", "Mirror Boundaries", 0));
#ifndef QT_NO_TOOLTIP
        buttonBox->setToolTip(QApplication::translate("settingsIGram", "<html><head/><body><table border=\"1\" style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px;\" width=\"100%\" cellspacing=\"2\" cellpadding=\"0\"><tr><td><p align=\"center\">Cell 1</p></td><td><p>cell 2</p></td></tr><tr><td>row1</td></tr></table></body></html>", 0));
#endif // QT_NO_TOOLTIP
        groupBox->setTitle(QApplication::translate("settingsIGram", "Mirror Outter Edge:", 0));
        label->setText(QApplication::translate("settingsIGram", "Line Width:", 0));
        label_2->setText(QApplication::translate("settingsIGram", "Line Color:", 0));
#ifndef QT_NO_TOOLTIP
        edgeLineColorPb->setToolTip(QApplication::translate("settingsIGram", "Set color of line", 0));
#endif // QT_NO_TOOLTIP
        edgeLineColorPb->setText(QString());
        groupBox_2->setTitle(QApplication::translate("settingsIGram", "optional Mirror center hole:", 0));
        label_4->setText(QApplication::translate("settingsIGram", "Line Width:", 0));
        label_3->setText(QApplication::translate("settingsIGram", "Line Color:", 0));
        centerLineColorPb->setText(QString());
        label_5->setText(QApplication::translate("settingsIGram", "Opacity:", 0));
        label_6->setText(QApplication::translate("settingsIGram", "Style:", 0));
#ifndef QT_NO_TOOLTIP
        styleCB->setToolTip(QApplication::translate("settingsIGram", "Line style", 0));
#endif // QT_NO_TOOLTIP
        hflipCb->setText(QApplication::translate("settingsIGram", "Flip interferogram left to right upon loading.", 0));
    } // retranslateUi

};

namespace Ui {
    class settingsIGram: public Ui_settingsIGram {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_SETTINGSIGRAM_H
