#pragma once

#include <QWidget>
#include "ui_Terminal.h"
#include <QList>
#include <QProcess>

class Terminal : public QWidget
{
    Q_OBJECT

public:
    Terminal(QWidget *parent = Q_NULLPTR);
    ~Terminal();

    bool start();
    bool stop();
    bool restart();

private slots:
    void onProcessReadyRead();
    void onBtnEnetrClicked();
    void onProcessStateChanged(QProcess::ProcessState newState);
    void onBtnStartClicked();
    void onBtnStopClicked();
    void onBtnRestartClicked();
    void onBtnClsClicked();
    void onBtnSaveToFileClicked();
private:
    QString getCommand() const;
    void appendLineFeedsToCmd(QString& cmd);
    QString getCommandWithLineFeeds() const;
    void setCommand(const QString& text);
    void setConsoleText(const QString& text);
    void appendConsoleText(const QString& text);
    void clearConsoleText();  
    QString getConsoleText() const;
    void setStatus(const QString& text);
    bool eventFilter(QObject *target, QEvent *event) override;

    void appendCmdRecord(const QString& context);
    QString getCmdRecordAt(int index = 0);
private:
    enum {MaxCmdRecordSize = 20};
    Ui::Terminal ui;

    QProcess*   m_process;
    QList<QString> m_cmdRecord;
    int m_recordIndex;
    bool m_startFlag;
};
