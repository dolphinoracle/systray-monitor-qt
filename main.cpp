#include "mainwindow.h"
#include <QApplication>
#include <QTranslator>
#include <QLocale>
#include <QLockFile>
#include <QCommandLineParser>

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    QString user = QProcessEnvironment::systemEnvironment().value("USER");
//    QLockFile lockfile("/var/lock/systray-monitor-qt_" + user + ".lock");
//    if (!lockfile.tryLock())
//        return EXIT_FAILURE;

    QCommandLineParser parser;
        parser.setApplicationDescription(QObject::tr("monitor command output from systray icon"));
        parser.addHelpOption();
        parser.addVersionOption();
        // An option with a value
           QCommandLineOption conffileoption(QStringList() << "c" << "config",
                   QCoreApplication::translate("main", "Full path to config file."),
                   QCoreApplication::translate("main", "path"));
           parser.addOption(conffileoption);
        parser.process(app);

    QTranslator qtTran;
    qtTran.load(QString("qt_") + QLocale::system().name());
    app.installTranslator(&qtTran);

    QTranslator appTran;
    appTran.load(QString("systray-monitor_") + QLocale::system().name(), "/usr/share/systray-monitor/locale");
    app.installTranslator(&appTran);

    QString conffile = parser.value(conffileoption);

    MainWindow w(conffile);
    w.hide();
    return app.exec();
}


