#pragma once

#include <QString>
#include <QStringList>

namespace Global {

struct EchoStoreSCUSettings
{
    QString aeTitle;
    QString IP;
    QString port;
    QStringList fileList;
    QString dir;
    QString logLevel;
};

struct EchoStoreSCPSettings
{
    QString aeTitle;
    QString port;
    QString outputDir;
    QString logLevel;
};

class Settings
{
public:
    Settings();
    ~Settings();

    static void saveEchoStoreSCUSettings(const EchoStoreSCUSettings& settings);
    static EchoStoreSCUSettings getEchoStoreSCUSettings();

    static void saveEchoStoreSCPSettings(const EchoStoreSCPSettings& settings);
    static EchoStoreSCPSettings getEchoStoreSCPSettings();
};

QString applicationDirPath();
QString configurePath();

bool replaceLogLevel(const QString& logFile, const QString& logLevel);

} // namespace Global