#include "DcmNetTools.h"

DcmNetTools::DcmNetTools(QWidget *parent)
    : QWidget(parent)
{
    ui.setupUi(this);
    initUI();
}

void DcmNetTools::initUI()
{
    this->setWindowIcon(QIcon(":/DcmNetTools/image/logo.png"));
    ui.tabWidget->clear();

    m_echoStoreSCU = new EchoStoreSCU(this);
    ui.tabWidget->addTab(m_echoStoreSCU, tr("EchoStoreSCU"));

    m_echoStoreSCP = new EchoStoreSCP(this);
    ui.tabWidget->addTab(m_echoStoreSCP, tr("EchoStoreSCP"));
}

void DcmNetTools::closeEvent(QCloseEvent* event)
{
    m_echoStoreSCU->saveSettings();
    return QWidget::closeEvent(event);
}
