#include <QDebug>
#include <QDir>
#include <QKeyEvent>
#include <QMessageBox>
#include <QScreen>
#include <QTimer>
#include <QClipboard>
#include <QSettings>
#include <QDir>
#include <QFile>


#include "about.h"
#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(const QString &conffile, QDialog *parent) :
    QDialog(parent),
    ui(new Ui::MainWindow)
{
    QString conf_file = conffile;
    write_default_conf_file();
    int time = 0;
    qApp->setQuitOnLastWindowClosed(false);
    //process conf file
    if (conf_file.isEmpty()){
        conf_file = QDir::homePath() + "/.config/systray-monitor-qt/default.conf";
    }
    qDebug() << "config file " << conf_file;
    if ( QFile(conf_file).exists()) {
        QSettings settings(conf_file, QSettings::NativeFormat);

        time = settings.value("time").toInt();
        command = settings.value("command").toString();
    }

    time = time * 1000;
    //backup defaults
    if (time == 0){
    time = 5000;
    }
    if (command.isEmpty()){
    command = "/usr/lib/systray-monitor-qt/xfce-hkmon NET CPU TEMP IO RAM";
    }

    ui->setupUi(this);
    createActions();
    createMenu();
    connect(trayIcon, &QSystemTrayIcon::activated, this, &MainWindow::iconActivated);
    trayIcon->show();
    ui->textBoxReport->setWordWrapMode(QTextOption::NoWrap);
    //this->setWindowFlags(Qt::Tool | Qt::WindowStaysOnTopHint | Qt::FramelessWindowHint);
    infocmd();
    infocmd();
    runinfocmd(time);

}

// util function for getting bash command output and error code
Output MainWindow::runCmd(QString cmd)
{
    if (proc.state() != QProcess::NotRunning) {
        qDebug() << "Process already running:" << proc.program() << proc.arguments();
        return Output();
    }
    QEventLoop loop;
    connect(&proc, SIGNAL(finished(int)), &loop, SLOT(quit()));
    proc.setProcessChannelMode(QProcess::MergedChannels);
    proc.start("/bin/bash", QStringList() << "-c" << cmd);
    loop.exec();
    return {proc.exitCode(), proc.readAll().trimmed()};
}


void MainWindow::start()
{
    if (proc.state() != QProcess::NotRunning)
        return;
    this->show();
    this->raise();

}
MainWindow::~MainWindow()
{
    delete ui;
}


void MainWindow::iconActivated(QSystemTrayIcon::ActivationReason reason)
{
    //setPosition();
    switch (reason) {
    case QSystemTrayIcon::DoubleClick:
    case QSystemTrayIcon::MiddleClick:
    case QSystemTrayIcon::Trigger:
        start();
        break;
    default:
        break;
    }
}

void MainWindow::createActions()
{
    aboutAction = new QAction(QIcon::fromTheme("help-about"), tr("About"), this);
    helpAction = new QAction(QIcon::fromTheme("help-browser"), tr("Help"), this);
    quitAction = new QAction(QIcon::fromTheme("gtk-quit"), tr("Quit"), this);
    toggleAutostartAction = new QAction(QIcon::fromTheme("preferences-system"), tr("Enable Autostart?"), this);

    connect(aboutAction, &QAction::triggered, this, &MainWindow::about);
    connect(helpAction, &QAction::triggered, this, &MainWindow::help);
    connect(quitAction, &QAction::triggered, qApp, &QGuiApplication::quit);
    connect(toggleAutostartAction, &QAction::triggered, this, &MainWindow::toggleAutostart);
}

void MainWindow::createMenu()
{
    menu = new QMenu(this);
    menu->addAction(toggleAutostartAction);
    menu->addAction(helpAction);
    menu->addAction(aboutAction);
    menu->addAction(quitAction);

    trayIcon = new QSystemTrayIcon(this);
    trayIcon->setIcon(QIcon::fromTheme("applications-system-symbolic"));
    trayIcon->setContextMenu(menu);
    trayIcon->setToolTip(tr("Monitor"));
}

void MainWindow::help()
{
    QString url = "file:///usr/share/doc/systray-monitor-qt/systray-monitor-qt.html";
    QLocale locale;
    QString lang = locale.bcp47Name();
    displayDoc(url, tr("%1 Help").arg(tr("Systray Monitor")));
}

void MainWindow::setPosition()
{
    QPoint pos = QCursor::pos();
    QScreen *screen = QGuiApplication::screenAt(pos);
    if (pos.y() + this->size().height() > screen->availableVirtualGeometry().height())
        pos.setY(screen->availableVirtualGeometry().height() - this->size().height());
    if (pos.x() + this->size().width() > screen->availableVirtualGeometry().width())
        pos.setX(screen->availableVirtualGeometry().width() - this->size().width());
    this->move(pos);
}

void MainWindow::toggleAutostart()
{
    QString local_file = QDir::homePath() + "/.config/autostart/systray-monitor-qt.desktop";
    if (QMessageBox::Yes == QMessageBox::question(nullptr, tr("Autostart Settings"), tr("Enable Autostart?")))
        QFile::copy("/usr/share/applications/systray-monitor-qt.desktop", local_file);
    else
        QFile::remove(local_file);


}

//// implement change event that closes app when window loses focus
//void MainWindow::changeEvent(QEvent *event)
//{
//    QWidget::changeEvent(event);
//    if (event->type() == QEvent::ActivationChange) {
//        if (!this->isActiveWindow())
//            this->hide();
//    }
//}

void MainWindow::on_cancel_pressed()
{
    this->hide();
}

// process keystrokes
void MainWindow::keyPressEvent(QKeyEvent *event)
{
    if (event->key() == Qt::Key_Escape)
        this->hide();
}

// About systray-monitor-qt
void MainWindow::about()
{
    QString version = runCmd("dpkg-query --show systray-monitor-qt").str.simplified().section(' ', 1, 1);
    displayAboutMsgBox(tr("About") + " " + tr("Systray Monitor Qt"),
                       "<p align=\"center\"><b><h2>" + tr("Systray Monitor Qt") + "</h2></b></p><p align=\"center\">" +
                       tr("Version: ") + version + "</p><p align=\"center\"><h3>" +
                       tr("Monitor command output from systray icon") +
                       "</h3></p><p align=\"center\"><a href=\"http://mxlinux.org\">http://mxlinux.org</a><br /></p>"
                       "<p align=\"center\">" + tr("Copyright (c) MX Linux") + "<br /><br /></p>",
                       QStringLiteral("/usr/share/doc/systray-monitor-qt/license.html"), tr("%1 License").arg(this->windowTitle()));
}

// timer to run command
void MainWindow::runinfocmd(int time)
{
    QTimer *timer = new QTimer(this);
        connect(timer, SIGNAL(timeout()), this, SLOT(infocmd()));
        timer->start(time);
}

//infocommand builder
void MainWindow::infocmd()
{
    INFO = runCmd(command).str.trimmed().remove("<txt>").remove("</txt>").remove("<tool>").remove("</tool>");
    trayIcon->setToolTip(INFO);
    ui->textBoxReport->setPlainText(INFO);
}


void MainWindow::on_ButtonCopy_clicked()
{

    QClipboard *clipboard = QApplication::clipboard();
    clipboard->setText(ui->textBoxReport->toPlainText());

}

void MainWindow::write_default_conf_file()
{
    if (! QDir(QDir::homePath() + "/.config/systray-monitor-qt").exists() ){
        QDir().mkdir(QDir::homePath() + "/.config/systray-monitor-qt");
    }

    QFile file(QDir::homePath() + "/.config/systray-monitor-qt/default.conf");
    if (! file.exists()){
        if (file.open(QFile::Truncate|QFile::WriteOnly)) {
            const QByteArray &text = "#Time in seconds\ntime=5\n#command should be in quotation marks\ncommand=\"/usr/lib/systray-monitor-qt/xfce-hkmon NET CPU TEMP IO RAM\"";
            file.write(text);
            file.close();
        }
    }


}
