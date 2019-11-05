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

private:
    void initUI();
    void connectSignalSlots();

    QString getAETitle() const;
    void setAETitle(const QString& aeTitle);
    QString getIP() const;
    void setIP(const QString& ip);
    QString getPort() const;
    void setPort(const QString& port);

    QString getCurrentTimeString() const;
    QString getCurrentTimeString4Log() const;
    QStringList getStoreFileNameList() const;
    QString getStoreDir() const;

    void clearTextEditConsole();
    void setTextEditConsoleText(const QString& text);
    void appendTextEditConsoleText(const QString& text);
    QString getTextEditConsoleText() const;
    void startProcess(const QString& program, const QStringList& args);

private slots:
    void onBtnChooseFilesClicked();
    void onBtnClearOutputClicked();
    void onBtnChooseDirClicked();
    void onBtnSaveOutputToFileClicked();
    void onBtnEchoClicked();
    void onBtnStoreFilesClicked();
    void onBtnStoreDirClicked();
    void onProcessReadyReadStandardOutput();

private:
    Ui::EchoStoreSCU *ui;
    QProcess* m_process;

};
