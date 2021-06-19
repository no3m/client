/********************************************************************************
** Form generated from reading UI file 'radio.ui'
**
** Created by: Qt User Interface Compiler version 5.15.2
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_RADIO_H
#define UI_RADIO_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QComboBox>
#include <QtWidgets/QFrame>
#include <QtWidgets/QLabel>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_RadioWindow
{
public:
    QLabel *radioName;
    QComboBox *cbScanDelay;
    QPushButton *pbLock;
    QPushButton *pbSettings;
    QPushButton *pbScan;
    QLabel *connectionStatus;
    QComboBox *cbGroup;
    QLabel *pttLabel;
    QComboBox *cbBand;
    QFrame *frame;
    QPushButton *pbTrack;
    QLabel *bearingLabel;
    QComboBox *cbLinked;
    QLabel *hpfLabel;
    QLabel *bpfLabel;
    QLabel *auxLabel;
    QLabel *gainLabel;
    QPushButton *pbScanEnabled;
    QPushButton *pbSwapAntennas;

    void setupUi(QWidget *RadioWindow)
    {
        if (RadioWindow->objectName().isEmpty())
            RadioWindow->setObjectName(QString::fromUtf8("RadioWindow"));
        RadioWindow->resize(340, 360);
        QSizePolicy sizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(RadioWindow->sizePolicy().hasHeightForWidth());
        RadioWindow->setSizePolicy(sizePolicy);
        RadioWindow->setMinimumSize(QSize(340, 360));
        RadioWindow->setMaximumSize(QSize(340, 360));
        radioName = new QLabel(RadioWindow);
        radioName->setObjectName(QString::fromUtf8("radioName"));
        radioName->setGeometry(QRect(6, 5, 156, 20));
        QFont font;
        font.setPointSize(10);
        font.setBold(true);
        font.setWeight(75);
        radioName->setFont(font);
        radioName->setAlignment(Qt::AlignLeading|Qt::AlignLeft|Qt::AlignVCenter);
        cbScanDelay = new QComboBox(RadioWindow);
        cbScanDelay->setObjectName(QString::fromUtf8("cbScanDelay"));
        cbScanDelay->setGeometry(QRect(76, 332, 68, 24));
        pbLock = new QPushButton(RadioWindow);
        pbLock->setObjectName(QString::fromUtf8("pbLock"));
        pbLock->setGeometry(QRect(167, 30, 70, 24));
        pbSettings = new QPushButton(RadioWindow);
        pbSettings->setObjectName(QString::fromUtf8("pbSettings"));
        pbSettings->setGeometry(QRect(238, 30, 68, 24));
        pbScan = new QPushButton(RadioWindow);
        pbScan->setObjectName(QString::fromUtf8("pbScan"));
        pbScan->setGeometry(QRect(5, 332, 70, 24));
        connectionStatus = new QLabel(RadioWindow);
        connectionStatus->setObjectName(QString::fromUtf8("connectionStatus"));
        connectionStatus->setGeometry(QRect(316, 338, 12, 12));
        sizePolicy.setHeightForWidth(connectionStatus->sizePolicy().hasHeightForWidth());
        connectionStatus->setSizePolicy(sizePolicy);
        connectionStatus->setMinimumSize(QSize(12, 12));
        connectionStatus->setMaximumSize(QSize(12, 12));
        cbGroup = new QComboBox(RadioWindow);
        cbGroup->setObjectName(QString::fromUtf8("cbGroup"));
        cbGroup->setGeometry(QRect(76, 30, 90, 24));
        pttLabel = new QLabel(RadioWindow);
        pttLabel->setObjectName(QString::fromUtf8("pttLabel"));
        pttLabel->setGeometry(QRect(306, 34, 35, 16));
        QSizePolicy sizePolicy1(QSizePolicy::Preferred, QSizePolicy::Preferred);
        sizePolicy1.setHorizontalStretch(0);
        sizePolicy1.setVerticalStretch(0);
        sizePolicy1.setHeightForWidth(pttLabel->sizePolicy().hasHeightForWidth());
        pttLabel->setSizePolicy(sizePolicy1);
        pttLabel->setMinimumSize(QSize(0, 0));
        pttLabel->setMaximumSize(QSize(10000, 10000));
        QFont font1;
        font1.setPointSize(9);
        font1.setBold(true);
        font1.setWeight(75);
        pttLabel->setFont(font1);
        pttLabel->setAlignment(Qt::AlignCenter);
        cbBand = new QComboBox(RadioWindow);
        cbBand->setObjectName(QString::fromUtf8("cbBand"));
        cbBand->setGeometry(QRect(5, 30, 70, 24));
        frame = new QFrame(RadioWindow);
        frame->setObjectName(QString::fromUtf8("frame"));
        frame->setGeometry(QRect(5, 58, 300, 270));
        frame->setFrameShape(QFrame::StyledPanel);
        frame->setFrameShadow(QFrame::Raised);
        pbTrack = new QPushButton(RadioWindow);
        pbTrack->setObjectName(QString::fromUtf8("pbTrack"));
        pbTrack->setGeometry(QRect(145, 332, 70, 24));
        bearingLabel = new QLabel(RadioWindow);
        bearingLabel->setObjectName(QString::fromUtf8("bearingLabel"));
        bearingLabel->setGeometry(QRect(246, 5, 61, 21));
        bearingLabel->setFont(font1);
        bearingLabel->setAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);
        cbLinked = new QComboBox(RadioWindow);
        cbLinked->setObjectName(QString::fromUtf8("cbLinked"));
        cbLinked->setGeometry(QRect(216, 332, 90, 24));
        hpfLabel = new QLabel(RadioWindow);
        hpfLabel->setObjectName(QString::fromUtf8("hpfLabel"));
        hpfLabel->setGeometry(QRect(306, 64, 35, 16));
        sizePolicy1.setHeightForWidth(hpfLabel->sizePolicy().hasHeightForWidth());
        hpfLabel->setSizePolicy(sizePolicy1);
        hpfLabel->setMinimumSize(QSize(0, 0));
        hpfLabel->setMaximumSize(QSize(10000, 10000));
        QFont font2;
        font2.setPointSize(7);
        font2.setBold(true);
        font2.setWeight(75);
        hpfLabel->setFont(font2);
        hpfLabel->setAlignment(Qt::AlignCenter);
        bpfLabel = new QLabel(RadioWindow);
        bpfLabel->setObjectName(QString::fromUtf8("bpfLabel"));
        bpfLabel->setGeometry(QRect(306, 82, 35, 16));
        sizePolicy1.setHeightForWidth(bpfLabel->sizePolicy().hasHeightForWidth());
        bpfLabel->setSizePolicy(sizePolicy1);
        bpfLabel->setMinimumSize(QSize(0, 0));
        bpfLabel->setMaximumSize(QSize(10000, 10000));
        bpfLabel->setFont(font2);
        bpfLabel->setAlignment(Qt::AlignCenter);
        auxLabel = new QLabel(RadioWindow);
        auxLabel->setObjectName(QString::fromUtf8("auxLabel"));
        auxLabel->setGeometry(QRect(306, 100, 35, 16));
        sizePolicy1.setHeightForWidth(auxLabel->sizePolicy().hasHeightForWidth());
        auxLabel->setSizePolicy(sizePolicy1);
        auxLabel->setMinimumSize(QSize(0, 0));
        auxLabel->setMaximumSize(QSize(10000, 10000));
        auxLabel->setFont(font2);
        auxLabel->setAlignment(Qt::AlignCenter);
        gainLabel = new QLabel(RadioWindow);
        gainLabel->setObjectName(QString::fromUtf8("gainLabel"));
        gainLabel->setGeometry(QRect(306, 138, 35, 16));
        gainLabel->setFont(font2);
        gainLabel->setAlignment(Qt::AlignCenter);
        pbScanEnabled = new QPushButton(RadioWindow);
        pbScanEnabled->setObjectName(QString::fromUtf8("pbScanEnabled"));
        pbScanEnabled->setGeometry(QRect(307, 295, 32, 33));
        QFont font3;
        font3.setPointSize(7);
        font3.setBold(false);
        font3.setWeight(50);
        pbScanEnabled->setFont(font3);
        pbSwapAntennas = new QPushButton(RadioWindow);
        pbSwapAntennas->setObjectName(QString::fromUtf8("pbSwapAntennas"));
        pbSwapAntennas->setGeometry(QRect(307, 258, 32, 33));
        pbSwapAntennas->setFont(font3);

        retranslateUi(RadioWindow);

        QMetaObject::connectSlotsByName(RadioWindow);
    } // setupUi

    void retranslateUi(QWidget *RadioWindow)
    {
        RadioWindow->setWindowTitle(QCoreApplication::translate("RadioWindow", "Form", nullptr));
        radioName->setText(QString());
        pbLock->setText(QCoreApplication::translate("RadioWindow", "Lock", nullptr));
        pbSettings->setText(QCoreApplication::translate("RadioWindow", "Settings", nullptr));
        pbScan->setText(QCoreApplication::translate("RadioWindow", "Scan", nullptr));
        connectionStatus->setText(QString());
        pttLabel->setText(QString());
        pbTrack->setText(QCoreApplication::translate("RadioWindow", "Track", nullptr));
        bearingLabel->setText(QString());
        hpfLabel->setText(QString());
        bpfLabel->setText(QString());
        auxLabel->setText(QString());
        gainLabel->setText(QString());
        pbScanEnabled->setText(QCoreApplication::translate("RadioWindow", "Scan\n"
"Flag", nullptr));
        pbSwapAntennas->setText(QCoreApplication::translate("RadioWindow", "Swap\n"
"ANT", nullptr));
    } // retranslateUi

};

namespace Ui {
    class RadioWindow: public Ui_RadioWindow {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_RADIO_H
