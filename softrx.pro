QT += core gui network widgets websockets

TARGET = softrx-client
TEMPLATE = app

SOURCES += main.cpp \
	radiowindow.cpp \
	settingsdialog.cpp \

HEADERS  += defines.h \
	radiowindow.h \
	settingsdialog.h \

FORMS    += radio.ui \
	settingsdialog.ui \

unix {
  QT += pdf
}

win32-g++ {
    QT += xml
    CONFIG += link_pkgconfig
    PKGCONFIG += poppler-qt5 libopenjp2
}

