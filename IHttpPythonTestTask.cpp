#include "IHttpPythonTestTask.h"
#include "core/config/IContextImport.h"
#include "core/application/IAsioApplication.h"

$IPackageBegin(IPubCore, IHttpPythonTest)

void IHttpPythonTestTask::$task()
{
    std::thread thread([&](){
        checkPytestExist();
        checkScriptDir();

        if(getScriptDir().isEmpty()){
            return;
        }

        startTest();
    });
    thread.detach();
}

QString IHttpPythonTestTask::getScriptDir()
{
    $ContextQString contextPath{CLASS_NAME.toStdString()+"/scriptDir"};
    if(contextPath.isLoadedValue()){
        return contextPath.value();
    }

    auto path = QFileInfo(IAsioApplication::instance()->arguments().first()).absolutePath() + "/pytest";
    qDebug() << path;
    if(QDir(path).exists()){
        return path;
    }

    return {};
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

void IHttpPythonTestTask::checkScriptDir()
{
    if(getScriptDir().isEmpty()){
        QString tip = CLASS_NAME + " script dir not found";
        qWarning() << tip << endl;
    }
}

void IHttpPythonTestTask::startTest()
{   
    QProcess* process = new QProcess();
    process->setWorkingDirectory(getScriptDir());

    QStringList arguments;
    arguments << "--html=report.html"
              << "--self-contained-html";

    process->start("pytest", arguments);
    process->waitForFinished();
    qDebug().noquote() << QString(process->readAll());
    delete process;
    openTest();
}

void IHttpPythonTestTask::openTest()
{
    QProcess* process = new QProcess();
    process->setWorkingDirectory(getScriptDir());
    process->start("cmd", QStringList{"/c", "start", "report.html"});
    process->waitForFinished();
    qDebug() << QString(process->readAll());
    delete process;
}

void IHttpPythonTestTask::writeDebugInfo()
{
    auto path = getScriptDir() + "/common.py";
}

$IPackageEnd(IPubCore, IHttpPythonTest)
