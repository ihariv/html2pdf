#include "server.h"
#include <QDebug>
#include <QCoreApplication>
#include <QJsonArray>
#include <QJsonDocument>
#include <QTextStream>

Server::Server(QObject *parent) : QObject(parent)
{
    mTcpServer = new QTcpServer(this);

    connect(mTcpServer, &QTcpServer::newConnection, this, &Server::slotNewConnection);

    if(!mTcpServer->listen(QHostAddress::Any, 6000)){
        qDebug() << "server is not started";
    } else {
        qDebug() << "server is started";
    }
}

void Server::slotNewConnection()
{
    mTcpSocket = mTcpServer->nextPendingConnection();

    mTcpSocket->write("Hello, World!!! I am echo server!\r\n");

    connect(mTcpSocket, &QTcpSocket::readyRead, this, &Server::slotServerRead);
    connect(mTcpSocket, &QTcpSocket::disconnected, this, &Server::slotClientDisconnected);
}

void Server::slotServerRead()
{
    while(mTcpSocket->bytesAvailable()>0)
    {

        QByteArray json = mTcpSocket->readAll();

        QJsonDocument loadDoc(QJsonDocument::fromJson(json));
        //read(loadDoc.object());

        QByteArray url;
        url += loadDoc["Url"].toString().toUtf8();
        QByteArray html;
        html += loadDoc["Html"].toString();
        QTextStream(stdout) << "Loaded save for "
                                << loadDoc["Url"].toString()
                                << " using "
                                << "JSON...\n";


        emit severDataIn(url,html,mTcpSocket);

        //Html2PdfConverter converter(url, "test.pdf");
        //mTcpSocket->write(url);
    }
}

void Server::slotClientDisconnected()
{
    mTcpSocket->close();
}


