#pragma once

#include <QtWidgets/QWidget>
#include "ui_DcmNetTools.h"
#include "EchoStoreSCU.h"
#include "EchoStoreSCP.h"
#include "Terminal.h"

class DcmNetTools : public QWidget
{
    Q_OBJECT

public:
    DcmNetTools(QWidget *parent = Q_NULLPTR);

private:
    void initUI();

    void closeEvent(QCloseEvent* event) override;

private:
    Ui::DcmNetToolsClass ui;

    EchoStoreSCU*   m_echoStoreSCU;
    EchoStoreSCP*   m_echoStoreSCP;
    Terminal*       m_terminal;
};
