#ifndef FOLDERSWATCHER_H
#define FOLDERSWATCHER_H

#include <QObject>
#include <QFileSystemWatcher>
#include <QDebug>
#include "dbman.h"

class foldersWatcher : public QObject
{
    Q_OBJECT

private:
    DBMan *db;
    QFileSystemWatcher watcher;


private slots:
    void folderChanged(QString folder);
    void setWatcherFolders();

public:
    explicit foldersWatcher(DBMan *databse, QObject *parent = nullptr);


signals:
    void logNeedProcession(QString);

};

#endif // FOLDERSWATCHER_H
