#include "Global.h"
#include <QSettings>
#include <QCoreApplication>
#include <QFile>

namespace Global {

QSettings::Format g_dcmNetToolsRecordFileFormat = QSettings::IniFormat;
inline const QString g_dcmNetToolsRecordFileName = "DcmNetToolsRecord.ini";

inline const QString g_echoStoreSCUIniGroupName = "EchoStoreSCU";
inline const QString g_echoStoreSCUIniValueAETitle = "AETitle";
inline const QString g_echoStoreSCUIniValueIP = "IP";
inline const QString g_echoStoreSCUIniValuePort = "Port";
inline const QString g_echoStoreSCUIniLogLevel = "LogLevel";
inline const QString g_echoStoreSCUFileListArray = "FileList";
inline const QString g_echoStoreSCUFile = "StoreFile";
inline const QString g_echoStoreSCUDir = "StoreDir";

inline const QString g_echoStoreSCPIniGroupName = "EchoStoreSCP";
inline const QString g_echoStoreSCPIniValueAETitle = "AETitle";
inline const QString g_echoStoreSCPIniLogLevel = "LogLevel";
inline const QString g_echoStoreSCPIniOutDir = "OutputDir";
inline const QString g_echoStoreSCPIniValuePort = "Port";

inline const QByteArray g_logCfg =
    "# log4cplus config for DcmNetTools \n"
    "\n"
    "log4cplus.rootLogger = DEBUG, console \n"
    "\n"
    "log4cplus.appender.console = log4cplus::ConsoleAppender \n"
    "log4cplus.appender.console.Threshold = INFO \n"
    "log4cplus.appender.console.logToStderr = true \n"
    "log4cplus.appender.console.ImmediateFlush = true \n"
    "log4cplus.appender.console.layout = log4cplus::PatternLayout \n"
    "log4cplus.appender.console.layout.ConversionPattern = [%p] %D{%Y-%m-%d %H:%M:%S.%q} - %m%n \n"
    "";

Settings::Settings()
{
}


Settings::~Settings()
{
}

void Settings::saveEchoStoreSCUSettings(const EchoStoreSCUSettings& settings)
{
    QSettings setting(Global::configurePath() + "/" + g_dcmNetToolsRecordFileName,
                      g_dcmNetToolsRecordFileFormat);
    setting.beginGroup(g_echoStoreSCUIniGroupName);
    setting.setValue(g_echoStoreSCUIniValueAETitle, settings.aeTitle);
    setting.setValue(g_echoStoreSCUIniValueIP, settings.IP);
    setting.setValue(g_echoStoreSCUIniLogLevel, settings.logLevel);
    setting.setValue(g_echoStoreSCUIniValuePort, settings.port);
    setting.setValue(g_echoStoreSCUDir, settings.dir);

    setting.beginWriteArray(g_echoStoreSCUFileListArray);
    for (int i = 0; i < settings.fileList.size(); ++i) {
        setting.setArrayIndex(i);
        setting.setValue(g_echoStoreSCUFile, settings.fileList[i]);
    }
    setting.endArray();

    setting.endGroup();
}

EchoStoreSCUSettings Settings::getEchoStoreSCUSettings()
{
    EchoStoreSCUSettings settings;
    QSettings setting(Global::configurePath() + "/" + g_dcmNetToolsRecordFileName,
                      g_dcmNetToolsRecordFileFormat);
    setting.beginGroup(g_echoStoreSCUIniGroupName);
    settings.aeTitle = setting.value(g_echoStoreSCUIniValueAETitle, "EchoStoreSCU").toString();
    settings.IP = setting.value(g_echoStoreSCUIniValueIP, "127.0.0.1").toString();
    settings.port = setting.value(g_echoStoreSCUIniValuePort, "104").toString();
    settings.dir = setting.value(g_echoStoreSCUDir, "").toString();
    settings.logLevel = setting.value(g_echoStoreSCUIniLogLevel, "info").toString();

    int size = setting.beginReadArray(g_echoStoreSCUFileListArray);
    QString buf;
    for (int i = 0; i < size; ++i) {
        setting.setArrayIndex(i);
        buf = setting.value(g_echoStoreSCUFile).toString();
        settings.fileList.append(buf);
    }
    setting.endArray();

    setting.endGroup();
    return settings;
}

void Settings::saveEchoStoreSCPSettings(const EchoStoreSCPSettings& settings)
{
    QSettings setting(Global::configurePath() + "/" + g_dcmNetToolsRecordFileName,
                      g_dcmNetToolsRecordFileFormat);
    setting.beginGroup(g_echoStoreSCPIniGroupName);
    setting.setValue(g_echoStoreSCPIniValueAETitle, settings.aeTitle);
    setting.setValue(g_echoStoreSCPIniValuePort, settings.port);
    setting.setValue(g_echoStoreSCPIniLogLevel, settings.logLevel);
    setting.setValue(g_echoStoreSCPIniOutDir, settings.outputDir);
    setting.endGroup();
}

EchoStoreSCPSettings Settings::getEchoStoreSCPSettings()
{
    EchoStoreSCPSettings settings;
    QSettings setting(Global::configurePath() + "/" + g_dcmNetToolsRecordFileName,
                      g_dcmNetToolsRecordFileFormat);
    setting.beginGroup(g_echoStoreSCPIniGroupName);
    settings.aeTitle = setting.value(g_echoStoreSCPIniValueAETitle, "EchoStoreSCP").toString();
    settings.port = setting.value(g_echoStoreSCPIniValuePort, "104").toString();
    settings.logLevel = setting.value(g_echoStoreSCPIniLogLevel, "info").toString();
    settings.outputDir = setting.value(g_echoStoreSCPIniOutDir, ".").toString();
    setting.endGroup();
    return settings;
}
QString applicationDirPath()
{
    return QCoreApplication::applicationDirPath();
}

bool replaceLogLevel(const QString &logFile, const QString &logLevel)
{
    QStringList logLevelList;
    logLevelList << "TRACE";
    logLevelList << "DEBUG";
    logLevelList << "INFO";
    logLevelList << "WARN";
    logLevelList << "ERROR";
    logLevelList << "FATAL";

    QString ll = logLevel.trimmed();
    ll = ll.toUpper();
    if (!logLevelList.contains(ll)) {
        ll = logLevelList[0];
    }
    logLevelList.removeOne(ll);

    QFile file(logFile);
    if (!file.open(QIODevice::ReadWrite | QIODevice::Text)) {
        return false;
    }
    QByteArray data = file.readAll();
    if (data.size() < 50) {
        data = g_logCfg;
    }
    for (auto& s : logLevelList) {
        data.replace(s.toLocal8Bit(), ll.toLocal8Bit());
    }
    file.close();

    if (!file.open(QIODevice::WriteOnly | QIODevice::Truncate)) {
        return false;
    }
    return file.write(data) > 0;
}

QString configurePath()
{
    return QCoreApplication::applicationDirPath() + "/config";
}

} // namespace Global
