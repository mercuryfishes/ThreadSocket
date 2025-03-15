#ifndef SENDFILE_H
#define SENDFILE_H

#include <QObject>
#include <QTcpSocket>

class SendFile : public QObject
{
    Q_OBJECT
public:
    explicit SendFile(qintptr socket, QObject *parent = nullptr);

    void working(QString path);
signals:
    void done();
    void text(QByteArray msg);

private:
    qintptr m_socket;
    QTcpSocket* m_tcp;

public slots:
};

#endif // SENDFILE_H
