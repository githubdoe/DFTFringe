/********************************************************************************
** Form generated from reading UI file 'zernikedlg.ui'
**
** Created by: Qt User Interface Compiler version 5.3.2
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_ZERNIKEDLG_H
#define UI_ZERNIKEDLG_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QDialog>
#include <QtWidgets/QDialogButtonBox>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QSpacerItem>
#include <QtWidgets/QTableView>
#include <QtWidgets/QVBoxLayout>

QT_BEGIN_NAMESPACE

class Ui_zernikeDlg
{
public:
    QHBoxLayout *horizontalLayout;
    QTableView *tableView;
    QVBoxLayout *verticalLayout;
    QPushButton *clear;
    QPushButton *delete_2;
    QPushButton *enableAll;
    QPushButton *spherical;
    QSpacerItem *verticalSpacer;
    QDialogButtonBox *buttonBox;

    void setupUi(QDialog *zernikeDlg)
    {
        if (zernikeDlg->objectName().isEmpty())
            zernikeDlg->setObjectName(QStringLiteral("zernikeDlg"));
        zernikeDlg->resize(362, 557);
        horizontalLayout = new QHBoxLayout(zernikeDlg);
        horizontalLayout->setObjectName(QStringLiteral("horizontalLayout"));
        tableView = new QTableView(zernikeDlg);
        tableView->setObjectName(QStringLiteral("tableView"));
        tableView->setWordWrap(false);

        horizontalLayout->addWidget(tableView);

        verticalLayout = new QVBoxLayout();
        verticalLayout->setObjectName(QStringLiteral("verticalLayout"));
        clear = new QPushButton(zernikeDlg);
        clear->setObjectName(QStringLiteral("clear"));

        verticalLayout->addWidget(clear);

        delete_2 = new QPushButton(zernikeDlg);
        delete_2->setObjectName(QStringLiteral("delete_2"));

        verticalLayout->addWidget(delete_2);

        enableAll = new QPushButton(zernikeDlg);
        enableAll->setObjectName(QStringLiteral("enableAll"));

        verticalLayout->addWidget(enableAll);

        spherical = new QPushButton(zernikeDlg);
        spherical->setObjectName(QStringLiteral("spherical"));

        verticalLayout->addWidget(spherical);

        verticalSpacer = new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding);

        verticalLayout->addItem(verticalSpacer);

        buttonBox = new QDialogButtonBox(zernikeDlg);
        buttonBox->setObjectName(QStringLiteral("buttonBox"));
        buttonBox->setOrientation(Qt::Vertical);
        buttonBox->setStandardButtons(QDialogButtonBox::Cancel|QDialogButtonBox::Ok);

        verticalLayout->addWidget(buttonBox);


        horizontalLayout->addLayout(verticalLayout);


        retranslateUi(zernikeDlg);
        QObject::connect(buttonBox, SIGNAL(accepted()), zernikeDlg, SLOT(accept()));
        QObject::connect(buttonBox, SIGNAL(rejected()), zernikeDlg, SLOT(reject()));

        QMetaObject::connectSlotsByName(zernikeDlg);
    } // setupUi

    void retranslateUi(QDialog *zernikeDlg)
    {
        zernikeDlg->setWindowTitle(QApplication::translate("zernikeDlg", "Computed Zernike Values", 0));
        clear->setText(QApplication::translate("zernikeDlg", "Clear All", 0));
        delete_2->setText(QApplication::translate("zernikeDlg", "Delete All", 0));
        enableAll->setText(QApplication::translate("zernikeDlg", "Enable All", 0));
        spherical->setText(QApplication::translate("zernikeDlg", "Spherical Only", 0));
    } // retranslateUi

};

namespace Ui {
    class zernikeDlg: public Ui_zernikeDlg {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_ZERNIKEDLG_H
