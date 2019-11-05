#pragma once

#include <QString>

struct EchoStoreSCUSettings
{
    QString aeTitle;
    QString IP;
    QString port;
};

class Settings
{
public:
    Settings();
    ~Settings();

    static void saveEchoStoreSCUSettings(const EchoStoreSCUSettings& settings);
    static EchoStoreSCUSettings getEchoStoreSCUSettings();
};

