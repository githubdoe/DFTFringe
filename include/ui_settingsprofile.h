/********************************************************************************
** Form generated from reading UI file 'settingsprofile.ui'
**
** Created by: Qt User Interface Compiler version 5.3.2
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_SETTINGSPROFILE_H
#define UI_SETTINGSPROFILE_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QDialog>
#include <QtWidgets/QDialogButtonBox>
#include <QtWidgets/QFormLayout>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLabel>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_settingsProfile
{
public:
    QDialogButtonBox *buttonBox;
    QLabel *label_8;
    QLabel *label_11;
    QWidget *widget;
    QFormLayout *formLayout;
    QLabel *label;
    QPushButton *pushButton_1;
    QLabel *label_2;
    QPushButton *pushButton_2;
    QLabel *label_3;
    QPushButton *pushButton_3;
    QLabel *label_4;
    QPushButton *pushButton_4;
    QLabel *label_5;
    QPushButton *pushButton_5;
    QLabel *label_9;
    QLabel *label_10;
    QLabel *label_6;
    QLabel *label_7;
    QPushButton *pushButton_6;
    QPushButton *pushButton_7;
    QPushButton *pushButton_8;
    QPushButton *pushButton_9;

    void setupUi(QDialog *settingsProfile)
    {
        if (settingsProfile->objectName().isEmpty())
            settingsProfile->setObjectName(QStringLiteral("settingsProfile"));
        settingsProfile->resize(400, 300);
        buttonBox = new QDialogButtonBox(settingsProfile);
        buttonBox->setObjectName(QStringLiteral("buttonBox"));
        buttonBox->setGeometry(QRect(210, 260, 111, 32));
        buttonBox->setOrientation(Qt::Horizontal);
        buttonBox->setStandardButtons(QDialogButtonBox::NoButton);
        label_8 = new QLabel(settingsProfile);
        label_8->setObjectName(QStringLiteral("label_8"));
        label_8->setGeometry(QRect(30, 0, 231, 16));
        label_11 = new QLabel(settingsProfile);
        label_11->setObjectName(QStringLiteral("label_11"));
        label_11->setGeometry(QRect(180, 10, 191, 61));
        label_11->setWordWrap(true);
        widget = new QWidget(settingsProfile);
        widget->setObjectName(QStringLiteral("widget"));
        widget->setGeometry(QRect(30, 20, 121, 266));
        formLayout = new QFormLayout(widget);
        formLayout->setObjectName(QStringLiteral("formLayout"));
        formLayout->setContentsMargins(0, 0, 0, 0);
        label = new QLabel(widget);
        label->setObjectName(QStringLiteral("label"));

        formLayout->setWidget(0, QFormLayout::LabelRole, label);

        pushButton_1 = new QPushButton(widget);
        pushButton_1->setObjectName(QStringLiteral("pushButton_1"));

        formLayout->setWidget(0, QFormLayout::FieldRole, pushButton_1);

        label_2 = new QLabel(widget);
        label_2->setObjectName(QStringLiteral("label_2"));

        formLayout->setWidget(1, QFormLayout::LabelRole, label_2);

        pushButton_2 = new QPushButton(widget);
        pushButton_2->setObjectName(QStringLiteral("pushButton_2"));

        formLayout->setWidget(1, QFormLayout::FieldRole, pushButton_2);

        label_3 = new QLabel(widget);
        label_3->setObjectName(QStringLiteral("label_3"));

        formLayout->setWidget(2, QFormLayout::LabelRole, label_3);

        pushButton_3 = new QPushButton(widget);
        pushButton_3->setObjectName(QStringLiteral("pushButton_3"));

        formLayout->setWidget(2, QFormLayout::FieldRole, pushButton_3);

        label_4 = new QLabel(widget);
        label_4->setObjectName(QStringLiteral("label_4"));

        formLayout->setWidget(3, QFormLayout::LabelRole, label_4);

        pushButton_4 = new QPushButton(widget);
        pushButton_4->setObjectName(QStringLiteral("pushButton_4"));

        formLayout->setWidget(3, QFormLayout::FieldRole, pushButton_4);

        label_5 = new QLabel(widget);
        label_5->setObjectName(QStringLiteral("label_5"));

        formLayout->setWidget(4, QFormLayout::LabelRole, label_5);

        pushButton_5 = new QPushButton(widget);
        pushButton_5->setObjectName(QStringLiteral("pushButton_5"));

        formLayout->setWidget(4, QFormLayout::FieldRole, pushButton_5);

        label_9 = new QLabel(widget);
        label_9->setObjectName(QStringLiteral("label_9"));

        formLayout->setWidget(5, QFormLayout::LabelRole, label_9);

        label_10 = new QLabel(widget);
        label_10->setObjectName(QStringLiteral("label_10"));

        formLayout->setWidget(6, QFormLayout::LabelRole, label_10);

        label_6 = new QLabel(widget);
        label_6->setObjectName(QStringLiteral("label_6"));

        formLayout->setWidget(7, QFormLayout::LabelRole, label_6);

        label_7 = new QLabel(widget);
        label_7->setObjectName(QStringLiteral("label_7"));

        formLayout->setWidget(8, QFormLayout::LabelRole, label_7);

        pushButton_6 = new QPushButton(widget);
        pushButton_6->setObjectName(QStringLiteral("pushButton_6"));

        formLayout->setWidget(5, QFormLayout::FieldRole, pushButton_6);

        pushButton_7 = new QPushButton(widget);
        pushButton_7->setObjectName(QStringLiteral("pushButton_7"));

        formLayout->setWidget(6, QFormLayout::FieldRole, pushButton_7);

        pushButton_8 = new QPushButton(widget);
        pushButton_8->setObjectName(QStringLiteral("pushButton_8"));

        formLayout->setWidget(7, QFormLayout::FieldRole, pushButton_8);

        pushButton_9 = new QPushButton(widget);
        pushButton_9->setObjectName(QStringLiteral("pushButton_9"));

        formLayout->setWidget(8, QFormLayout::FieldRole, pushButton_9);


        retranslateUi(settingsProfile);
        QObject::connect(buttonBox, SIGNAL(accepted()), settingsProfile, SLOT(accept()));
        QObject::connect(buttonBox, SIGNAL(rejected()), settingsProfile, SLOT(reject()));

        QMetaObject::connectSlotsByName(settingsProfile);
    } // setupUi

    void retranslateUi(QDialog *settingsProfile)
    {
        settingsProfile->setWindowTitle(QApplication::translate("settingsProfile", "Profile line colors", 0));
        label_8->setText(QApplication::translate("settingsProfile", "These colors are used for the profile line plots", 0));
        label_11->setText(QApplication::translate("settingsProfile", "When displaying multiple profiles these colors are cycled through for each wavefront.", 0));
        label->setText(QApplication::translate("settingsProfile", "Color 1:", 0));
        pushButton_1->setText(QString());
        label_2->setText(QApplication::translate("settingsProfile", "Color 2:", 0));
        pushButton_2->setText(QString());
        label_3->setText(QApplication::translate("settingsProfile", "Color 3:", 0));
        pushButton_3->setText(QString());
        label_4->setText(QApplication::translate("settingsProfile", "Color 4:", 0));
        pushButton_4->setText(QString());
        label_5->setText(QApplication::translate("settingsProfile", "Color 5:", 0));
        pushButton_5->setText(QString());
        label_9->setText(QApplication::translate("settingsProfile", "Color 6:", 0));
        label_10->setText(QApplication::translate("settingsProfile", "Color 7:", 0));
        label_6->setText(QApplication::translate("settingsProfile", "Color 8:", 0));
        label_7->setText(QApplication::translate("settingsProfile", "Color 9:", 0));
        pushButton_6->setText(QString());
        pushButton_7->setText(QString());
        pushButton_8->setText(QString());
        pushButton_9->setText(QString());
    } // retranslateUi

};

namespace Ui {
    class settingsProfile: public Ui_settingsProfile {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_SETTINGSPROFILE_H
