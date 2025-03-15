#include "sendfile.h"

#include <QThread>
#include <QDebug>
#include <QFile>
#include <QtEndian>

SendFile::SendFile(qintptr socket, QObject *parent) : QObject(parent)
{
    m_socket = socket;
}

void SendFile::working(QString path)
{
    qDebug() << "当前子线程的ID：" << QThread::currentThread();
    m_tcp = new QTcpSocket;
    m_tcp->setSocketDescriptor(m_socket); // 设置好后这个对象就可以通信了

    connect(m_tcp, &QTcpSocket::disconnected, this, [=](){
        m_tcp->close();
        m_tcp->deleteLater();
        emit done();
        qDebug() << "客户端数据已经接收完毕，并断开了连接，开始销毁套接字对象，拜拜...";
    });

    qDebug() << "发送的文件名字：" << path;
    QFile file(path);
    if (file.open(QFile::ReadOnly)) {
        while (!file.atEnd()) {
            QByteArray line = file.readLine();

            // 添加包头
            int len = qToBigEndian(line.size());
            QByteArray data((char*)&len, 4);
            data.append(line);

            // 发送数据
            m_tcp->write(data);
            emit text(line);
            QThread::msleep(50);
        }
    }
    file.close();
}
