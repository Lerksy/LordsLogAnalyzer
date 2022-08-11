#include "dbman.h"

bool DBMan::openDB(){
    db = QSqlDatabase::addDatabase("QSQLITE");
    db.setDatabaseName(dbPath);
    return db.open();
}

void DBMan::createDB(){
    QSqlQuery initQuery;
    initQuery.exec("CREATE TABLE IF NOT EXISTS Accounts(IGGID BIGINT, TelegramID BIGINT, AccName varchar(255));");
    initQuery.exec("CREATE TABLE IF NOT EXISTS Settings(Property varchar(255), Value varchar(255));");
    initQuery.exec("CREATE UNIQUE INDEX IF NOT EXISTS idx_accounts_IGGID ON Accounts(IGGID);");
    initQuery.exec("CREATE UNIQUE INDEX IF NOT EXISTS idx_settings_setting ON Settings(Property);");
}

void DBMan::dbOpenError(){
    qDebug()<< "Database Open Error!";
    QCoreApplication::quit();
}

QString DBMan::escapeInputStringForSQL(QVariant string){
    QSqlField f(QLatin1String(""), string.metaType());
    f.setValue(string);
    return db.driver()->formatValue(f);
}

bool DBMan::checkExistingRecord(QString table, QString column, QString value){
    QSqlQuery checkerQuery;
    checkerQuery.prepare("SELECT COUNT(*) FROM :TableName WHERE :Column = :Value;");
    checkerQuery.bindValue(":TableName", table);
    checkerQuery.bindValue(":Column", column);
    checkerQuery.bindValue(":Value", value);
    checkerQuery.exec();
    checkerQuery.next();
    return checkerQuery.value(0).toBool();
}

QVariant DBMan::readSetting(QString propertyName){
    QSqlQuery getSettingQuery;
    QString escapedSetting = escapeInputStringForSQL(propertyName);
    getSettingQuery.exec(QString("SELECT * FROM Settings WHERE Property='%1';").arg(escapedSetting));
    if(getSettingQuery.size() < 1) return QVariant();
    getSettingQuery.next();
    return getSettingQuery.value(escapedSetting);
}

DBMan::DBMan(QObject *parent) : QObject{parent}{
    if(!QFile::exists(dbPath)){
        if(!openDB()) dbOpenError();
        createDB();
    }else{
        if(!openDB()) dbOpenError();
    }
}

bool DBMan::addIGGID(qint64 IGGID, qint64 telegramID, QString accountName){
    QSqlQuery insertQuery;
    insertQuery.prepare("INSERT INTO Accounts(IGGID, TelegramID, AccName) VALUES(:IGGID, :TgID, :AccName);");
    insertQuery.bindValue(":IGGID", IGGID);
    insertQuery.bindValue(":TgID", telegramID);
    insertQuery.bindValue(":AccName", accountName);
    return insertQuery.exec();
}

bool DBMan::updateName(qint64 IGGID, QString newName){
    QSqlQuery updateQuery;
    updateQuery.prepare("UPDATE Accounts SET AccName=:NewName WHERE IGGID=:IGGID;");
    updateQuery.bindValue(":NewName", newName);
    updateQuery.bindValue(":IGGID", IGGID);
    return updateQuery.exec();
}

QStringList DBMan::getMonitoringData(){
    QSqlQuery getDataQuery;
    getDataQuery.exec("SELECT IGGID FROM Accounts;");
    if(getDataQuery.size() < 1) return QStringList();
    QStringList IGGIDList;
    while(getDataQuery.next()){
        IGGIDList += getDataQuery.value("IGGID").toString();
    }
    return IGGIDList;
}

bool DBMan::setBaseMonitoringFolder(QString path){
    bool isRecordExists = checkExistingRecord("Settings", "Property", "BaseFolderPath");
    QSqlQuery setPathQuery;
    if(!isRecordExists){
        setPathQuery.prepare("INSERT INTO Settings(Property, Value) VALUES ('BaseFolderPath', :Path);");
    }else{
        setPathQuery.prepare("UPDATE Settings SET Value=:Path WHERE Property='BaseFolderPath';");
    }
    setPathQuery.bindValue(":Path", path);
    return setPathQuery.exec();
}

QString DBMan::getBaseMonitoringFolder(){
    bool isRecordExists = checkExistingRecord("Settings", "Property", "BaseFolderPath");
    if(isRecordExists){
        QSqlQuery getPathQuery;
        getPathQuery.exec("SELECT Value FROM Settings WHERE Property='BaseFolderPath';");
        getPathQuery.next();
        return getPathQuery.value("Value").toString();
    }
    return QString();
}
