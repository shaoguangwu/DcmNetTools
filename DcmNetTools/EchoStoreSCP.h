#pragma once

#include <QWidget>
#include "ui_EchoStoreSCP.h"
#include "Global.h"
#include <QProcess>

class EchoStoreSCP : public QWidget
{
    Q_OBJECT

public:
    EchoStoreSCP(QWidget *parent = Q_NULLPTR);
    ~EchoStoreSCP();

    void saveSettings();

private slots:
    void onBtnClearOutputClicked();
    void onBtnSaveOutpuToFileClicked();
    void onBtnStartClicked();
    void onBtnStopClicked();
    void onBtnRestartClicked();
    void onBtnChooseOutDirClicked();

    void onProcessReadyRead();
    void onProcessStateChanged(QProcess::ProcessState);

private:
    void setSettings(const Global::EchoStoreSCPSettings& set);
    Global::EchoStoreSCPSettings getSettings() const;
    void setStatus(const QString& text);
    QString getCurrentTimeString4Log() const;

    QString getAETitle() const;
    QString getPort() const;
    QString getLogLevel() const;
    QString getOutputDir() const;

    void setOutputDir(const QString& text);

    void start();
    void stop();
    void restart();

    void appendOutputText(const QString& text);
    void clearOutputText();
    QString getOutputText();

private:
    Ui::EchoStoreSCP ui;
    QProcess*   m_process;
    QString m_program;
    QString m_echoscpLogCfgFile;
};
