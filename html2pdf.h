#ifndef HTML2PDF_H
#define HTML2PDF_H

#include <QApplication>
#include <QCommandLineParser>
#include <QFile>
#include <QTextStream>
#include <QWebEnginePage>
#include <QWebEngineProfile>
#include <QWebEngineCertificateError>

//using namespace std;
//using namespace std::placeholders;

//class WebPage : public QWebEnginePage
//{
//    Q_OBJECT

//protected:
//    bool certificateError(const QWebEngineCertificateError &error) override;

//};

//class Html2PdfConverter : public QObject
//{
//    Q_OBJECT
//public:
//    explicit Html2PdfConverter();
//    int run();

//private slots:
//    void loadFinished(bool ok);
//    void pdfPrintingFinished(const QString &filePath, bool success);
//    void slotRun(QByteArray url, QByteArray html, QTcpSocket* socket);

//public:
//    QString m_inputPath;
//    QString m_outputPath;
//    QScopedPointer<WebPage> m_page;
//    QTcpSocket* TCPsocket;
//};

#endif // HTML2PDF_H
