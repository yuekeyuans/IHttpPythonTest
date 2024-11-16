#include "IHttpPythonTestTask.h"
#include "core/config/IContextImport.h"
#include "core/application/IAsioApplication.h"

$IPackageBegin(IPubCore, IHttpPythonTest)

void IHttpPythonTestTask::$task()
{
    std::thread thread([&](){
        if(!isTaskEnabled()){
            qDebug() << CLASS_NAME << "disabled";
            return;
        }
        checkPytestExist();

        m_scriptDir = getScriptDir();
        if(m_scriptDir.isEmpty()){
            qDebug() << CLASS_NAME << "script dir not exist";
            return;
        }
        writeConfig();
        startTest();
    });
    thread.detach();
}

bool IHttpPythonTestTask::isTaskEnabled()
{
    $ContextBool pyTestEnabled{"/test/pytest/enabled", true};
    if(!pyTestEnabled.value()){
        return false;
    }
    return true;
}

void IHttpPythonTestTask::checkPytestExist()
{
    QString pythonCmd = "python";

    QProcess checkPython;
    checkPython.start("python3", QStringList() << "--version");
    checkPython.waitForFinished();
    if (checkPython.exitCode() == 0) {
        pythonCmd = "python3";
    }

    QProcess process;
    process.start(pythonCmd, QStringList() << "-m" << "pytest" << "--version");
    if (!process.waitForFinished()) {
        QString tip = "Error executing command:" + process.errorString();
        qFatal(tip.toUtf8());
    }
    QByteArray result = process.readAllStandardOutput();
    if (!result.isEmpty()) {
        return;
    }

    QByteArray error = process.readAllStandardError();
    if (!error.isEmpty()) {
        QString tip =  "Error:" + error;
        qFatal(tip.toUtf8());
    } else {
        qFatal("pytest is not installed, please install it.");
    }
}

QString IHttpPythonTestTask::getScriptDir()
{
    QVector<decltype(&IHttpPythonTestTask::getContextScriptDir)> funs{
        &IHttpPythonTestTask::getApplicationScriptDir,
        &IHttpPythonTestTask::getContextScriptDir,
        &IHttpPythonTestTask::getSourceRootScriptDir
    };

    for(auto fun : funs){
        QString value = std::mem_fn(fun)(this);
        if(!value.isEmpty()){
            return value;
        }
    }
    return {};
}

QString IHttpPythonTestTask::getContextScriptDir()
{
    $ContextQString path{"/test/pytest/scriptDir"};
    if(path.value().isEmpty()){
        return {};
    }
    if(!QDir(path.value()).exists()){
        qDebug() << path.value() << "script dir not exist";
    }
    return path.value();
}

QString IHttpPythonTestTask::getApplicationScriptDir()
{
    auto path = QFileInfo(IAsioApplication::instance()->arguments().first()).absolutePath() + "/pytest";
    if(QDir(path).exists()){
        return path;
    }
    path = QFileInfo(IAsioApplication::instance()->arguments().first()).absolutePath() + "/test/pytest";
    if(QDir(path).exists()){
        return path;
    }
    return {};
}

QString IHttpPythonTestTask::getSourceRootScriptDir()
{
#ifdef QMAKE_PROJECT_DIR
    auto path = QString(QMAKE_PROJECT_DIR) + "/pytest";
    if(QDir(path).exists()){
        return path;
    }

    path = QString(QMAKE_PROJECT_DIR) + "/test/pytest";
    if(QDir(path).exists()){
        return path;
    }
#endif
    return {};
}

void IHttpPythonTestTask::startTest()
{   
    QProcess* process = new QProcess();
    process->setWorkingDirectory(m_scriptDir);

    QStringList arguments;
    arguments << "--html=report.html"
              << "--self-contained-html";

    process->start("pytest", arguments);
    process->waitForFinished();
    qDebug().noquote() << process->readAll();
    delete process;
    openTest();
}

void IHttpPythonTestTask::openTest()
{
    QProcess* process = new QProcess();
    process->setWorkingDirectory(m_scriptDir);
    process->start("cmd", QStringList{"/c", "start", "report.html"});
    process->waitForFinished();
    delete process;
}

void IHttpPythonTestTask::writeConfig()
{
    auto path = m_scriptDir + "/ServerConfig.py";
    QFile file(path);
    file.open(QFile::WriteOnly);
    QTextStream stream(&file);

    $ContextQString ip{"/runtime/ip"};
    $ContextInt port{"/runtime/port"};
    stream << "port=\"" << *port << "\"" << endl;
    stream << "ip=\"" << *ip << "\"" << endl;
    stream << "serverAddress=\"http://" << *ip << ":" << *port << "\"" << endl;

    file.close();
}

$IPackageEnd(IPubCore, IHttpPythonTest)
