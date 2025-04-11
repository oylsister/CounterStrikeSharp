/*
 *  This file is part of CounterStrikeSharp.
 *  CounterStrikeSharp is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  CounterStrikeSharp is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with CounterStrikeSharp.  If not, see <https://www.gnu.org/licenses/>. *
 */

// clang-format off
#include "mm_plugin.h"
#include "core/timer_system.h"
#include "scripting/autonative.h"
#include "scripting/script_engine.h"
#include "core/function.h"
#include "pch.h"
#include <funchook.h>

// clang-format on

namespace counterstrikesharp {

struct HookMetadata {
    void* originalFunction; // Pointer to the original function
    std::vector<void*> arguments; // Arguments passed to the function
    void* returnValue; // Return value of the function
};

std::map<void*, HookMetadata> g_HookMetadataMap; // Global map to track hooks

void DHookGetReturn(ScriptContext& script_context)
{
    auto hookAddress = script_context.GetArgument<void*>(0);
    auto dataType = script_context.GetArgument<DataType_t>(1);

    auto it = g_HookMetadataMap.find(hookAddress);
    if (it == g_HookMetadataMap.end()) {
        script_context.ThrowNativeError("Invalid hook");
        return;
    }

    auto& metadata = it->second;

    switch (dataType) {
    case DATA_TYPE_BOOL:
        script_context.SetResult(*reinterpret_cast<bool*>(metadata.returnValue));
        break;
    case DATA_TYPE_CHAR:
        script_context.SetResult(*reinterpret_cast<char*>(metadata.returnValue));
        break;
    case DATA_TYPE_UCHAR:
        script_context.SetResult(*reinterpret_cast<unsigned char*>(metadata.returnValue));
        break;
    case DATA_TYPE_SHORT:
        script_context.SetResult(*reinterpret_cast<short*>(metadata.returnValue));
        break;
    case DATA_TYPE_USHORT:
        script_context.SetResult(*reinterpret_cast<unsigned short*>(metadata.returnValue));
        break;
    case DATA_TYPE_INT:
        script_context.SetResult(*reinterpret_cast<int*>(metadata.returnValue));
        break;
    case DATA_TYPE_UINT:
        script_context.SetResult(*reinterpret_cast<unsigned int*>(metadata.returnValue));
        break;
    case DATA_TYPE_LONG:
        script_context.SetResult(*reinterpret_cast<long*>(metadata.returnValue));
        break;
    case DATA_TYPE_ULONG:
        script_context.SetResult(*reinterpret_cast<unsigned long*>(metadata.returnValue));
        break;
    case DATA_TYPE_LONG_LONG:
        script_context.SetResult(*reinterpret_cast<long long*>(metadata.returnValue));
        break;
    case DATA_TYPE_ULONG_LONG:
        script_context.SetResult(*reinterpret_cast<unsigned long long*>(metadata.returnValue));
        break;
    case DATA_TYPE_FLOAT:
        script_context.SetResult(*reinterpret_cast<float*>(metadata.returnValue));
        break;
    case DATA_TYPE_DOUBLE:
        script_context.SetResult(*reinterpret_cast<double*>(metadata.returnValue));
        break;
    case DATA_TYPE_POINTER:
        script_context.SetResult(*reinterpret_cast<void**>(metadata.returnValue));
        break;
    case DATA_TYPE_STRING:
        script_context.SetResult(*reinterpret_cast<const char**>(metadata.returnValue));
        break;
    default:
        assert(!"Unknown function parameter type!");
        break;
    }
}

void DHookSetReturn(ScriptContext& script_context)
{
    auto hookAddress = script_context.GetArgument<void*>(0);
    auto dataType = script_context.GetArgument<DataType_t>(1);

    auto it = g_HookMetadataMap.find(hookAddress);
    if (it == g_HookMetadataMap.end()) {
        script_context.ThrowNativeError("Invalid hook");
        return;
    }

    auto& metadata = it->second;
    auto valueIndex = 2;

    switch (dataType) {
    case DATA_TYPE_BOOL:
        metadata.returnValue = new bool(script_context.GetArgument<bool>(valueIndex));
        break;
    case DATA_TYPE_CHAR:
        metadata.returnValue = new char(script_context.GetArgument<char>(valueIndex));
        break;
    case DATA_TYPE_UCHAR:
        metadata.returnValue = new unsigned char(script_context.GetArgument<unsigned char>(valueIndex));
        break;
    case DATA_TYPE_SHORT:
        metadata.returnValue = new short(script_context.GetArgument<short>(valueIndex));
        break;
    case DATA_TYPE_USHORT:
        metadata.returnValue = new unsigned short(script_context.GetArgument<unsigned short>(valueIndex));
        break;
    case DATA_TYPE_INT:
        metadata.returnValue = new int(script_context.GetArgument<int>(valueIndex));
        break;
    case DATA_TYPE_UINT:
        metadata.returnValue = new unsigned int(script_context.GetArgument<unsigned int>(valueIndex));
        break;
    case DATA_TYPE_LONG:
        metadata.returnValue = new long(script_context.GetArgument<long>(valueIndex));
        break;
    case DATA_TYPE_ULONG:
        metadata.returnValue = new unsigned long(script_context.GetArgument<unsigned long>(valueIndex));
        break;
    case DATA_TYPE_LONG_LONG:
        metadata.returnValue = new long long(script_context.GetArgument<long long>(valueIndex));
        break;
    case DATA_TYPE_ULONG_LONG:
        metadata.returnValue = new unsigned long long(script_context.GetArgument<unsigned long long>(valueIndex));
        break;
    case DATA_TYPE_FLOAT:
        metadata.returnValue = new float(script_context.GetArgument<float>(valueIndex));
        break;
    case DATA_TYPE_DOUBLE:
        metadata.returnValue = new double(script_context.GetArgument<double>(valueIndex));
        break;
    case DATA_TYPE_POINTER:
        metadata.returnValue = new void*(script_context.GetArgument<void*>(valueIndex));
        break;
    case DATA_TYPE_STRING:
        metadata.returnValue = new const char*(script_context.GetArgument<const char*>(valueIndex));
        break;
    default:
        assert(!"Unknown function parameter type!");
        break;
    }
}

void DHookGetParam(ScriptContext& script_context)
{
    auto hook = script_context.GetArgument<dyno::IHook*>(0);
    auto dataType = script_context.GetArgument<DataType_t>(1);
    auto paramIndex = script_context.GetArgument<int>(2);
    if (hook == nullptr) {
        script_context.ThrowNativeError("Invalid hook");
    }

    switch (dataType) {
    case DATA_TYPE_BOOL:
        script_context.SetResult(hook->getArgument<bool>(paramIndex));
        break;
    case DATA_TYPE_CHAR:
        script_context.SetResult(hook->getArgument<char>(paramIndex));
        break;
    case DATA_TYPE_UCHAR:
        script_context.SetResult(hook->getArgument<unsigned char>(paramIndex));
        break;
    case DATA_TYPE_SHORT:
        script_context.SetResult(hook->getArgument<short>(paramIndex));
        break;
    case DATA_TYPE_USHORT:
        script_context.SetResult(hook->getArgument<unsigned short>(paramIndex));
        break;
    case DATA_TYPE_INT:
        script_context.SetResult(hook->getArgument<int>(paramIndex));
        break;
    case DATA_TYPE_UINT:
        script_context.SetResult(hook->getArgument<unsigned int>(paramIndex));
        break;
    case DATA_TYPE_LONG:
        script_context.SetResult(hook->getArgument<long>(paramIndex));
        break;
    case DATA_TYPE_ULONG:
        script_context.SetResult(hook->getArgument<unsigned long>(paramIndex));
        break;
    case DATA_TYPE_LONG_LONG:
        script_context.SetResult(hook->getArgument<long long>(paramIndex));
        break;
    case DATA_TYPE_ULONG_LONG:
        script_context.SetResult(hook->getArgument<unsigned long long>(paramIndex));
        break;
    case DATA_TYPE_FLOAT:
        script_context.SetResult(hook->getArgument<float>(paramIndex));
        break;
    case DATA_TYPE_DOUBLE:
        script_context.SetResult(hook->getArgument<double>(paramIndex));
        break;
    case DATA_TYPE_POINTER:
        script_context.SetResult(hook->getArgument<void*>(paramIndex));
        break;
    case DATA_TYPE_STRING:
        script_context.SetResult(hook->getArgument<const char*>(paramIndex));
        break;
    default:
        assert(!"Unknown function parameter type!");
        break;
    }
}

void DHookSetParam(ScriptContext& script_context)
{
    auto hook = script_context.GetArgument<dyno::IHook*>(0);
    auto dataType = script_context.GetArgument<DataType_t>(1);
    auto paramIndex = script_context.GetArgument<int>(2);
    if (hook == nullptr) {
        script_context.ThrowNativeError("Invalid hook");
    }

    auto valueIndex = 3;

    switch (dataType) {
    case DATA_TYPE_BOOL:
        hook->setArgument(paramIndex, script_context.GetArgument<bool>(valueIndex));
        break;
    case DATA_TYPE_CHAR:
        hook->setArgument(paramIndex, script_context.GetArgument<char>(valueIndex));
        break;
    case DATA_TYPE_UCHAR:
        hook->setArgument(paramIndex, script_context.GetArgument<unsigned char>(valueIndex));
        break;
    case DATA_TYPE_SHORT:
        hook->setArgument(paramIndex, script_context.GetArgument<short>(valueIndex));
        break;
    case DATA_TYPE_USHORT:
        hook->setArgument(paramIndex, script_context.GetArgument<unsigned short>(valueIndex));
        break;
    case DATA_TYPE_INT:
        hook->setArgument(paramIndex, script_context.GetArgument<int>(valueIndex));
        break;
    case DATA_TYPE_UINT:
        hook->setArgument(paramIndex, script_context.GetArgument<unsigned int>(valueIndex));
        break;
    case DATA_TYPE_LONG:
        hook->setArgument(paramIndex, script_context.GetArgument<long>(valueIndex));
        break;
    case DATA_TYPE_ULONG:
        hook->setArgument(paramIndex, script_context.GetArgument<unsigned long>(valueIndex));
        break;
    case DATA_TYPE_LONG_LONG:
        hook->setArgument(paramIndex, script_context.GetArgument<long long>(valueIndex));
        break;
    case DATA_TYPE_ULONG_LONG:
        hook->setArgument(paramIndex, script_context.GetArgument<unsigned long long>(valueIndex));
        break;
    case DATA_TYPE_FLOAT:
        hook->setArgument(paramIndex, script_context.GetArgument<float>(valueIndex));
        break;
    case DATA_TYPE_DOUBLE:
        hook->setArgument(paramIndex, script_context.GetArgument<double>(valueIndex));
        break;
    case DATA_TYPE_POINTER:
        hook->setArgument(paramIndex, script_context.GetArgument<void*>(valueIndex));
        break;
    case DATA_TYPE_STRING:
        hook->setArgument(paramIndex, script_context.GetArgument<const char*>(valueIndex));
        break;
    default:
        assert(!"Unknown function parameter type!");
        break;
    }
}

REGISTER_NATIVES(dynamichooks, {
    ScriptEngine::RegisterNativeHandler("DYNAMIC_HOOK_GET_RETURN", DHookGetReturn);
    ScriptEngine::RegisterNativeHandler("DYNAMIC_HOOK_SET_RETURN", DHookSetReturn);
    ScriptEngine::RegisterNativeHandler("DYNAMIC_HOOK_GET_PARAM", DHookGetParam);
    ScriptEngine::RegisterNativeHandler("DYNAMIC_HOOK_SET_PARAM", DHookSetParam);
})
} // namespace counterstrikesharp
