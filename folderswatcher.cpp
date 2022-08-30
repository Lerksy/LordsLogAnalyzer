#include "folderswatcher.h"

foldersWatcher::foldersWatcher(DBMan *database, QObject *parent) : QObject{parent}{
    if(database == nullptr){
        qDebug()<<"No database was provided, exiting...";
        QCoreApplication::quit();
    }else{
        this->db = database;
    }
    QString baseFolder = db->getBaseMonitoringFolder();

    if(baseFolder.isEmpty()){
        qDebug()<<"Base folder was not set!";
        return;
    }
    setWatcherFolders();
    QObject::connect(db, &DBMan::IGGIDAdded, this, &foldersWatcher::setWatcherFolders);
}

void foldersWatcher::setWatcherFolders(){
    QStringList watcherDirs = watcher.directories();

    if(!watcherDirs.isEmpty()){
        watcher.removePaths(watcherDirs);
    }

    watcherDirs.clear();
    QStringList IGGIDList = db->getMonitoringData();

    if(IGGIDList.isEmpty()) return;

    QString templatePathStr = db->getBaseMonitoringFolder() + "/%1/logs";

    for(auto i : IGGIDList){
        watcherDirs += templatePathStr.arg(i);
    }

    watcher.addPaths(watcherDirs);
}

void foldersWatcher::folderChanged(QString folder){
    if(folder.isEmpty()) return;

    QDir logsDir(folder);
    QStringList files = logsDir.entryList(QDir::Files | QDir::NoDotAndDotDot, QDir::Time);

    if(files.isEmpty()) return;

    emit logNeedProcession(files.at(0));
}


