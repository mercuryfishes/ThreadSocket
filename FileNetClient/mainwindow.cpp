#include "mainwindow.h"
#include "recvfile.h"
#include "ui_mainwindow.h"

#include <QMessageBox>
#include <QThread>
#include <QVector>
#include <QRandomGenerator>
#include <QDebug>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    qDebug() << "主线程的ID：" << QThread::currentThread();

    ui->ip->setText("127.0.0.1");
    ui->port->setText("8989");

    // 创建子线程
    QThread* subThread = new QThread;
    RecvFile* worker = new RecvFile;
    worker->moveToThread(subThread);

    connect(this, &MainWindow::startConnect, worker, &RecvFile::connectServer);
    connect(worker, &RecvFile::connectOK, this, [=](){
        QMessageBox::information(this, "提示", "已经成功连接到了服务器");
    });
    connect(worker, &RecvFile::message, this, [=](QByteArray msg){
        QVector<QColor> colors = {
            Qt::red, Qt::green, Qt::black, Qt::blue, Qt::darkBlue, Qt::cyan, Qt::magenta
        };
        int index = QRandomGenerator::global()->bounded(colors.size());
        ui->msg->setTextColor(colors.at(index));
        ui->msg->append(msg);
    });

    connect(worker, &RecvFile::gameOver, this, [=](){
        qDebug() << "销毁子线程已经工作的任务对象";
        subThread->quit();
        subThread->wait();
        subThread->deleteLater();

        worker->deleteLater();
    });

    subThread->start();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_connect_clicked()
{
    QString ip = ui->ip->text();
    unsigned short port = ui->port->text().toUShort();
    emit startConnect(ip, port);
}
