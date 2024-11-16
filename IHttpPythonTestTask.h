﻿#pragma once

#include "core/util/IHeaderUtil.h"
#include "core/test/ITestTaskInterface.h"
#include "core/unit/IClassNameUnit.h"

$IPackageBegin(IPubCore, IHttpPythonTest)

class IHttpPythonTestTask : public ITestTaskInterface<IHttpPythonTestTask>
{
public:
    IHttpPythonTestTask() = default;

public:
    virtual void $task() final;

private:
    bool isTaskEnabled();
    void checkPytestExist();

private:
    QString getScriptDir();
    QString getContextScriptDir();
    QString getApplicationScriptDir();
    QString getSourceRootScriptDir();

private:
    void startTest();
    void openTest();
    void writeConfig();

private:
    QString m_scriptDir;
};

$IPackageEnd(IPubCore, IHttpPythonTest)
