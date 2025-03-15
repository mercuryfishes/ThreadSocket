#ifndef RECVFILE_H
#define RECVFILE_H

#include <QObject>
#include <QTcpSocket>

class RecvFile : public QObject
{
    Q_OBJECT
public:
    explicit RecvFile(QObject *parent = nullptr);

    // 用于连接服务器的函数
    void connectServer(QString ip, unsigned short port);

    void dealDate();

signals:
    void connectOK();
    void message(QByteArray msg);
    void gameOver();

private:
    QTcpSocket* m_tcp;

public slots:
};

#endif // RECVFILE_H
