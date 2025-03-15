#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QMessageBox>
#include <QVector>
#include <QRandomGenerator>
#include <QFileDialog>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    ui->port->setText("8989");

    qDebug() << "主线程的线程ID：" << QThread::currentThread();

    m_server = new MyTcpServer(this);

    connect(m_server, &MyTcpServer::newClient, this, [=](qintptr socket){
        // 处理子线程相关动作
        QThread *subThread = new QThread;
        SendFile* worker = new SendFile(socket); // 不能指定父对象，因为要移动到子线程中
        worker->moveToThread(subThread);
        connect(this, &MainWindow::start, worker, &SendFile::working);

        connect(worker, &SendFile::done, this, [=](){
            qDebug() << "销毁子线程和任务对象资源...";
            subThread->quit();
            subThread->wait();
            subThread->deleteLater();

            worker->deleteLater();
        });

        connect(worker, &SendFile::text, this, [=](QByteArray msg){
            QVector<QColor> colors = {
                Qt::red, Qt::green, Qt::black, Qt::blue, Qt::darkBlue, Qt::cyan, Qt::magenta
            };
            int index = QRandomGenerator::global()->bounded(colors.size());
            ui->msg->setTextColor(colors.at(index));
            ui->msg->append(msg);
        });

        subThread->start();
    });
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_start_clicked()
{
    unsigned short port = ui->port->text().toUShort();
    m_server->listen(QHostAddress::Any, port);
}

void MainWindow::on_selectFile_clicked()
{
    QString path = QFileDialog::getOpenFileName(this);
    if (!path.isEmpty()) {
        ui->path->setText(path);
    }
}

void MainWindow::on_sendFile_clicked()
{
    // 得到文件路径
    if(ui->path->text().isEmpty()) {
        QMessageBox::information(this, "提示", "要发送的文件不能为空");
        return ;
    }
    emit start(ui->path->text());
}
