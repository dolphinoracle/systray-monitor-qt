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
    // Write default.conf file
    write_default_conf_file();
    qApp->setQuitOnLastWindowClosed(false);
    this->setWindowIcon(QIcon::fromTheme("applications-system"));

    //process conf file
    QString conf_file = conffile;

    //defualt time of 5 seconds
    int time = 5;

    //if no conf_file specified, load default.conf
    if (conf_file.isEmpty()){
        conf_file = QDir::homePath() + "/.config/systray-monitor-qt/default.conf";
    }
    //display conf file being loaded in terminal output, either default.conf or user specified
    qDebug() << "config file " << conf_file;

    //read values for time and command if conf file exists
    if ( QFile(conf_file).exists()) {
        QSettings settings(conf_file, QSettings::NativeFormat);

        // read time and command variables whatever conf file is active

        time = settings.value("time").toInt();
        command = settings.value("command").toString();
    }

    //time is in seconds, but Qt's timer runs in 1/1000th of a second
    time = time * 1000;

    //set a default command to run if settings file is empty
    //set here because if command is invalid in conf_file then we want to display no output rather than using default command
    if (command.isEmpty()){
    command = "/usr/lib/systray-monitor-qt/xfce-hkmon NET CPU TEMP IO RAM";
    }

    //create UI
    ui->setupUi(this);

    //add actions for menu items to run
    createActions();

    //menu items to run actions
    createMenu();

    //make left click show action window
    connect(trayIcon, &QSystemTrayIcon::activated, this, &MainWindow::iconActivated);

    //display systray tray icon
    trayIcon->show();

    //set options for textreport box in action window
    ui->textBoxReport->setWordWrapMode(QTextOption::NoWrap);

    //run command twice before starting timer
    infocmd();
    infocmd();

    //start command on timer
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
    //show action window, bring to on top
    this->show();
    this->raise();

}
MainWindow::~MainWindow()
{
    delete ui;
}


void MainWindow::iconActivated(QSystemTrayIcon::ActivationReason reason)
{
    //define what to do with varius left click combos, all start action window
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
    //create action items for menu
    aboutAction = new QAction(QIcon::fromTheme("help-about"), tr("About"), this);
    helpAction = new QAction(QIcon::fromTheme("help-browser"), tr("Help"), this);
    quitAction = new QAction(QIcon::fromTheme("gtk-quit"), tr("Quit"), this);
    toggleAutostartAction = new QAction(QIcon::fromTheme("preferences-system"), tr("Enable Autostart?"), this);

    //define what the actions do and what triggers them
    connect(aboutAction, &QAction::triggered, this, &MainWindow::about);
    connect(helpAction, &QAction::triggered, this, &MainWindow::help);
    connect(quitAction, &QAction::triggered, qApp, &QGuiApplication::quit);
    connect(toggleAutostartAction, &QAction::triggered, this, &MainWindow::toggleAutostart);
}

void MainWindow::createMenu()
{
    //create menu and add action items as entries
    menu = new QMenu(this);
    menu->addAction(toggleAutostartAction);
    menu->addAction(helpAction);
    menu->addAction(aboutAction);
    menu->addAction(quitAction);

    //create systray icon, and give it a menu and default tooltip
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
    //add desktop file to $HOME/.config/autostart if yes
    QString local_file = QDir::homePath() + "/.config/autostart/systray-monitor-qt.desktop";
    if (QMessageBox::Yes == QMessageBox::question(nullptr, tr("Autostart Settings"), tr("Enable Autostart?")))
        QFile::copy("/usr/share/applications/systray-monitor-qt.desktop", local_file);
    else
        QFile::remove(local_file);


}

// implement change event that closes app when window loses focus, hides window when something else gains focus
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
    //hide action window but don't close app
    this->hide();
}

// process keystrokes
void MainWindow::keyPressEvent(QKeyEvent *event)
{
    //close action window on Esc key
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
    //run command based on given time
    QTimer *timer = new QTimer(this);
        connect(timer, SIGNAL(timeout()), this, SLOT(infocmd()));
        timer->start(time);
}

//infocommand builder
void MainWindow::infocmd()
{
    //run defined command, filter some tags out
    INFO = runCmd(command).str.trimmed().remove("<txt>").remove("</txt>").remove("<tool>").remove("</tool>");

    //display info in tooltip and action window text box
    trayIcon->setToolTip(INFO);
    ui->textBoxReport->setPlainText(INFO);
}


void MainWindow::on_ButtonCopy_clicked()
{

    //copy action window text contents to clipboard
    QClipboard *clipboard = QApplication::clipboard();
    clipboard->setText(ui->textBoxReport->toPlainText());

}

void MainWindow::write_default_conf_file()
{
    //create commented $HOME/.config/systray-monitor-qt/default.conf if not present
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
