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

#include "Settings.h"

EchoStoreSCU::EchoStoreSCU(QWidget *parent)
    : QWidget(parent)
    , m_process(new QProcess)
{
    ui = new Ui::EchoStoreSCU();
    ui->setupUi(this);

    initUI();
    connectSignalSlots();
}

EchoStoreSCU::~EchoStoreSCU()
{
    delete ui;
}

void EchoStoreSCU::initUI()
{
    ui->textEditConsole->setReadOnly(true);

    auto set = Settings::getEchoStoreSCUSettings();
    setAETitle(set.aeTitle);
    setIP(set.IP);
    setPort(set.port);
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
    connect(m_process, SIGNAL(readyReadStandardOutput()), 
        this, SLOT(onProcessReadyReadStandardOutput()));
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

QString EchoStoreSCU::getStoreDir() const
{
    return ui->lineEditDir->text();
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

void EchoStoreSCU::saveSettings()
{
    EchoStoreSCUSettings set;
    set.aeTitle = getAETitle();
    set.IP = getIP();
    set.port = getPort();
    Settings::saveEchoStoreSCUSettings(set);
}

void EchoStoreSCU::onBtnChooseFilesClicked()
{
    auto fileList = QFileDialog::getOpenFileNames(this, tr("Choose DICOM Files"));
    if (fileList.isEmpty()) {
        return;
    }
    QString text;
    for (int i = 0; i < fileList.count(); ++i) {
        text.append(fileList[i]);
        if (i != fileList.count() - 1) {
            text.append("\n");
        }
    }
    ui->textEditFiles->clear();
    ui->textEditFiles->setText(text);
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
    ui->lineEditDir->setText(text);
}

void EchoStoreSCU::onBtnSaveOutputToFileClicked()
{
    QString logDir = QDir::currentPath() + "/log/";
    QDir dir;
    dir.mkdir(logDir);
    QDateTime dt = QDateTime::currentDateTime();
    QString fileName = logDir + getCurrentTimeString() + QString(".output.txt");
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
    QString program = "bin/echoscu.exe";
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
    QString program = "bin/storescu.exe";
    if (!QFile::exists(program)) {
        QMessageBox::warning(this, tr("Error"), tr("Can not find \"storescu\""));
        return;
    }
    QStringList args;
    args << getIP();
    args << getPort();
    args << "-aet" << getAETitle();
    args << getStoreDir();
    args << "+sd";
    startProcess(program, args);
}

void EchoStoreSCU::onProcessReadyReadStandardOutput()
{
    QString text = getCurrentTimeString4Log();
    text += QString::fromLocal8Bit(m_process->readAllStandardOutput());
    appendTextEditConsoleText(text);
}
