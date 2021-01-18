#include "Terminal.h"
#include <QDir>
#include <QTextStream>
#include <QDateTime>
#include <QMessageBox>
#include <QDesktopServices>
#include <QDebug>

Terminal::Terminal(QWidget *parent)
    : QWidget(parent)
    , m_recordIndex(0)
    , m_startFlag(false)
{
    ui.setupUi(this);
    connect(ui.btnEnter, SIGNAL(clicked()), this, SLOT(onBtnEnetrClicked()));
    ui.textEdit->setReadOnly(true);

    ui.lineEdit->installEventFilter(this);
    connect(ui.btnStart, SIGNAL(clicked()), this, SLOT(onBtnStartClicked()));
    connect(ui.btnStop, SIGNAL(clicked()), this, SLOT(onBtnStopClicked()));
    connect(ui.btnRestart, SIGNAL(clicked()), this, SLOT(onBtnRestartClicked()));
    connect(ui.btnCls, SIGNAL(clicked()), this, SLOT(onBtnClsClicked()));
    connect(ui.btnSaveToFile, SIGNAL(clicked()), this, SLOT(onBtnSaveToFileClicked()));

    m_process = new QProcess;
    m_process->setProcessChannelMode(QProcess::MergedChannels);
    connect(m_process, SIGNAL(readyReadStandardOutput()), this, SLOT(onProcessReadyRead()));
    connect(m_process, SIGNAL(readyReadStandardError()), this, SLOT(onProcessReadyRead()));
    connect(m_process, SIGNAL(stateChanged(QProcess::ProcessState)), 
        this, SLOT(onProcessStateChanged(QProcess::ProcessState)));
#if defined(Q_OS_WIN)
//    m_process->setWorkingDirectory(QCoreApplication::applicationDirPath() + "\\bin\\win32");
#elif defined(Q_OS_LINUX)
    m_process->setWorkingDirectory(QCoreApplication::applicationDirPath() + "/bin/linux");
#else
#endif
    start();
}

Terminal::~Terminal()
{
    stop();
}

bool Terminal::start()
{
    m_startFlag = true;
#if defined(Q_OS_WIN)
    m_process->start("bash", QStringList());
#elif defined(Q_OS_LINUX)
    m_process->start("bash", QStringList());
#else
    return false;
#endif
    return m_process->waitForStarted();
}
bool Terminal::stop()
{
    m_startFlag = false;
    m_process->kill();
    return m_process->waitForFinished();
}
bool Terminal::restart()
{
    return start() && stop();
}

void Terminal::onProcessReadyRead()
{
    QString text = QString::fromLocal8Bit(m_process->readAllStandardOutput());
    if (m_startFlag) {
        m_startFlag = false;
    } else {
        int index = text.indexOf("\n\r\n");
        if (index >= 0) {
            text.remove(0, index + 3);
        }
    }
    appendConsoleText(text);
}

void Terminal::onBtnEnetrClicked()
{
    m_recordIndex = 0;
    QString cmd = getCommand();
    if (!cmd.isEmpty()) {
        appendCmdRecord(cmd);
    }
    appendLineFeedsToCmd(cmd);
#ifdef Q_OS_WIN
    m_process->write(cmd.toLocal8Bit());
#elif defined(Q_OS_LINUX)
    m_process->write(cmd.toLocal8Bit());
#else
#endif
    ui.lineEdit->clear();
}

void Terminal::onBtnStartClicked()
{
    auto state = m_process->state();
    if (state == QProcess::NotRunning) {
        start();
    }
}
void Terminal::onBtnStopClicked()
{
    auto state = m_process->state();
    if (state != QProcess::NotRunning) {
        stop();
    }
}
void Terminal::onBtnRestartClicked()
{
    auto state = m_process->state();
    if (state != QProcess::NotRunning) {
        stop();
    }
    start();
}
void Terminal::onBtnClsClicked()
{
    clearConsoleText();
}

void Terminal::onBtnSaveToFileClicked()
{
    QString logDir = QDir::currentPath() + "/log/";
    QDir dir;
    dir.mkdir(logDir);
    QDateTime dt = QDateTime::currentDateTime();
    QString fileName = logDir + dt.toString("yyyy-MM-dd_hh.mm.ss.zzz") + QString(".terminal.output.txt");
    QFile file(fileName);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text))
    {
        QMessageBox::warning(this, tr("Error"), QString("create file failed : ") + fileName);
        return;
    }
    QTextStream stream(&file);
    stream.setCodec("UTF-8");
    stream.setGenerateByteOrderMark(false);
    stream << getConsoleText();
    file.close();

    QMessageBox::StandardButton btn =
        QMessageBox::question(this, tr("Open It or not ? "), QString("Save file successfully. Open it or not?\n") + fileName);
    if (btn == QMessageBox::Yes) {
        QDesktopServices::openUrl(QUrl(fileName));
    }
}

QString Terminal::getCommand() const
{
    return ui.lineEdit->text();
}

void Terminal::appendLineFeedsToCmd(QString& cmd)
{
    if (!cmd.endsWith("\n")) {
        cmd += "\n";
    }
}

QString Terminal::getCommandWithLineFeeds() const
{
    QString text = ui.lineEdit->text();
    if (!text.endsWith("\n")) {
        text += "\n";
    }
    return text;
}

void Terminal::setCommand(const QString& text)
{
    ui.lineEdit->setText(text);
}

void Terminal::setConsoleText(const QString& text)
{
    ui.textEdit->setPlainText(text);
}

void Terminal::appendConsoleText(const QString& text)
{
    ui.textEdit->appendPlainText(text);
}

void Terminal::clearConsoleText()
{
    ui.textEdit->clear();
}

QString Terminal::getConsoleText() const
{
    return ui.textEdit->toPlainText();
}

void Terminal::setStatus(const QString& text)
{
    ui.labelStatus->setText(text);
}

bool Terminal::eventFilter(QObject *target, QEvent *event)
{
    if (target == ui.lineEdit) {
        if (event->type() == QEvent::KeyPress) {
            QKeyEvent* keyEvent = static_cast<QKeyEvent *>(event);
            if (keyEvent->key() == Qt::Key_Up) {
                if (m_recordIndex < m_cmdRecord.size()) {
                    QString cmd = getCmdRecordAt(m_recordIndex);
                    if (cmd == getCommand()) {
                        if (m_recordIndex < m_cmdRecord.size() - 1) {
                            ++m_recordIndex;
                        }
                    }
                    setCommand(getCmdRecordAt(m_recordIndex));
                    if (m_recordIndex < m_cmdRecord.size() - 1) {
                        ++m_recordIndex;
                    }
                }        
            }
            else if (keyEvent->key() == Qt::Key_Down) {
                QString cmd = getCmdRecordAt(m_recordIndex);
                if (cmd == getCommand()) {
                    if (m_recordIndex > 0) {
                        --m_recordIndex;
                    }
                }
                setCommand(getCmdRecordAt(m_recordIndex));
                if (m_recordIndex > 0) {
                    --m_recordIndex;
                }   
            }
            QWidget::eventFilter(target, event);
        }
    }
    return QWidget::eventFilter(target, event);
}

void Terminal::appendCmdRecord(const QString& context)
{
    if (m_cmdRecord.size() == MaxCmdRecordSize) {
        m_cmdRecord.takeFirst();
    }

    if (m_cmdRecord.isEmpty()) {
        m_cmdRecord.append(context);
    }
    else {
        if (context != m_cmdRecord.back()) {
            m_cmdRecord.append(context);
        }
    }
}

QString Terminal::getCmdRecordAt(int index)
{
    if (m_cmdRecord.size() <= index) {
        return QString();
    }
    return *(m_cmdRecord.rbegin() + index);
}

void Terminal::onProcessStateChanged(QProcess::ProcessState newState)
{
    switch (newState)
    {
    case QProcess::NotRunning:
        setStatus("Not Runing");
        break;
    case QProcess::Starting:
        setStatus("Starting");
        break;
    case QProcess::Running:
        setStatus("Runing");
        break;
    default:
        break;
    }
}