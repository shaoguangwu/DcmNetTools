#include "EchoStoreSCP.h"

#include <QFileDialog>
#include <QDateTime>
#include <QFile>
#include <QDir>
#include <QIODevice>
#include <QMessageBox>
#include <QTextStream>
#include <QProcess>
#include <QDebug>
#include <QDesktopServices>

#include "Settings.h"
#include <QValidator> 

EchoStoreSCP::EchoStoreSCP(QWidget *parent)
    : QWidget(parent)
{
    ui.setupUi(this);
    setSettings(Settings::getEchoStoreSCPSettings());
    setStatus("NotRunning");

    QRegExp regExpPort("^([0-9]|[1-9]\\d|[1-9]\\d{2}|[1-9]\\d{3}|[1-5]\\d{4}|6[0-4]\\d{3}|65[0-4]\\d{2}|655[0-2]\\d|6553[0-5])$");
    QValidator* validatorPort = new QRegExpValidator(regExpPort, this);
    ui.lineEditPort->setValidator(validatorPort);

    m_process = new QProcess(this);
    //connect(m_process, SIGNAL(readyReadStandardOutput()),
    //    this, SLOT(onProcessReadyRead()));
    //connect(m_process, SIGNAL(readyReadStandardError()),
    //    this, SLOT(onProcessReadyRead()));
    connect(m_process, SIGNAL(readyRead()),
        this, SLOT(onProcessReadyRead()));
    connect(m_process, SIGNAL(stateChanged(QProcess::ProcessState)),
        this, SLOT(onProcessStateChanged(QProcess::ProcessState)));

    connect(ui.btnClearOutput, SIGNAL(clicked()), this, SLOT(onBtnClearOutputClicked()));
    connect(ui.btnSaveOutpuToFile, SIGNAL(clicked()), this, SLOT(onBtnSaveOutpuToFileClicked()));
    connect(ui.btnStart, SIGNAL(clicked()), this, SLOT(onBtnStartClicked()));
    connect(ui.btnStop, SIGNAL(clicked()), this, SLOT(onBtnStopClicked()));
    connect(ui.btnRestart, SIGNAL(clicked()), this, SLOT(onBtnRestartClicked()));
    connect(ui.btnChooseOutDir, SIGNAL(clicked()), this, SLOT(onBtnChooseOutDirClicked()));
}

EchoStoreSCP::~EchoStoreSCP()
{
    stop();
}

QString EchoStoreSCP::getAETitle() const
{
    return ui.lineEditAETitle->text();
}

QString EchoStoreSCP::getPort() const
{
    return ui.lineEditPort->text();;
}

QString EchoStoreSCP::getLogLevel() const
{
    return ui.lineEditLogLevel->text();
}

QString EchoStoreSCP::getOutputDir() const
{
    return ui.lineEditOutDir->text();
}

void EchoStoreSCP::setOutputDir(const QString& text)
{
    ui.lineEditOutDir->setText(text);
}

void EchoStoreSCP::start()
{
    if (m_process->state() == QProcess::NotRunning) {
        QString program = "bin/storescp.exe";
        if (!QFile::exists(program)) {
            QMessageBox::warning(this, tr("Error"), tr("Can not find \"storescp\""));
            return;
        }

        QString outDir = getOutputDir();
        QString dataDir = "data";
        QDir dir;
        dir.mkdir(dataDir);
        dir.mkdir(outDir);
        QStringList args;
        args << "-aet";
        args << getAETitle();
        args << getPort();
        args << "-od";
        args << dataDir;
        args << "-ll";
        args << getLogLevel();
        args << "-od";
        args << outDir;
        m_process->start(program, args);
    }
}
void EchoStoreSCP::stop()
{
    if (m_process->state() != QProcess::NotRunning) {
        m_process->kill();
        m_process->waitForFinished();
    }
}
void EchoStoreSCP::restart()
{
    stop();
    start();
}

void EchoStoreSCP::appendOutputText(const QString& text)
{
    ui.plainTextEdit->appendPlainText(text);
}
void EchoStoreSCP::clearOutputText()
{
    ui.plainTextEdit->clear();
}
QString EchoStoreSCP::getOutputText()
{
    return ui.plainTextEdit->toPlainText();
}

void EchoStoreSCP::saveSettings()
{
    Settings::saveEchoStoreSCPSettings(getSettings());
}

void EchoStoreSCP::setSettings(const EchoStoreSCPSettings& set)
{
    ui.lineEditAETitle->setText(set.aeTitle);
    ui.lineEditPort->setText(set.port);
    ui.lineEditLogLevel->setText(set.logLevel);
    setOutputDir(set.outputDir);
}
EchoStoreSCPSettings EchoStoreSCP::getSettings() const
{
    EchoStoreSCPSettings set;
    set.aeTitle = ui.lineEditAETitle->text();
    set.port = ui.lineEditPort->text();
    set.logLevel = ui.lineEditLogLevel->text();
    set.outputDir = ui.lineEditOutDir->text();
    return set;
}

void EchoStoreSCP::setStatus(const QString& text)
{
    ui.labelStatus->setText(text);
}

void EchoStoreSCP::onBtnClearOutputClicked()
{
    clearOutputText();
}

void EchoStoreSCP::onBtnSaveOutpuToFileClicked()
{
    QString logDir = QDir::currentPath() + "/log/";
    QDir dir;
    dir.mkdir(logDir);
    QDateTime dt = QDateTime::currentDateTime();
    QString fileName = logDir + dt.toString("yyyy-MM-dd_hh.mm.ss.zzz") + QString(".output.txt");
    QFile file(fileName);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text))
    {
        QMessageBox::warning(this, tr("Error"), QString("create file failed : ") + fileName);
        return;
    }
    QTextStream stream(&file);
    stream.setCodec("UTF-8");
    stream.setGenerateByteOrderMark(false);
    stream << getOutputText();
    file.close();

    QMessageBox::StandardButton btn =
        QMessageBox::question(this, tr("Open It or not ? "), QString("Save file successfully. Open it or not?\n") + fileName);
    if (btn == QMessageBox::Yes) {
        QDesktopServices::openUrl(QUrl(fileName));
    }
}

void EchoStoreSCP::onBtnStartClicked()
{
    start();
}

void EchoStoreSCP::onBtnStopClicked()
{
    stop();
}

void EchoStoreSCP::onBtnRestartClicked()
{
    restart();
}

void EchoStoreSCP::onBtnChooseOutDirClicked()
{
    QString outDir = QFileDialog::getExistingDirectory(this, tr("Choose Output Directory"));
    if (!outDir.isEmpty()) {
        setOutputDir(outDir);
    }
}

void EchoStoreSCP::onProcessReadyRead()
{
    QString text = getCurrentTimeString4Log();
    text += QString::fromLocal8Bit(m_process->readAll());
    appendOutputText(text);
}
void EchoStoreSCP::onProcessStateChanged(QProcess::ProcessState state)
{
    QString staus;
    switch (state)
    {
    case QProcess::NotRunning:
        staus = "NotRunning"; 
        break;
    case QProcess::Starting:
        staus = "Starting";
        break;
    case QProcess::Running:
        staus = "Running";
        break;
    default:
        break;
    }
    setStatus(staus);
    QString text = getCurrentTimeString4Log();
    text += "The SCP is : ";
    text += staus;
    appendOutputText(text);
}

QString EchoStoreSCP::getCurrentTimeString4Log() const
{
    QDateTime dt = QDateTime::currentDateTime();
    return dt.toString("[ yyyy-MM-dd hh:mm:ss.zzz ] : \n");
}