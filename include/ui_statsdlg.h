/********************************************************************************
** Form generated from reading UI file 'statsdlg.ui'
**
** Created by: Qt User Interface Compiler version 5.3.2
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_STATSDLG_H
#define UI_STATSDLG_H

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
#include <QtWidgets/QPushButton>
#include <QtWidgets/QSpacerItem>
#include <QtWidgets/QSpinBox>
#include <QtWidgets/QVBoxLayout>

QT_BEGIN_NAMESPACE

class Ui_StatsDlg
{
public:
    QVBoxLayout *verticalLayout;
    QCheckBox *removeOutliers;
    QHBoxLayout *horizontalLayout_3;
    QCheckBox *RemoveAboveCB;
    QDoubleSpinBox *RMSLimit;
    QCheckBox *doInputsCB;
    QHBoxLayout *horizontalLayout_5;
    QCheckBox *zernGroup;
    QSpinBox *zernStart;
    QSpinBox *zernEnd;
    QCheckBox *showFileNames;
    QHBoxLayout *horizontalLayout;
    QLabel *bgColor;
    QPushButton *bakcgroundpb;
    QSpacerItem *horizontalSpacer;
    QHBoxLayout *horizontalLayout_2;
    QSpacerItem *verticalSpacer;
    QCheckBox *csv;
    QCheckBox *pdf;
    QCheckBox *jpg;
    QDialogButtonBox *buttonBox;

    void setupUi(QDialog *StatsDlg)
    {
        if (StatsDlg->objectName().isEmpty())
            StatsDlg->setObjectName(QStringLiteral("StatsDlg"));
        StatsDlg->resize(359, 325);
        verticalLayout = new QVBoxLayout(StatsDlg);
        verticalLayout->setObjectName(QStringLiteral("verticalLayout"));
        removeOutliers = new QCheckBox(StatsDlg);
        removeOutliers->setObjectName(QStringLiteral("removeOutliers"));

        verticalLayout->addWidget(removeOutliers);

        horizontalLayout_3 = new QHBoxLayout();
        horizontalLayout_3->setObjectName(QStringLiteral("horizontalLayout_3"));
        RemoveAboveCB = new QCheckBox(StatsDlg);
        RemoveAboveCB->setObjectName(QStringLiteral("RemoveAboveCB"));

        horizontalLayout_3->addWidget(RemoveAboveCB);

        RMSLimit = new QDoubleSpinBox(StatsDlg);
        RMSLimit->setObjectName(QStringLiteral("RMSLimit"));
        RMSLimit->setDecimals(3);
        RMSLimit->setMinimum(0.001);
        RMSLimit->setSingleStep(0.001);

        horizontalLayout_3->addWidget(RMSLimit);


        verticalLayout->addLayout(horizontalLayout_3);

        doInputsCB = new QCheckBox(StatsDlg);
        doInputsCB->setObjectName(QStringLiteral("doInputsCB"));
        doInputsCB->setChecked(true);

        verticalLayout->addWidget(doInputsCB);

        horizontalLayout_5 = new QHBoxLayout();
        horizontalLayout_5->setObjectName(QStringLiteral("horizontalLayout_5"));
        zernGroup = new QCheckBox(StatsDlg);
        zernGroup->setObjectName(QStringLiteral("zernGroup"));

        horizontalLayout_5->addWidget(zernGroup);

        zernStart = new QSpinBox(StatsDlg);
        zernStart->setObjectName(QStringLiteral("zernStart"));

        horizontalLayout_5->addWidget(zernStart);

        zernEnd = new QSpinBox(StatsDlg);
        zernEnd->setObjectName(QStringLiteral("zernEnd"));
        zernEnd->setMinimum(4);
        zernEnd->setMaximum(35);

        horizontalLayout_5->addWidget(zernEnd);


        verticalLayout->addLayout(horizontalLayout_5);

        showFileNames = new QCheckBox(StatsDlg);
        showFileNames->setObjectName(QStringLiteral("showFileNames"));

        verticalLayout->addWidget(showFileNames);

        horizontalLayout = new QHBoxLayout();
        horizontalLayout->setObjectName(QStringLiteral("horizontalLayout"));
        bgColor = new QLabel(StatsDlg);
        bgColor->setObjectName(QStringLiteral("bgColor"));
        bgColor->setMinimumSize(QSize(15, 15));
        bgColor->setMaximumSize(QSize(15, 15));

        horizontalLayout->addWidget(bgColor);

        bakcgroundpb = new QPushButton(StatsDlg);
        bakcgroundpb->setObjectName(QStringLiteral("bakcgroundpb"));

        horizontalLayout->addWidget(bakcgroundpb);

        horizontalSpacer = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout->addItem(horizontalSpacer);


        verticalLayout->addLayout(horizontalLayout);

        horizontalLayout_2 = new QHBoxLayout();
        horizontalLayout_2->setObjectName(QStringLiteral("horizontalLayout_2"));

        verticalLayout->addLayout(horizontalLayout_2);

        verticalSpacer = new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding);

        verticalLayout->addItem(verticalSpacer);

        csv = new QCheckBox(StatsDlg);
        csv->setObjectName(QStringLiteral("csv"));

        verticalLayout->addWidget(csv);

        pdf = new QCheckBox(StatsDlg);
        pdf->setObjectName(QStringLiteral("pdf"));

        verticalLayout->addWidget(pdf);

        jpg = new QCheckBox(StatsDlg);
        jpg->setObjectName(QStringLiteral("jpg"));

        verticalLayout->addWidget(jpg);

        buttonBox = new QDialogButtonBox(StatsDlg);
        buttonBox->setObjectName(QStringLiteral("buttonBox"));
        buttonBox->setMinimumSize(QSize(341, 0));
        buttonBox->setOrientation(Qt::Horizontal);
        buttonBox->setStandardButtons(QDialogButtonBox::Cancel|QDialogButtonBox::Ok);

        verticalLayout->addWidget(buttonBox);


        retranslateUi(StatsDlg);
        QObject::connect(buttonBox, SIGNAL(accepted()), StatsDlg, SLOT(accept()));
        QObject::connect(buttonBox, SIGNAL(rejected()), StatsDlg, SLOT(reject()));

        QMetaObject::connectSlotsByName(StatsDlg);
    } // setupUi

    void retranslateUi(QDialog *StatsDlg)
    {
        StatsDlg->setWindowTitle(QApplication::translate("StatsDlg", "Generate Wave front statiscits", 0));
#ifndef QT_NO_WHATSTHIS
        StatsDlg->setWhatsThis(QApplication::translate("StatsDlg", "Create a plot of the running average of the RMS of each loaded wavefronts.", 0));
#endif // QT_NO_WHATSTHIS
        removeOutliers->setText(QApplication::translate("StatsDlg", "Remove outliers", 0));
        RemoveAboveCB->setText(QApplication::translate("StatsDlg", "Remove above RMS value of:", 0));
        doInputsCB->setText(QApplication::translate("StatsDlg", "Input Wavefronts", 0));
        zernGroup->setText(QApplication::translate("StatsDlg", "zern from - to", 0));
        showFileNames->setText(QApplication::translate("StatsDlg", "Show WaveFront file names", 0));
        bgColor->setText(QString());
        bakcgroundpb->setText(QApplication::translate("StatsDlg", "Background Color", 0));
        csv->setText(QApplication::translate("StatsDlg", "Save as .csv", 0));
        pdf->setText(QApplication::translate("StatsDlg", "Save as pdf", 0));
        jpg->setText(QApplication::translate("StatsDlg", "Save plots as image", 0));
    } // retranslateUi

};

namespace Ui {
    class StatsDlg: public Ui_StatsDlg {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_STATSDLG_H
