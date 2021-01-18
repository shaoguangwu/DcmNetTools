#pragma once

#include <QWidget>
#include <QProcess>

namespace Ui { class EchoStoreSCU; };

class EchoStoreSCU : public QWidget
{
    Q_OBJECT

public:
    EchoStoreSCU(QWidget *parent = Q_NULLPTR);
    ~EchoStoreSCU();

    void saveSettings();
    void killProcess();

private:
    void initUI();
    void connectSignalSlots();

    QString getAETitle() const;
    void setAETitle(const QString& aeTitle);
    QString getIP() const;
    void setIP(const QString& ip);
    QString getPort() const;
    void setPort(const QString& port);
    QStringList getStoreFileNameList() const;
    void setStoreFileNameList(const QStringList& list);
    void clearStoreFileNameList();
    QString getStoreDir() const;
    void setStoreDir(const QString& dir);
    QString getLogLevel() const;
    void setLogLevel(const QString& text);

    QString getCurrentTimeString() const;
    QString getCurrentTimeString4Log() const;

    void clearTextEditConsole();
    void setTextEditConsoleText(const QString& text);
    void appendTextEditConsoleText(const QString& text);
    QString getTextEditConsoleText() const;
    void startProcess(const QString& program, const QStringList& args);
    void setStatus(const QString& text);

private slots:
    void onBtnChooseFilesClicked();
    void onBtnClearOutputClicked();
    void onBtnChooseDirClicked();
    void onBtnSaveOutputToFileClicked();
    void onBtnEchoClicked();
    void onBtnStoreFilesClicked();
    void onBtnStoreDirClicked();
    void onBtnStopClicked();
    void onProcessReadyRead();
    void onProcessStateChanged(QProcess::ProcessState);

private:
    Ui::EchoStoreSCU *ui;
    QProcess* m_process;
    QString m_program;

    QString m_echoscuLoggerCfgFile;
    QString m_storescuLoggerCfgFile;
};
