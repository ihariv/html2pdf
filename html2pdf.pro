TEMPLATE = app

QT += webenginewidgets
QT += network
#target.path = /var/www/html2pdf
#INSTALLS += target

SOURCES += html2pdf.cpp \
    server.cpp

HEADERS += \
    html2pdf.h \
    server.h
