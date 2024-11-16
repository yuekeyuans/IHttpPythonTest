#pragma once

#include "core/util/IHeaderUtil.h"
#include "core/test/ITestTaskInterface.h"
#include "core/unit/IClassNameUnit.h"

$IPackageBegin(IPubCore, IHttpPythonTest)

class IHttpPythonTestTask
    : public ITestTaskInterface<IHttpPythonTestTask>//, public IClassNameUnit<IHttpPythonTestTask>
{
public:
    IHttpPythonTestTask() = default;

public:
    virtual void $task() final;

private:
    QString getScriptDir();
    void checkPytestExist();
    void checkScriptDir();
    void startTest();
    void openTest();
    void writeDebugInfo();
};


$IPackageEnd(IPubCore, IHttpPythonTest)
