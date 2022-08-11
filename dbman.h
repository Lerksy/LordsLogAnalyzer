#ifndef DBMAN_H
#define DBMAN_H

#include <QObject>
#include <QFile>
#include <QtSql>

class DBMan : public QObject
{
    Q_OBJECT

private:
    bool openDB();
    void createDB();
    void dbOpenError();
    QSqlDatabase db;
    const QString dbPath = QCoreApplication::applicationDirPath() + "/database.db";
    QString escapeInputStringForSQL(QVariant string);
    bool checkExistingRecord(QString table, QString column, QString value);

public:
    explicit DBMan(QObject *parent = nullptr);
    bool addIGGID(qint64 IGGID, qint64 telegramID, QString accountName = QString());
    bool updateName(qint64 IGGID, QString newName);
    QStringList getMonitoringData();
    bool setBaseMonitoringFolder(QString path);
    QString getBaseMonitoringFolder();
    QVariant readSetting(QString settingName);

signals:

};

#endif // DBMAN_H
