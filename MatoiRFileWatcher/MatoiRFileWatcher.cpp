#pragma execution_character_set("utf-8")
#include "MatoiRFileWatcher.h"
#include <QMenu>
#include <QFileDialog>
#include <QMessageBox>
#include <QFileIconProvider>
#include <QProcess>
#include <QCloseEvent>


MatoiRFileWatcher::MatoiRFileWatcher(QWidget *parent)
    : QMainWindow(parent)
{
    ui.setupUi(this);

	systemTrayIcon.setIcon(windowIcon());
	connect(&systemTrayIcon, &QSystemTrayIcon::activated, [=](QSystemTrayIcon::ActivationReason reason)
		{
			switch (reason)
			{
			case QSystemTrayIcon::DoubleClick:
				if (isHidden())
					show();
				else
					hide();
				break;
			}
		});

	auto menu = new QMenu{ this };
	menu->addAction("退出", qApp, &QApplication::quit);
	systemTrayIcon.setContextMenu(menu);
	

	connect(ui.watcherList, &QListWidget::itemClicked, [=](QListWidgetItem* item)
		{
			ui.watcherPath->setText(item->data(WatcherRole).toString());
		});
	connect(ui.browsePath, &QPushButton::clicked, [=]
		{
			QProcess::execute(QString{ R"(explorer /select,%1)" }.arg(QDir::toNativeSeparators(ui.watcherPath->text())));
		});
	
	connect(ui.watcherList, &QListWidget::customContextMenuRequested, [=](const QPoint&pos)
		{
			QMenu menu(this);
			auto item = ui.watcherList->itemAt(pos);
			if (item == nullptr)
			{
				menu.addAction("Add File Watcher", [=] {appendWatcher(true); });
				menu.addAction("Add Directory Watcher", [=] {appendWatcher(false); });
			}
			else
			{
				menu.addAction("Remove Watcher", [=]
					{
						ui.watcherList->removeItemWidget(item);
						watcher.removePath(item->data(WatcherRole).toString());
						delete(item);
					});
			}
			menu.exec(ui.watcherList->mapToGlobal(pos));
		});


	connect(&watcher, &QFileSystemWatcher::directoryChanged, [=](const QString& path)
		{
			systemTrayIcon.showMessage("Tip", QString{ "文件夹变动 %1" }.arg(path));
		});
	connect(&watcher, &QFileSystemWatcher::fileChanged, [=](const QString& path)
		{
			systemTrayIcon.showMessage("Tip", QString{ "文件变动 %1" }.arg(path));
		});

	systemTrayIcon.show();
}

void MatoiRFileWatcher::appendWatcher(bool isFile)
{
	const QString path = isFile ? QFileDialog::getOpenFileName(this, "Watch Path") : QFileDialog::getExistingDirectory(this, "Watch Path");
	if (path.isEmpty())
		return;
	const QFileInfo info{ path };
	const QFileIconProvider fileIconProvider;
	auto item = new QListWidgetItem(ui.watcherList);
	item->setToolTip(path);
	item->setIcon(fileIconProvider.icon(info));
	item->setText(info.fileName());
	item->setData(WatcherRole, path);
	watcher.addPath(path);
	ui.watcherList->addItem(item);
}

void MatoiRFileWatcher::closeEvent(QCloseEvent* event)
{
	hide();
	event->ignore();
}
