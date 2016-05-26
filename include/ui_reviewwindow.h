/********************************************************************************
** Form generated from reading UI file 'reviewwindow.ui'
**
** Created by: Qt User Interface Compiler version 5.3.2
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_REVIEWWINDOW_H
#define UI_REVIEWWINDOW_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QSplitter>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_reviewWindow
{
public:
    QVBoxLayout *verticalLayout_3;
    QSplitter *tobBottomSplitter;
    QWidget *widget;
    QVBoxLayout *verticalLayout;
    QSplitter *TopHSplitter;
    QWidget *widget_2;
    QVBoxLayout *verticalLayout_2;
    QSplitter *bottomHSplitter;

    void setupUi(QWidget *reviewWindow)
    {
        if (reviewWindow->objectName().isEmpty())
            reviewWindow->setObjectName(QStringLiteral("reviewWindow"));
        reviewWindow->resize(400, 300);
        verticalLayout_3 = new QVBoxLayout(reviewWindow);
        verticalLayout_3->setObjectName(QStringLiteral("verticalLayout_3"));
        tobBottomSplitter = new QSplitter(reviewWindow);
        tobBottomSplitter->setObjectName(QStringLiteral("tobBottomSplitter"));
        tobBottomSplitter->setOrientation(Qt::Vertical);
        widget = new QWidget(tobBottomSplitter);
        widget->setObjectName(QStringLiteral("widget"));
        verticalLayout = new QVBoxLayout(widget);
        verticalLayout->setObjectName(QStringLiteral("verticalLayout"));
        TopHSplitter = new QSplitter(widget);
        TopHSplitter->setObjectName(QStringLiteral("TopHSplitter"));
        TopHSplitter->setOrientation(Qt::Horizontal);

        verticalLayout->addWidget(TopHSplitter);

        tobBottomSplitter->addWidget(widget);
        widget_2 = new QWidget(tobBottomSplitter);
        widget_2->setObjectName(QStringLiteral("widget_2"));
        verticalLayout_2 = new QVBoxLayout(widget_2);
        verticalLayout_2->setObjectName(QStringLiteral("verticalLayout_2"));
        bottomHSplitter = new QSplitter(widget_2);
        bottomHSplitter->setObjectName(QStringLiteral("bottomHSplitter"));
        bottomHSplitter->setOrientation(Qt::Horizontal);

        verticalLayout_2->addWidget(bottomHSplitter);

        tobBottomSplitter->addWidget(widget_2);

        verticalLayout_3->addWidget(tobBottomSplitter);


        retranslateUi(reviewWindow);

        QMetaObject::connectSlotsByName(reviewWindow);
    } // setupUi

    void retranslateUi(QWidget *reviewWindow)
    {
        reviewWindow->setWindowTitle(QApplication::translate("reviewWindow", "Form", 0));
    } // retranslateUi

};

namespace Ui {
    class reviewWindow: public Ui_reviewWindow {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_REVIEWWINDOW_H
