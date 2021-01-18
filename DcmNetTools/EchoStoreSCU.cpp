#include "EchoStoreSCU.h"
#include "ui_EchoStoreSCU.h"

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

EchoStoreSCU::EchoStoreSCU(QWidget *parent)
    : QWidget(parent)
    , m_process(new QProcess)
{
    ui = new Ui::EchoStoreSCU();
    ui->setupUi(this);
    setStatus("NotRunning");

    initUI();
    connectSignalSlots();
}

EchoStoreSCU::~EchoStoreSCU()
{
    killProcess();
    delete ui;
}

void EchoStoreSCU::initUI()
{
    QRegExp regExpPort("^([0-9]|[1-9]\\d|[1-9]\\d{2}|[1-9]\\d{3}|[1-5]\\d{4}|6[0-4]\\d{3}|65[0-4]\\d{2}|655[0-2]\\d|6553[0-5])$");
    QValidator* validatorPort = new QRegExpValidator(regExpPort, this);
    ui->lineEditPort->setValidator(validatorPort);
    QRegExp regExpIp("^((25[0-5])|(2[0-4]\\d)|(1\\d\\d)|([1-9]\\d)|\\d)(\\.((25[0-5])|(2[0-4]\\d)|(1\\d\\d)|([1-9]\\d)|\\d)){3}$");
    QValidator* validatorIp = new QRegExpValidator(regExpIp, this);
    ui->lineEditIP->setValidator(validatorIp);

    ui->textEditConsole->setReadOnly(true);

    auto set = Settings::getEchoStoreSCUSettings();
    setAETitle(set.aeTitle);
    setIP(set.IP);
    setPort(set.port);
    setStoreDir(set.dir);
    setStoreFileNameList(set.fileList);
    setLogLevel(set.logLevel);
}

void EchoStoreSCU::connectSignalSlots()
{
    connect(ui->btnChooseFiles, SIGNAL(clicked()), 
        this, SLOT(onBtnChooseFilesClicked()));
    connect(ui->btnChooseDir, SIGNAL(clicked()),
        this, SLOT(onBtnChooseDirClicked()));
    connect(ui->btnClearOutput, SIGNAL(clicked()),
        this, SLOT(onBtnClearOutputClicked()));
    connect(ui->btnSaveOuputToFile, SIGNAL(clicked()),
        this, SLOT(onBtnSaveOutputToFileClicked()));
    connect(ui->btnEcho, SIGNAL(clicked()),
        this, SLOT(onBtnEchoClicked()));
    connect(ui->btnStoreFiles, SIGNAL(clicked()),
        this, SLOT(onBtnStoreFilesClicked()));
    connect(ui->btnStoreDir, SIGNAL(clicked()),
        this, SLOT(onBtnStoreDirClicked()));
    connect(ui->btnStop, SIGNAL(clicked()),
        this, SLOT(onBtnStopClicked()));
    //connect(m_process, SIGNAL(readyReadStandardOutput()), 
    //    this, SLOT(onProcessReadyReadStandardOutput()));
    connect(m_process, SIGNAL(readyRead()),
        this, SLOT(onProcessReadyRead()));
    connect(m_process, SIGNAL(stateChanged(QProcess::ProcessState)),
        this, SLOT(onProcessStateChanged(QProcess::ProcessState)));
}

QString EchoStoreSCU::getAETitle() const
{
    return ui->lineEditAeTitle->text();
}

void EchoStoreSCU::setAETitle(const QString& aeTitle)
{
    ui->lineEditAeTitle->setText(aeTitle);
}

QString EchoStoreSCU::getIP() const
{
    return ui->lineEditIP->text();
}

void EchoStoreSCU::setIP(const QString& ip)
{
    ui->lineEditIP->setText(ip);
}

QString EchoStoreSCU::getPort() const
{
    return ui->lineEditPort->text();
}

void EchoStoreSCU::setPort(const QString& port)
{
    ui->lineEditPort->setText(port);
}

QString EchoStoreSCU::getCurrentTimeString() const
{
    QDateTime dt = QDateTime::currentDateTime();
    return dt.toString("yyyy-MM-dd_hh.mm.ss.zzz");
}

QString EchoStoreSCU::getCurrentTimeString4Log() const
{
    QDateTime dt = QDateTime::currentDateTime();
    return dt.toString("[ yyyy-MM-dd hh:mm:ss.zzz ] : \n");
}

QStringList EchoStoreSCU::getStoreFileNameList() const
{
    QString text = ui->textEditFiles->toPlainText();
    QStringList list = text.split("\n");
    return list;
}

void EchoStoreSCU::setStoreFileNameList(const QStringList& list)
{
    QString text;
    for (int i = 0; i < list.count(); ++i) {
        text.append(QDir::toNativeSeparators(list[i]));
        if (i != list.count() - 1) {
            text.append("\n");
        }
    }
    ui->textEditFiles->setText(text);
}
void EchoStoreSCU::clearStoreFileNameList()
{
    ui->textEditFiles->clear();
}

QString EchoStoreSCU::getStoreDir() const
{
    return ui->lineEditDir->text();
}
void EchoStoreSCU::setStoreDir(const QString& dir)
{
    ui->lineEditDir->setText(dir);
}

QString EchoStoreSCU::getLogLevel() const
{
    return ui->lineEditLogLevel->text();
}

void EchoStoreSCU::setLogLevel(const QString& text)
{
    ui->lineEditLogLevel->setText(text);
}

void EchoStoreSCU::clearTextEditConsole()
{
    ui->textEditConsole->clear();
}
void EchoStoreSCU::setTextEditConsoleText(const QString& text)
{
    ui->textEditConsole->setText(text);
}

void EchoStoreSCU::appendTextEditConsoleText(const QString& text)
{
    ui->textEditConsole->append(text);
}

QString EchoStoreSCU::getTextEditConsoleText() const
{
    return ui->textEditConsole->toPlainText();
}

void EchoStoreSCU::startProcess(const QString& program, const QStringList& args)
{
    m_process->setProgram(program);
    m_process->setArguments(args);
    m_process->start();
}

void EchoStoreSCU::setStatus(const QString& text)
{
    ui->labelState->setText(text);
}

void EchoStoreSCU::saveSettings()
{
    EchoStoreSCUSettings set;
    set.aeTitle = getAETitle();
    set.IP = getIP();
    set.port = getPort();
    set.dir = getStoreDir();
    set.fileList = getStoreFileNameList();
    set.logLevel = getLogLevel();
    Settings::saveEchoStoreSCUSettings(set);
}

void EchoStoreSCU::killProcess()
{
    m_process->kill();
    m_process->waitForFinished();
}

void EchoStoreSCU::onBtnChooseFilesClicked()
{
    auto fileList = QFileDialog::getOpenFileNames(this, tr("Choose DICOM Files"));
    if (fileList.isEmpty()) {
        return;
    }
    clearStoreFileNameList();
    setStoreFileNameList(fileList);
}

void EchoStoreSCU::onBtnClearOutputClicked()
{
    clearTextEditConsole();
}

void EchoStoreSCU::onBtnChooseDirClicked()
{
    auto text = QFileDialog::getExistingDirectory(this, tr("Choose DICOM Directory"));
    if (text.isEmpty()) {
        return;
    }
    ui->lineEditDir->clear();
    ui->lineEditDir->setText(QDir::toNativeSeparators(text));
}

void EchoStoreSCU::onBtnSaveOutputToFileClicked()
{
    QString logDir = QCoreApplication::applicationDirPath() + "/log/";
    QDir dir;
    dir.mkdir(logDir);
    QDateTime dt = QDateTime::currentDateTime();
    QString fileName = logDir + getCurrentTimeString() + QString(".echostorescu.output.txt");
    QString text = getTextEditConsoleText();
    QFile file(fileName);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text))
    {
        QMessageBox::warning(this, tr("Error"), QString("create file failed : ") + fileName);
        return;
    }
    QTextStream stream(&file);
    stream.setCodec("UTF-8");
    stream.setGenerateByteOrderMark(false);
    stream << text;
    file.close();

    QMessageBox::StandardButton btn = 
        QMessageBox::question(this, tr("Open It or not ? "), QString("Save file successfully. Open it or not?\n") + fileName);
    if (btn == QMessageBox::Yes) {
        QDesktopServices::openUrl(QUrl(fileName));
    }
}

void EchoStoreSCU::onBtnEchoClicked()
{
    killProcess();
#if defined(Q_OS_LINUX)
    QString program = QCoreApplication::applicationDirPath() + "/bin/linux/echoscu";
#elif defined(Q_OS_WIN)
    QString program = QCoreApplication::applicationDirPath() + "/bin/win32/echoscu.exe";
#else
    return;
#endif
    if (!QFile::exists(program)) {
        QMessageBox::warning(this, tr("Error"), tr("Can not find \"echoscu\""));
        return;
    }
    QStringList args;
    args << getIP();
    args << getPort();
    startProcess(program, args);
}
void EchoStoreSCU::onBtnStoreFilesClicked()
{
    killProcess();
    QString program = "bin/storescu.exe";
    if (!QFile::exists(program)) {
        QMessageBox::warning(this, tr("Error"), tr("Can not find \"storescu\""));
        return;
    }
    QStringList args;
    args << getIP();
    args << getPort();
    args << getStoreFileNameList();
    startProcess(program, args);
}
void EchoStoreSCU::onBtnStoreDirClicked()
{
    killProcess();
    QString program = "bin/storescu.exe";
    if (!QFile::exists(program)) {
        QMessageBox::warning(this, tr("Error"), tr("Can not find \"storescu\""));
        return;
    }
    QStringList args;
    args << getIP();
    args << getPort();
    args << "-aet" << getAETitle();
    args << "-ll";
    args << getLogLevel();
    args << "+sd";
    args << getStoreDir();
    startProcess(program, args);
}

void EchoStoreSCU::onBtnStopClicked()
{
    killProcess();
}

void EchoStoreSCU::onProcessReadyRead()
{
    QString text = getCurrentTimeString4Log();
    text += QString::fromLocal8Bit(m_process->readAll());
    appendTextEditConsoleText(text);
}

void EchoStoreSCU::onProcessStateChanged(QProcess::ProcessState state)
{
    switch (state)
    {
    case QProcess::NotRunning:
        setStatus("NotRunning");
        break;
    case QProcess::Starting:
        setStatus("Starting");
        break;
    case QProcess::Running:
        setStatus("Running");
        break;
    default:
        break;
    }
}
