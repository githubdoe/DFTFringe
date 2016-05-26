/********************************************************************************
** Form generated from reading UI file 'metricsdisplay.ui'
**
** Created by: Qt User Interface Compiler version 5.3.2
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_METRICSDISPLAY_H
#define UI_METRICSDISPLAY_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QDockWidget>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLabel>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QSpacerItem>
#include <QtWidgets/QTableView>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_metricsDisplay
{
public:
    QWidget *dockWidgetContents;
    QVBoxLayout *verticalLayout;
    QLabel *wftName;
    QHBoxLayout *horizontalLayout;
    QLabel *label;
    QLabel *metDiam;
    QHBoxLayout *horizontalLayout_2;
    QLabel *label_3;
    QLabel *metROC;
    QHBoxLayout *horizontalLayout_3;
    QLabel *label_5;
    QLabel *metRMS;
    QHBoxLayout *horizontalLayout_4;
    QLabel *label_7;
    QLabel *metStrehl;
    QHBoxLayout *horizontalLayout_5;
    QLabel *label_2;
    QLabel *metConic;
    QLabel *wavesPerFringe;
    QLabel *lambda;
    QSpacerItem *verticalSpacer;
    QHBoxLayout *horizontalLayout_6;
    QPushButton *recomputePB;
    QPushButton *DisableAll;
    QPushButton *sphericalPb;
    QLabel *label_4;
    QTableView *tableView;

    void setupUi(QDockWidget *metricsDisplay)
    {
        if (metricsDisplay->objectName().isEmpty())
            metricsDisplay->setObjectName(QStringLiteral("metricsDisplay"));
        metricsDisplay->resize(176, 710);
        metricsDisplay->setMinimumSize(QSize(176, 389));
        dockWidgetContents = new QWidget();
        dockWidgetContents->setObjectName(QStringLiteral("dockWidgetContents"));
        verticalLayout = new QVBoxLayout(dockWidgetContents);
        verticalLayout->setObjectName(QStringLiteral("verticalLayout"));
        wftName = new QLabel(dockWidgetContents);
        wftName->setObjectName(QStringLiteral("wftName"));
        wftName->setWordWrap(true);

        verticalLayout->addWidget(wftName);

        horizontalLayout = new QHBoxLayout();
        horizontalLayout->setObjectName(QStringLiteral("horizontalLayout"));
        label = new QLabel(dockWidgetContents);
        label->setObjectName(QStringLiteral("label"));
        label->setFrameShape(QFrame::NoFrame);
        label->setFrameShadow(QFrame::Plain);
        label->setAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);

        horizontalLayout->addWidget(label);

        metDiam = new QLabel(dockWidgetContents);
        metDiam->setObjectName(QStringLiteral("metDiam"));
        metDiam->setEnabled(false);
        metDiam->setFrameShape(QFrame::Panel);
        metDiam->setFrameShadow(QFrame::Sunken);

        horizontalLayout->addWidget(metDiam);


        verticalLayout->addLayout(horizontalLayout);

        horizontalLayout_2 = new QHBoxLayout();
        horizontalLayout_2->setObjectName(QStringLiteral("horizontalLayout_2"));
        label_3 = new QLabel(dockWidgetContents);
        label_3->setObjectName(QStringLiteral("label_3"));
        label_3->setAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);

        horizontalLayout_2->addWidget(label_3);

        metROC = new QLabel(dockWidgetContents);
        metROC->setObjectName(QStringLiteral("metROC"));
        metROC->setEnabled(false);
        metROC->setFrameShape(QFrame::Panel);
        metROC->setFrameShadow(QFrame::Sunken);

        horizontalLayout_2->addWidget(metROC);


        verticalLayout->addLayout(horizontalLayout_2);

        horizontalLayout_3 = new QHBoxLayout();
        horizontalLayout_3->setObjectName(QStringLiteral("horizontalLayout_3"));
        label_5 = new QLabel(dockWidgetContents);
        label_5->setObjectName(QStringLiteral("label_5"));
        label_5->setTextFormat(Qt::RichText);
        label_5->setAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);
        label_5->setWordWrap(true);

        horizontalLayout_3->addWidget(label_5);

        metRMS = new QLabel(dockWidgetContents);
        metRMS->setObjectName(QStringLiteral("metRMS"));
        metRMS->setEnabled(false);
        metRMS->setFrameShape(QFrame::Panel);
        metRMS->setFrameShadow(QFrame::Sunken);

        horizontalLayout_3->addWidget(metRMS);


        verticalLayout->addLayout(horizontalLayout_3);

        horizontalLayout_4 = new QHBoxLayout();
        horizontalLayout_4->setObjectName(QStringLiteral("horizontalLayout_4"));
        label_7 = new QLabel(dockWidgetContents);
        label_7->setObjectName(QStringLiteral("label_7"));
        label_7->setAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);

        horizontalLayout_4->addWidget(label_7);

        metStrehl = new QLabel(dockWidgetContents);
        metStrehl->setObjectName(QStringLiteral("metStrehl"));
        metStrehl->setEnabled(false);
        metStrehl->setFrameShape(QFrame::Panel);
        metStrehl->setFrameShadow(QFrame::Sunken);
        metStrehl->setAlignment(Qt::AlignLeading|Qt::AlignLeft|Qt::AlignVCenter);

        horizontalLayout_4->addWidget(metStrehl);


        verticalLayout->addLayout(horizontalLayout_4);

        horizontalLayout_5 = new QHBoxLayout();
        horizontalLayout_5->setObjectName(QStringLiteral("horizontalLayout_5"));
        label_2 = new QLabel(dockWidgetContents);
        label_2->setObjectName(QStringLiteral("label_2"));

        horizontalLayout_5->addWidget(label_2);

        metConic = new QLabel(dockWidgetContents);
        metConic->setObjectName(QStringLiteral("metConic"));
        metConic->setEnabled(false);
        metConic->setFrameShape(QFrame::Panel);
        metConic->setFrameShadow(QFrame::Sunken);

        horizontalLayout_5->addWidget(metConic);


        verticalLayout->addLayout(horizontalLayout_5);

        wavesPerFringe = new QLabel(dockWidgetContents);
        wavesPerFringe->setObjectName(QStringLiteral("wavesPerFringe"));

        verticalLayout->addWidget(wavesPerFringe);

        lambda = new QLabel(dockWidgetContents);
        lambda->setObjectName(QStringLiteral("lambda"));

        verticalLayout->addWidget(lambda);

        verticalSpacer = new QSpacerItem(20, 10, QSizePolicy::Minimum, QSizePolicy::Fixed);

        verticalLayout->addItem(verticalSpacer);

        horizontalLayout_6 = new QHBoxLayout();
        horizontalLayout_6->setObjectName(QStringLiteral("horizontalLayout_6"));
        recomputePB = new QPushButton(dockWidgetContents);
        recomputePB->setObjectName(QStringLiteral("recomputePB"));
        recomputePB->setMinimumSize(QSize(0, 0));

        horizontalLayout_6->addWidget(recomputePB);

        DisableAll = new QPushButton(dockWidgetContents);
        DisableAll->setObjectName(QStringLiteral("DisableAll"));

        horizontalLayout_6->addWidget(DisableAll);


        verticalLayout->addLayout(horizontalLayout_6);

        sphericalPb = new QPushButton(dockWidgetContents);
        sphericalPb->setObjectName(QStringLiteral("sphericalPb"));

        verticalLayout->addWidget(sphericalPb);

        label_4 = new QLabel(dockWidgetContents);
        label_4->setObjectName(QStringLiteral("label_4"));

        verticalLayout->addWidget(label_4);

        tableView = new QTableView(dockWidgetContents);
        tableView->setObjectName(QStringLiteral("tableView"));

        verticalLayout->addWidget(tableView);

        metricsDisplay->setWidget(dockWidgetContents);

        retranslateUi(metricsDisplay);

        QMetaObject::connectSlotsByName(metricsDisplay);
    } // setupUi

    void retranslateUi(QDockWidget *metricsDisplay)
    {
        metricsDisplay->setWindowTitle(QApplication::translate("metricsDisplay", "Metrics", 0));
        wftName->setText(QApplication::translate("metricsDisplay", "TextLabel", 0));
        label->setText(QApplication::translate("metricsDisplay", "Diameter:", 0));
        metDiam->setText(QString());
        label_3->setText(QApplication::translate("metricsDisplay", "Roc:", 0));
        metROC->setText(QString());
        label_5->setText(QApplication::translate("metricsDisplay", "Wavefront RMS at 550 nm", 0));
        metRMS->setText(QString());
        label_7->setText(QApplication::translate("metricsDisplay", "Strehl:", 0));
        metStrehl->setText(QString());
        label_2->setText(QApplication::translate("metricsDisplay", "Best Fit\n"
"Conic:", 0));
        metConic->setText(QString());
        wavesPerFringe->setText(QApplication::translate("metricsDisplay", "Waves per Fringe:", 0));
        lambda->setText(QApplication::translate("metricsDisplay", "Igram Wave Length:", 0));
        recomputePB->setText(QApplication::translate("metricsDisplay", "recompute", 0));
        DisableAll->setText(QApplication::translate("metricsDisplay", "Disable ALL", 0));
        sphericalPb->setText(QApplication::translate("metricsDisplay", "Enable Spherical only", 0));
        label_4->setText(QApplication::translate("metricsDisplay", "Zernike Values", 0));
    } // retranslateUi

};

namespace Ui {
    class metricsDisplay: public Ui_metricsDisplay {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_METRICSDISPLAY_H
