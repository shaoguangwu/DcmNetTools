#include "Settings.h"
#include <QSettings>

QSettings::Format g_dcmNetToolsRecordFileFormat = QSettings::IniFormat;
static QString g_dcmNetToolsRecordFileName = "DcmNetToolsRecord.ini";

static QString g_echoStoreSCUIniGroupName = "EchoStoreSCU";
static QString g_echoStoreSCUIniValueAETitle = "AETitle";
static QString g_echoStoreSCUIniValueIP = "IP";
static QString g_echoStoreSCUIniValuePort = "Port";

Settings::Settings()
{
}


Settings::~Settings()
{
}

void Settings::saveEchoStoreSCUSettings(const EchoStoreSCUSettings& settings)
{
    QSettings setting(g_dcmNetToolsRecordFileName, g_dcmNetToolsRecordFileFormat);
    setting.beginGroup(g_echoStoreSCUIniGroupName);
    setting.setValue(g_echoStoreSCUIniValueAETitle, settings.aeTitle);
    setting.setValue(g_echoStoreSCUIniValueIP, settings.IP);
    setting.setValue(g_echoStoreSCUIniValuePort, settings.port);
    setting.endGroup();
}

EchoStoreSCUSettings Settings::getEchoStoreSCUSettings()
{
    EchoStoreSCUSettings settings;
    QSettings setting(g_dcmNetToolsRecordFileName, g_dcmNetToolsRecordFileFormat);
    setting.beginGroup(g_echoStoreSCUIniGroupName);
    settings.aeTitle = setting.value(g_echoStoreSCUIniValueAETitle, "").toString();
    settings.IP = setting.value(g_echoStoreSCUIniValueIP, "127.0.0.1").toString();
    settings.port = setting.value(g_echoStoreSCUIniValuePort, "104").toString();
    setting.endGroup();
    return settings;
}