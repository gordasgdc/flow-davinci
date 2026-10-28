// SPDX-License-Identifier: MIT
#pragma once
#include <cstdarg>
#include <cstdint>
#include <cstring>
// The OS wrapper supplies the SDK ABI and startup callback. This is the existing
// Mac handshake/metadata/lifecycle policy, with no codecs or containers advertised.
namespace Unbound {
using namespace IOPlugin;
template<void (*Start)()> struct IOPluginEntry {
inline static APIContext hostAPI = {};

static StatusCode handleMessage(MessageID message, ...)
{
    switch (message) {
    case msgPluginStart:
        // Reuse the validated main-queue bootstrap; advertise no media codecs.
        Start();
        return errNone;
    case msgPluginTerminate:
    case msgPluginListCodecs:
    case msgPluginListContainers:
        return errNone;
    case msgPluginGetInfo: {
        va_list args;
        va_start(args, message);
        ObjectRef properties = va_arg(args, ObjectRef);
        va_end(args);
        if (!properties) return errInvalidParam;
        static const uint8_t uuid[] = {0x6d,0x41,0x0a,0x7e,0x52,0x9c,0x4b,0x13,0x9f,0x80,0xd4,0x3b,0x61,0xe7,0x2a,0xc5};
        StatusCode status = hostAPI.pHandleMessage(msgPropSet, properties, pIOPropUUID, propTypeUInt8, uuid, 16);
        if (status != errNone) return status;
        const char *name = "Flow DaVinci — asistent pentru panouri (adaptare OpenGate)";
        return hostAPI.pHandleMessage(msgPropSet, properties, pIOPropName, propTypeString, name, int(std::strlen(name)));
    }
    default:
        return errUnsupported;
    }
}

static StatusCode initialize(const APIContext *host, APIContext *plugin)
{
    if (!host || !plugin || !host->pHandleMessage) return errInvalidParam;
    if (host->version < version) return errVersion;
    hostAPI = *host;
    plugin->version = version;
    plugin->pHandleMessage = handleMessage;
    Start();
    return errNone;
}

};
}
