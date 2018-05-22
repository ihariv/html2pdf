/****************************************************************************
**
** Copyright (C) 2017 The Qt Company Ltd.
** Contact: https://www.qt.io/licensing/
**
** This file is part of the examples of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:BSD$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and The Qt Company. For licensing terms
** and conditions see https://www.qt.io/terms-conditions. For further
** information use the contact form at https://www.qt.io/contact-us.
**
** BSD License Usage
** Alternatively, you may use this file under the terms of the BSD license
** as follows:
**
** "Redistribution and use in source and binary forms, with or without
** modification, are permitted provided that the following conditions are
** met:
**   * Redistributions of source code must retain the above copyright
**     notice, this list of conditions and the following disclaimer.
**   * Redistributions in binary form must reproduce the above copyright
**     notice, this list of conditions and the following disclaimer in
**     the documentation and/or other materials provided with the
**     distribution.
**   * Neither the name of The Qt Company Ltd nor the names of its
**     contributors may be used to endorse or promote products derived
**     from this software without specific prior written permission.
**
**
** THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
** "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
** LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
** A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
** OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
** SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
** LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
** DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
** THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
** (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
** OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE."
**
** $QT_END_LICENSE$
**
****************************************************************************/

#include <QApplication>
#include <QCommandLineParser>
#include <QFile>
#include <QTextStream>
#include <QWebEnginePage>
#include <QWebEngineProfile>
#include <QWebEngineCertificateError>

#include <functional>
#include "server.h"

using namespace std;
using namespace std::placeholders;

class WebPage : public QWebEnginePage
{
    Q_OBJECT

protected:
    bool certificateError(const QWebEngineCertificateError &error) override;

};


bool WebPage::certificateError(const QWebEngineCertificateError &error)
{
    if (error.isOverridable()) {

    }
    return true;
}

class Html2PdfConverter : public QObject
{
    Q_OBJECT
public:
//    explicit Html2PdfConverter(QString inputPath, QString outputPath);
    explicit Html2PdfConverter();
    int run();

private slots:
    void loadFinished(bool ok);
    void pdfPrintingFinished(const QString &filePath, bool success);
    void slotRun(QByteArray url, QByteArray html, QTcpSocket* socket);

public:
    QString m_inputPath;
    QString m_outputPath;
    QScopedPointer<WebPage> m_page;
    QTcpSocket* TCPsocket;
};

Html2PdfConverter::Html2PdfConverter()
    : m_page(new WebPage)
{
    connect(m_page.data(), &WebPage::loadFinished,
            this, &Html2PdfConverter::loadFinished);
    connect(m_page.data(), &WebPage::pdfPrintingFinished,
            this, &Html2PdfConverter::pdfPrintingFinished);
}

int Html2PdfConverter::run()
{
    m_page->load(QUrl::fromUserInput(m_inputPath));
    return QApplication::exec();
}

void Html2PdfConverter::slotRun(QByteArray url, QByteArray html, QTcpSocket* socket)
{
    TCPsocket = socket;
    m_outputPath = "/tmp/test.pdf";
    m_inputPath = url;
    //m_page.reset(new WebPage);

    QString preFile = "/tmp/pdf_export_";
    int i = 0;
    do {
        i++;
        m_outputPath = preFile + QString::number(i);

        } while (QFile::exists(m_outputPath));

    if (html==""){
        m_page->load(QUrl::fromUserInput(m_inputPath));
    } else {
        m_page->setHtml(html);
    }

    QApplication::exec();
}

void Html2PdfConverter::loadFinished(bool ok)
{
    if (!ok) {
        QTextStream(stderr)
            << tr("failed to load URL '%1'").arg(m_inputPath) << "\n";

        TCPsocket->flush();
        TCPsocket->close();
        return;
    }

    m_page->printToPdf(m_outputPath);
}

void Html2PdfConverter::pdfPrintingFinished(const QString &filePath, bool success)
{
    if (!success) {
        QTextStream(stderr)
            << tr("failed to print to output file '%1'").arg(filePath) << "\n";
    } else {
        QFile loadFile(m_outputPath);
        if (!loadFile.open(QIODevice::ReadOnly)) {
            qWarning("Couldn't open save file.");

            TCPsocket->flush();
            TCPsocket->close();
            return;
        }
        QByteArray outData = loadFile.readAll();
        TCPsocket->write(outData);
        loadFile.remove();
        TCPsocket->flush();
        TCPsocket->close();

    }
}

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    QCoreApplication::setApplicationName("html2pdf");
    QCoreApplication::setApplicationVersion(QT_VERSION_STR);

    Server server;


    Html2PdfConverter converter;

    QObject::connect(&server, SIGNAL(severDataIn(QByteArray,QByteArray,QTcpSocket*)),
               &converter, SLOT(slotRun(QByteArray,QByteArray,QTcpSocket*)));
//    Html2PdfConverter converter(requiredArguments.at(0), requiredArguments.at(1));
//    return converter.run();
    return app.exec();
}

#include "html2pdf.moc"
