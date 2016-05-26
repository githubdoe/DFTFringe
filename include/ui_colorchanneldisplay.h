/********************************************************************************
** Form generated from reading UI file 'colorchanneldisplay.ui'
**
** Created by: Qt User Interface Compiler version 5.3.2
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_COLORCHANNELDISPLAY_H
#define UI_COLORCHANNELDISPLAY_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QDialog>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLabel>

QT_BEGIN_NAMESPACE

class Ui_ColorChannelDisplay
{
public:
    QHBoxLayout *horizontalLayout_2;
    QHBoxLayout *horizontalLayout;
    QLabel *red;
    QLabel *green;
    QLabel *blue;

    void setupUi(QDialog *ColorChannelDisplay)
    {
        if (ColorChannelDisplay->objectName().isEmpty())
            ColorChannelDisplay->setObjectName(QStringLiteral("ColorChannelDisplay"));
        ColorChannelDisplay->resize(926, 324);
        horizontalLayout_2 = new QHBoxLayout(ColorChannelDisplay);
        horizontalLayout_2->setObjectName(QStringLiteral("horizontalLayout_2"));
        horizontalLayout = new QHBoxLayout();
        horizontalLayout->setObjectName(QStringLiteral("horizontalLayout"));
        red = new QLabel(ColorChannelDisplay);
        red->setObjectName(QStringLiteral("red"));
        red->setMinimumSize(QSize(300, 300));

        horizontalLayout->addWidget(red);

        green = new QLabel(ColorChannelDisplay);
        green->setObjectName(QStringLiteral("green"));
        green->setMinimumSize(QSize(300, 300));

        horizontalLayout->addWidget(green);

        blue = new QLabel(ColorChannelDisplay);
        blue->setObjectName(QStringLiteral("blue"));
        blue->setEnabled(true);
        blue->setMinimumSize(QSize(0, 300));

        horizontalLayout->addWidget(blue);


        horizontalLayout_2->addLayout(horizontalLayout);


        retranslateUi(ColorChannelDisplay);

        QMetaObject::connectSlotsByName(ColorChannelDisplay);
    } // setupUi

    void retranslateUi(QDialog *ColorChannelDisplay)
    {
        ColorChannelDisplay->setWindowTitle(QApplication::translate("ColorChannelDisplay", "Igram color channels", 0));
#ifndef QT_NO_WHATSTHIS
        red->setWhatsThis(QApplication::translate("ColorChannelDisplay", "Blue channel", 0));
#endif // QT_NO_WHATSTHIS
        red->setText(QString());
#ifndef QT_NO_WHATSTHIS
        green->setWhatsThis(QApplication::translate("ColorChannelDisplay", "Green channel", 0));
#endif // QT_NO_WHATSTHIS
        green->setText(QString());
#ifndef QT_NO_WHATSTHIS
        blue->setWhatsThis(QApplication::translate("ColorChannelDisplay", "Red channel", 0));
#endif // QT_NO_WHATSTHIS
        blue->setText(QString());
    } // retranslateUi

};

namespace Ui {
    class ColorChannelDisplay: public Ui_ColorChannelDisplay {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_COLORCHANNELDISPLAY_H
