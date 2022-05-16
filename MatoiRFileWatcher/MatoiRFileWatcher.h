#pragma once

#include <QtWidgets/QMainWindow>
#include <QFileSystemWatcher>
#include <QSystemTrayIcon>
#include "ui_MatoiRFileWatcher.h"


class MatoiRFileWatcher : public QMainWindow
{
    Q_OBJECT

protected:
    enum : int {
        WatcherRole = Qt::UserRole + 1,
        FileRole
    };
public:
    MatoiRFileWatcher(QWidget *parent = Q_NULLPTR);


    void appendWatcher(bool isFile = true);
    void closeEvent(QCloseEvent* event)override;
	
	
private:
    QFileSystemWatcher watcher{ this };
    QSystemTrayIcon systemTrayIcon{ this };
	
    Ui::MatoiRFileWatcherClass ui;
};
