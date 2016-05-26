/********************************************************************************
** Form generated from reading UI file 'jitteroutlinedlg.ui'
**
** Created by: Qt User Interface Compiler version 5.3.2
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_JITTEROUTLINEDLG_H
#define UI_JITTEROUTLINEDLG_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QDialog>
#include <QtWidgets/QDialogButtonBox>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLabel>
#include <QtWidgets/QProgressBar>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QRadioButton>
#include <QtWidgets/QSpinBox>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_jitterOutlineDlg
{
public:
    QDialogButtonBox *buttonBox;
    QProgressBar *progressBar;
    QPushButton *startPb;
    QWidget *widget;
    QHBoxLayout *horizontalLayout;
    QRadioButton *scanX;
    QRadioButton *scanY;
    QRadioButton *scanRad;
    QWidget *widget1;
    QVBoxLayout *verticalLayout;
    QHBoxLayout *horizontalLayout_4;
    QLabel *label;
    QSpinBox *startSB;
    QHBoxLayout *horizontalLayout_3;
    QLabel *label_2;
    QSpinBox *endSB;
    QHBoxLayout *horizontalLayout_2;
    QLabel *label_3;
    QSpinBox *stepSB;
    QLabel *label_4;
    QWidget *widget2;
    QHBoxLayout *horizontalLayout_5;
    QPushButton *StopPb;
    QLabel *status;

    void setupUi(QDialog *jitterOutlineDlg)
    {
        if (jitterOutlineDlg->objectName().isEmpty())
            jitterOutlineDlg->setObjectName(QStringLiteral("jitterOutlineDlg"));
        jitterOutlineDlg->resize(234, 300);
        buttonBox = new QDialogButtonBox(jitterOutlineDlg);
        buttonBox->setObjectName(QStringLiteral("buttonBox"));
        buttonBox->setGeometry(QRect(9, 252, 75, 23));
        buttonBox->setOrientation(Qt::Horizontal);
        buttonBox->setStandardButtons(QDialogButtonBox::Cancel);
        buttonBox->setCenterButtons(true);
        progressBar = new QProgressBar(jitterOutlineDlg);
        progressBar->setObjectName(QStringLiteral("progressBar"));
        progressBar->setGeometry(QRect(90, 171, 131, 21));
        progressBar->setValue(0);
        startPb = new QPushButton(jitterOutlineDlg);
        startPb->setObjectName(QStringLiteral("startPb"));
        startPb->setGeometry(QRect(9, 170, 75, 23));
        widget = new QWidget(jitterOutlineDlg);
        widget->setObjectName(QStringLiteral("widget"));
        widget->setGeometry(QRect(9, 21, 205, 19));
        horizontalLayout = new QHBoxLayout(widget);
        horizontalLayout->setObjectName(QStringLiteral("horizontalLayout"));
        horizontalLayout->setContentsMargins(0, 0, 0, 0);
        scanX = new QRadioButton(widget);
        scanX->setObjectName(QStringLiteral("scanX"));

        horizontalLayout->addWidget(scanX);

        scanY = new QRadioButton(widget);
        scanY->setObjectName(QStringLiteral("scanY"));

        horizontalLayout->addWidget(scanY);

        scanRad = new QRadioButton(widget);
        scanRad->setObjectName(QStringLiteral("scanRad"));
        scanRad->setChecked(true);

        horizontalLayout->addWidget(scanRad);

        widget1 = new QWidget(jitterOutlineDlg);
        widget1->setObjectName(QStringLiteral("widget1"));
        widget1->setGeometry(QRect(9, 56, 121, 80));
        verticalLayout = new QVBoxLayout(widget1);
        verticalLayout->setObjectName(QStringLiteral("verticalLayout"));
        verticalLayout->setContentsMargins(0, 0, 0, 0);
        horizontalLayout_4 = new QHBoxLayout();
        horizontalLayout_4->setObjectName(QStringLiteral("horizontalLayout_4"));
        label = new QLabel(widget1);
        label->setObjectName(QStringLiteral("label"));

        horizontalLayout_4->addWidget(label);

        startSB = new QSpinBox(widget1);
        startSB->setObjectName(QStringLiteral("startSB"));
        startSB->setMinimum(-20);
        startSB->setMaximum(0);
        startSB->setValue(-20);

        horizontalLayout_4->addWidget(startSB);


        verticalLayout->addLayout(horizontalLayout_4);

        horizontalLayout_3 = new QHBoxLayout();
        horizontalLayout_3->setObjectName(QStringLiteral("horizontalLayout_3"));
        label_2 = new QLabel(widget1);
        label_2->setObjectName(QStringLiteral("label_2"));

        horizontalLayout_3->addWidget(label_2);

        endSB = new QSpinBox(widget1);
        endSB->setObjectName(QStringLiteral("endSB"));
        endSB->setMinimum(1);
        endSB->setMaximum(20);
        endSB->setValue(20);

        horizontalLayout_3->addWidget(endSB);


        verticalLayout->addLayout(horizontalLayout_3);

        horizontalLayout_2 = new QHBoxLayout();
        horizontalLayout_2->setObjectName(QStringLiteral("horizontalLayout_2"));
        label_3 = new QLabel(widget1);
        label_3->setObjectName(QStringLiteral("label_3"));

        horizontalLayout_2->addWidget(label_3);

        stepSB = new QSpinBox(widget1);
        stepSB->setObjectName(QStringLiteral("stepSB"));
        stepSB->setMinimum(1);
        stepSB->setMaximum(10);

        horizontalLayout_2->addWidget(stepSB);

        label_4 = new QLabel(widget1);
        label_4->setObjectName(QStringLiteral("label_4"));

        horizontalLayout_2->addWidget(label_4);


        verticalLayout->addLayout(horizontalLayout_2);

        widget2 = new QWidget(jitterOutlineDlg);
        widget2->setObjectName(QStringLiteral("widget2"));
        widget2->setGeometry(QRect(9, 210, 211, 25));
        horizontalLayout_5 = new QHBoxLayout(widget2);
        horizontalLayout_5->setObjectName(QStringLiteral("horizontalLayout_5"));
        horizontalLayout_5->setContentsMargins(0, 0, 0, 0);
        StopPb = new QPushButton(widget2);
        StopPb->setObjectName(QStringLiteral("StopPb"));
        QSizePolicy sizePolicy(QSizePolicy::Minimum, QSizePolicy::Fixed);
        sizePolicy.setHorizontalStretch(1);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(StopPb->sizePolicy().hasHeightForWidth());
        StopPb->setSizePolicy(sizePolicy);

        horizontalLayout_5->addWidget(StopPb);

        status = new QLabel(widget2);
        status->setObjectName(QStringLiteral("status"));
        QSizePolicy sizePolicy1(QSizePolicy::Preferred, QSizePolicy::Preferred);
        sizePolicy1.setHorizontalStretch(5);
        sizePolicy1.setVerticalStretch(0);
        sizePolicy1.setHeightForWidth(status->sizePolicy().hasHeightForWidth());
        status->setSizePolicy(sizePolicy1);

        horizontalLayout_5->addWidget(status);

#ifndef QT_NO_SHORTCUT
        label_2->setBuddy(endSB);
        label_3->setBuddy(stepSB);
#endif // QT_NO_SHORTCUT

        retranslateUi(jitterOutlineDlg);
        QObject::connect(buttonBox, SIGNAL(accepted()), jitterOutlineDlg, SLOT(accept()));
        QObject::connect(buttonBox, SIGNAL(rejected()), jitterOutlineDlg, SLOT(reject()));

        QMetaObject::connectSlotsByName(jitterOutlineDlg);
    } // setupUi

    void retranslateUi(QDialog *jitterOutlineDlg)
    {
        jitterOutlineDlg->setWindowTitle(QApplication::translate("jitterOutlineDlg", "Outline Scanner", 0));
#ifndef QT_NO_WHATSTHIS
        jitterOutlineDlg->setWhatsThis(QApplication::translate("jitterOutlineDlg", "<html><head/><body><p>Moves the outline offseting by start pixels.  Ending at end pixels and stepping by step pixels.</p><p>Will move either the x or y center of the outline, or reduce and increase the radius as</p><p>selected by the Scan radio buttons.</p></body></html>", 0));
#endif // QT_NO_WHATSTHIS
        startPb->setText(QApplication::translate("jitterOutlineDlg", "Start", 0));
        scanX->setText(QApplication::translate("jitterOutlineDlg", "Scan X", 0));
        scanY->setText(QApplication::translate("jitterOutlineDlg", "Scan y", 0));
        scanRad->setText(QApplication::translate("jitterOutlineDlg", "Scan Radius", 0));
        label->setText(QApplication::translate("jitterOutlineDlg", "Start at", 0));
        label_2->setText(QApplication::translate("jitterOutlineDlg", "End", 0));
        label_3->setText(QApplication::translate("jitterOutlineDlg", "StepSize", 0));
        label_4->setText(QApplication::translate("jitterOutlineDlg", "Pixels", 0));
        StopPb->setText(QApplication::translate("jitterOutlineDlg", "Stop", 0));
        status->setText(QString());
    } // retranslateUi

};

namespace Ui {
    class jitterOutlineDlg: public Ui_jitterOutlineDlg {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_JITTEROUTLINEDLG_H
