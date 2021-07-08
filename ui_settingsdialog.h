/********************************************************************************
** Form generated from reading UI file 'settingsdialog.ui'
**
** Created by: Qt User Interface Compiler version 5.15.2
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_SETTINGSDIALOG_H
#define UI_SETTINGSDIALOG_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QCheckBox>
#include <QtWidgets/QComboBox>
#include <QtWidgets/QDialog>
#include <QtWidgets/QDialogButtonBox>
#include <QtWidgets/QFrame>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QSpacerItem>
#include <QtWidgets/QTabWidget>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_SettingsDialog
{
public:
    QDialogButtonBox *buttonBox;
    QTabWidget *tabWidget;
    QWidget *general;
    QWidget *gridLayoutWidget;
    QGridLayout *gridLayout;
    QLabel *label_4;
    QLabel *label;
    QSpacerItem *verticalSpacer;
    QLineEdit *serverIp;
    QLabel *label_2;
    QLabel *label_3;
    QCheckBox *serverAutoConnect;
    QSpacerItem *horizontalSpacer;
    QLineEdit *serverPort;
    QLabel *label_5;
    QCheckBox *cbShadeUnavailable;
    QPushButton *pbShadeColor;
    QLabel *label_6;
    QCheckBox *cbTheme;
    QWidget *maps;
    QFrame *frame_3;
    QWidget *gridLayoutWidget_2;
    QGridLayout *gridLayout_10;
    QLabel *label_7;
    QLabel *label_8;
    QLabel *label_43;
    QPushButton *pbBrowse;
    QLineEdit *mapFile;
    QLineEdit *gridLocator;
    QLabel *label_9;
    QCheckBox *cbNightShade;
    QLabel *label_10;
    QComboBox *cbNightTransparency;
    QLabel *label_184;

    void setupUi(QDialog *SettingsDialog)
    {
        if (SettingsDialog->objectName().isEmpty())
            SettingsDialog->setObjectName(QString::fromUtf8("SettingsDialog"));
        SettingsDialog->resize(385, 300);
        buttonBox = new QDialogButtonBox(SettingsDialog);
        buttonBox->setObjectName(QString::fromUtf8("buttonBox"));
        buttonBox->setGeometry(QRect(35, 275, 341, 22));
        buttonBox->setOrientation(Qt::Horizontal);
        buttonBox->setStandardButtons(QDialogButtonBox::Cancel|QDialogButtonBox::Ok);
        tabWidget = new QTabWidget(SettingsDialog);
        tabWidget->setObjectName(QString::fromUtf8("tabWidget"));
        tabWidget->setGeometry(QRect(5, 5, 376, 261));
        general = new QWidget();
        general->setObjectName(QString::fromUtf8("general"));
        gridLayoutWidget = new QWidget(general);
        gridLayoutWidget->setObjectName(QString::fromUtf8("gridLayoutWidget"));
        gridLayoutWidget->setGeometry(QRect(0, 15, 371, 196));
        gridLayout = new QGridLayout(gridLayoutWidget);
        gridLayout->setObjectName(QString::fromUtf8("gridLayout"));
        gridLayout->setContentsMargins(0, 0, 0, 0);
        label_4 = new QLabel(gridLayoutWidget);
        label_4->setObjectName(QString::fromUtf8("label_4"));

        gridLayout->addWidget(label_4, 4, 0, 1, 1);

        label = new QLabel(gridLayoutWidget);
        label->setObjectName(QString::fromUtf8("label"));

        gridLayout->addWidget(label, 0, 0, 1, 1);

        verticalSpacer = new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding);

        gridLayout->addItem(verticalSpacer, 6, 1, 1, 1);

        serverIp = new QLineEdit(gridLayoutWidget);
        serverIp->setObjectName(QString::fromUtf8("serverIp"));

        gridLayout->addWidget(serverIp, 0, 1, 1, 1);

        label_2 = new QLabel(gridLayoutWidget);
        label_2->setObjectName(QString::fromUtf8("label_2"));

        gridLayout->addWidget(label_2, 1, 0, 1, 1);

        label_3 = new QLabel(gridLayoutWidget);
        label_3->setObjectName(QString::fromUtf8("label_3"));

        gridLayout->addWidget(label_3, 2, 0, 1, 1);

        serverAutoConnect = new QCheckBox(gridLayoutWidget);
        serverAutoConnect->setObjectName(QString::fromUtf8("serverAutoConnect"));

        gridLayout->addWidget(serverAutoConnect, 2, 1, 1, 1);

        horizontalSpacer = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        gridLayout->addItem(horizontalSpacer, 6, 2, 1, 1);

        serverPort = new QLineEdit(gridLayoutWidget);
        serverPort->setObjectName(QString::fromUtf8("serverPort"));

        gridLayout->addWidget(serverPort, 1, 1, 1, 1);

        label_5 = new QLabel(gridLayoutWidget);
        label_5->setObjectName(QString::fromUtf8("label_5"));

        gridLayout->addWidget(label_5, 3, 0, 1, 1);

        cbShadeUnavailable = new QCheckBox(gridLayoutWidget);
        cbShadeUnavailable->setObjectName(QString::fromUtf8("cbShadeUnavailable"));

        gridLayout->addWidget(cbShadeUnavailable, 4, 1, 1, 1);

        pbShadeColor = new QPushButton(gridLayoutWidget);
        pbShadeColor->setObjectName(QString::fromUtf8("pbShadeColor"));
        QSizePolicy sizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(pbShadeColor->sizePolicy().hasHeightForWidth());
        pbShadeColor->setSizePolicy(sizePolicy);
        pbShadeColor->setMinimumSize(QSize(30, 20));
        pbShadeColor->setMaximumSize(QSize(30, 25));

        gridLayout->addWidget(pbShadeColor, 3, 1, 1, 1);

        label_6 = new QLabel(gridLayoutWidget);
        label_6->setObjectName(QString::fromUtf8("label_6"));

        gridLayout->addWidget(label_6, 5, 0, 1, 1);

        cbTheme = new QCheckBox(gridLayoutWidget);
        cbTheme->setObjectName(QString::fromUtf8("cbTheme"));

        gridLayout->addWidget(cbTheme, 5, 1, 1, 1);

        tabWidget->addTab(general, QString());
        maps = new QWidget();
        maps->setObjectName(QString::fromUtf8("maps"));
        frame_3 = new QFrame(maps);
        frame_3->setObjectName(QString::fromUtf8("frame_3"));
        frame_3->setGeometry(QRect(5, 10, 361, 179));
        frame_3->setFrameShape(QFrame::StyledPanel);
        frame_3->setFrameShadow(QFrame::Raised);
        gridLayoutWidget_2 = new QWidget(frame_3);
        gridLayoutWidget_2->setObjectName(QString::fromUtf8("gridLayoutWidget_2"));
        gridLayoutWidget_2->setGeometry(QRect(6, 42, 351, 111));
        gridLayout_10 = new QGridLayout(gridLayoutWidget_2);
        gridLayout_10->setObjectName(QString::fromUtf8("gridLayout_10"));
        gridLayout_10->setContentsMargins(0, 0, 0, 0);
        label_7 = new QLabel(gridLayoutWidget_2);
        label_7->setObjectName(QString::fromUtf8("label_7"));
        label_7->setAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);

        gridLayout_10->addWidget(label_7, 1, 0, 1, 1);

        label_8 = new QLabel(gridLayoutWidget_2);
        label_8->setObjectName(QString::fromUtf8("label_8"));

        gridLayout_10->addWidget(label_8, 1, 2, 1, 1);

        label_43 = new QLabel(gridLayoutWidget_2);
        label_43->setObjectName(QString::fromUtf8("label_43"));
        label_43->setAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);

        gridLayout_10->addWidget(label_43, 0, 0, 1, 1);

        pbBrowse = new QPushButton(gridLayoutWidget_2);
        pbBrowse->setObjectName(QString::fromUtf8("pbBrowse"));
        pbBrowse->setEnabled(true);
        QSizePolicy sizePolicy1(QSizePolicy::Preferred, QSizePolicy::Preferred);
        sizePolicy1.setHorizontalStretch(0);
        sizePolicy1.setVerticalStretch(0);
        sizePolicy1.setHeightForWidth(pbBrowse->sizePolicy().hasHeightForWidth());
        pbBrowse->setSizePolicy(sizePolicy1);
        pbBrowse->setMinimumSize(QSize(0, 22));
        pbBrowse->setMaximumSize(QSize(80, 22));

        gridLayout_10->addWidget(pbBrowse, 0, 2, 1, 1);

        mapFile = new QLineEdit(gridLayoutWidget_2);
        mapFile->setObjectName(QString::fromUtf8("mapFile"));

        gridLayout_10->addWidget(mapFile, 0, 1, 1, 1);

        gridLocator = new QLineEdit(gridLayoutWidget_2);
        gridLocator->setObjectName(QString::fromUtf8("gridLocator"));

        gridLayout_10->addWidget(gridLocator, 1, 1, 1, 1);

        label_9 = new QLabel(gridLayoutWidget_2);
        label_9->setObjectName(QString::fromUtf8("label_9"));
        label_9->setAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);

        gridLayout_10->addWidget(label_9, 2, 0, 1, 1);

        cbNightShade = new QCheckBox(gridLayoutWidget_2);
        cbNightShade->setObjectName(QString::fromUtf8("cbNightShade"));

        gridLayout_10->addWidget(cbNightShade, 2, 1, 1, 1);

        label_10 = new QLabel(gridLayoutWidget_2);
        label_10->setObjectName(QString::fromUtf8("label_10"));

        gridLayout_10->addWidget(label_10, 3, 0, 1, 1);

        cbNightTransparency = new QComboBox(gridLayoutWidget_2);
        cbNightTransparency->setObjectName(QString::fromUtf8("cbNightTransparency"));

        gridLayout_10->addWidget(cbNightTransparency, 3, 1, 1, 1);

        label_184 = new QLabel(frame_3);
        label_184->setObjectName(QString::fromUtf8("label_184"));
        label_184->setGeometry(QRect(65, 5, 261, 35));
        QFont font;
        font.setBold(true);
        font.setWeight(75);
        label_184->setFont(font);
        label_184->setAlignment(Qt::AlignCenter);
        tabWidget->addTab(maps, QString());

        retranslateUi(SettingsDialog);
        QObject::connect(buttonBox, SIGNAL(rejected()), SettingsDialog, SLOT(reject()));
        QObject::connect(buttonBox, SIGNAL(accepted()), SettingsDialog, SLOT(accept()));

        tabWidget->setCurrentIndex(0);


        QMetaObject::connectSlotsByName(SettingsDialog);
    } // setupUi

    void retranslateUi(QDialog *SettingsDialog)
    {
        SettingsDialog->setWindowTitle(QCoreApplication::translate("SettingsDialog", "Dialog", nullptr));
        label_4->setText(QCoreApplication::translate("SettingsDialog", "Shade Unavailable", nullptr));
        label->setText(QCoreApplication::translate("SettingsDialog", "Server IP", nullptr));
        label_2->setText(QCoreApplication::translate("SettingsDialog", "Server Port", nullptr));
        label_3->setText(QCoreApplication::translate("SettingsDialog", "Auto-connect", nullptr));
        serverAutoConnect->setText(QString());
        label_5->setText(QCoreApplication::translate("SettingsDialog", "Shading Color", nullptr));
        cbShadeUnavailable->setText(QString());
        pbShadeColor->setText(QString());
        label_6->setText(QCoreApplication::translate("SettingsDialog", "Dark Theme", nullptr));
        cbTheme->setText(QString());
        tabWidget->setTabText(tabWidget->indexOf(general), QCoreApplication::translate("SettingsDialog", "General", nullptr));
        label_7->setText(QCoreApplication::translate("SettingsDialog", "Center Grid", nullptr));
        label_8->setText(QCoreApplication::translate("SettingsDialog", "eg. EN91WR", nullptr));
        label_43->setText(QCoreApplication::translate("SettingsDialog", "Map File", nullptr));
        pbBrowse->setText(QCoreApplication::translate("SettingsDialog", "Browse", nullptr));
        label_9->setText(QCoreApplication::translate("SettingsDialog", "Night Shading", nullptr));
        cbNightShade->setText(QString());
        label_10->setText(QCoreApplication::translate("SettingsDialog", "Night Transparency", nullptr));
        label_184->setText(QCoreApplication::translate("SettingsDialog", "Equirectangular Projection Map\n"
"(Plate Carree)", nullptr));
        tabWidget->setTabText(tabWidget->indexOf(maps), QCoreApplication::translate("SettingsDialog", "Maps", nullptr));
    } // retranslateUi

};

namespace Ui {
    class SettingsDialog: public Ui_SettingsDialog {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_SETTINGSDIALOG_H
