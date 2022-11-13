#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QDialog>
#include <QListWidgetItem>
#include <QMenu>
#include <QProcess>
#include <QSystemTrayIcon>

namespace Ui {
class MainWindow;
}

// struct for outputing both the exit code and the strings when running a command
struct Output {
    int exit_code;
    QString str;
};

class MainWindow : public QDialog
{
    Q_OBJECT

public:
    MainWindow(const QString &conffile, QDialog *parent = nullptr);
    ~MainWindow();
    void start();
    void about();
    Output runCmd(QString cmd);
    QString UID;
    QString INFO;
    QString command;

private slots:
    //void changeEvent(QEvent *event);
    void keyPressEvent(QKeyEvent *event);
    void on_cancel_pressed();
    void iconActivated(QSystemTrayIcon::ActivationReason reason);
    void infocmd();


    void on_ButtonCopy_clicked();

private:
    Ui::MainWindow *ui;
    QAction *aboutAction;
    QAction *helpAction;
    QAction *listDevicesAction;
    QAction *quitAction;
    QAction *toggleAutostartAction;
    QMenu *menu;
    QProcess proc;
    QSystemTrayIcon *trayIcon;

    void createActions();
    void createMenu();
    void help();
    void setPosition();
    void toggleAutostart();
    void runinfocmd(int time);
    void write_default_conf_file();

};

#endif // MAINWINDOW_H
