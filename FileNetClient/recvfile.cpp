#include "recvfile.h"

#include <QHostAddress>
#include <QDebug>
#include <QThread>
#include <QtEndian>

RecvFile::RecvFile(QObject *parent) : QObject(parent)
{

}

void RecvFile::connectServer(QString ip, unsigned short port)
{
    qDebug() << "子线程的ID：" << QThread::currentThread();

    // 进行连接服务器
    m_tcp = new QTcpSocket;
    m_tcp->connectToHost(QHostAddress(ip), port);
    connect(m_tcp, &QTcpSocket::connected, this, &RecvFile::connectOK);
    connect(m_tcp, &QTcpSocket::readyRead, this, [=](){
//        QByteArray all = m_tcp->readAll();
//        emit message(all);
        dealDate();
        m_tcp->close();
        m_tcp->deleteLater();
        emit gameOver();
    });
}

void RecvFile::dealDate()
{
    unsigned int totalBytes = 0;
    unsigned int recvBytes = 0;
    QByteArray block;

    // 判断有没有数据
    if (m_tcp->bytesAvailable() == 0) {
        qDebug() << "没有数据，拜拜...";
        return ;
    }

    // 读包头
    if (m_tcp->bytesAvailable() >= sizeof(int)) {
        QByteArray head = m_tcp->read(sizeof(int));
        totalBytes = qFromBigEndian(*(int*)head.data());
        qDebug() << "包头的长度：" << totalBytes;
    } else {
        return ;
    }

    // 读数据块
    while (totalBytes - recvBytes > 0 && m_tcp->bytesAvailable()) {
        block.append(m_tcp->read(totalBytes - recvBytes));
        recvBytes = block.size();
    }
    if (totalBytes == recvBytes) {
        emit message(block);
    }

    // 如果还有数据，继续读下一个数据包
    if (m_tcp->bytesAvailable() > 0) {
        dealDate();
    }
}
