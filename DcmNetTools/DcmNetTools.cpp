#include "DcmNetTools.h"

DcmNetTools::DcmNetTools(QWidget *parent)
    : QWidget(parent)
{
    ui.setupUi(this);
    initUI();
}

void DcmNetTools::initUI()
{
    QFont font = this->font();
    font.setFamily("Courier");
    this->setFont(font);

    this->setWindowIcon(QIcon(":/DcmNetTools/image/logo.png"));
    ui.tabWidget->clear();

    m_echoStoreSCU = new EchoStoreSCU(this);
    ui.tabWidget->addTab(m_echoStoreSCU, tr("EchoStoreSCU"));
    ui.tabWidget->setTabIcon(0, QIcon(":/DcmNetTools/image/user.png"));

    m_echoStoreSCP = new EchoStoreSCP(this);
    ui.tabWidget->addTab(m_echoStoreSCP, tr("EchoStoreSCP"));
    ui.tabWidget->setTabIcon(1, QIcon(":/DcmNetTools/image/server.png"));

#if defined(Q_OS_WIN)
    m_terminal = new Terminal(this);
    ui.tabWidget->addTab(m_terminal, tr("Terminal"));
    ui.tabWidget->setTabIcon(2, QIcon(":/DcmNetTools/image/terminal.png"));
#endif
}

void DcmNetTools::closeEvent(QCloseEvent* event)
{
    m_echoStoreSCU->saveSettings();
    m_echoStoreSCP->saveSettings();
    return QWidget::closeEvent(event);
}
