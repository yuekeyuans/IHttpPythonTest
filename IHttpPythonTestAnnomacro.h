#pragma once

#include "core/config/IConfigPreProcessor.h"

#define $EnabledHttpPythonTest(boolValue)   \
    PP_NORMAL_CONTEXT_CONFIG(IHttpPythonTest_EnabledHttpPythonTest, /test/pytest/enabled, boolValue)

