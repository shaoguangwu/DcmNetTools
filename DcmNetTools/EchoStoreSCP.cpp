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

#include "Global.h"
#include <QValidator> 

EchoStoreSCP::EchoStoreSCP(QWidget *parent)
    : QWidget(parent)
{
    ui.setupUi(this);
    setSettings(Global::Settings::getEchoStoreSCPSettings());
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

    QDir dir;
    dir.mkdir(Global::configurePath());

#if defined(Q_OS_LINUX)
    m_program = "storescp";
#elif defined(Q_OS_WIN)
    m_program = QCoreApplication::applicationDirPath() + "/bin/win32/storescp.exe";
#else
#endif
    m_echoscpLogCfgFile = Global::configurePath() + "/dcmnet-storescp-logger.cfg";
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
    QString logLevel = ui.lineEditLogLevel->text();
    return logLevel.isEmpty() ? "trace" : logLevel;
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

        QString outDir = getOutputDir();
        QDir dir(outDir);
        dir.mkpath(outDir);

        if (!dir.exists()) {
            QMessageBox::warning(this, tr("Error"), tr("Output directory is not exists."));
            return;
        }

        QStringList args;
        args << "-aet";
        args << getAETitle();
        args << "-od";
        args << outDir;
        //args << "+B";
        //args << "+F";
        args << getPort();

#ifdef Q_OS_LINUX
        args << "-lc";
        args << m_echoscpLogCfgFile;
        Global::replaceLogLevel(m_echoscpLogCfgFile, getLogLevel());
#elif Q_OS_WIN
        args << "-ll";
        args << getLogLevel();
#else
#endif

        QString parameter("## parameter: ");
        parameter += m_program;
        for (auto& arg : args) {
            parameter += ' ';
            parameter += arg;
        }
        appendOutputText(parameter);
        appendOutputText("");

        m_process->start(m_program, args);
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
    Global::Settings::saveEchoStoreSCPSettings(getSettings());
}

void EchoStoreSCP::setSettings(const Global::EchoStoreSCPSettings& set)
{
    ui.lineEditAETitle->setText(set.aeTitle);
    ui.lineEditPort->setText(set.port);
    ui.lineEditLogLevel->setText(set.logLevel);
    setOutputDir(set.outputDir);
}

Global::EchoStoreSCPSettings EchoStoreSCP::getSettings() const
{
    Global::EchoStoreSCPSettings set;
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
    QString fileName = logDir + dt.toString("yyyy-MM-dd_hh.mm.ss.zzz") + QString(".echostorescp.output.txt");
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