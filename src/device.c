/* The MIT License (MIT)
 *
 * Copyright (c) 2017 Stefano Trettel
 *
 * Software repository: MoonAL, https://github.com/stetre/moonal
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

#include "internal.h"

typedef struct {
    ALCvoid *buffer;
    ALCsizei framesize;
    ALCsizei maxframes;
    ALCsizei buffersize;
} udinfo_t;


static int freedevice(lua_State *L, ud_t *ud)
    {
    device_t device = (device_t)ud->handle;
    int capture = IsCaptureDevice(ud);
    udinfo_t *udinfo = (udinfo_t*)ud->info;
    ud->info = NULL; /* prevent Free() */
    freechildren(L, CONTEXT_MT, ud);
    if(ud->ddt) Free(L, ud->ddt);
    if(udinfo)
        {
        Free(L, udinfo->buffer);
        Free(L, udinfo);
        }
    if(!freeuserdata(L, ud)) return 0;
    TRACE_DELETE(device, "device");
    if(capture)
        alc.CaptureCloseDevice(device);
    else /* playback or loopback device */
        alc.CloseDevice(device);
    return 0;
    }

static ud_t *newdevice(lua_State *L, device_t device)
    {
    ud_t *ud;
    ud = newuserdata(L, device, DEVICE_MT);
    ud->device = device;
    ud->parent_ud = NULL;
    ud->destructor = freedevice;
    ud->ddt = getproc_device(L, device);
    TRACE_CREATE(device, "device");
    return ud;
    }

static int OpenDevice(lua_State *L)
    {
    const ALCchar *name = luaL_optstring(L, 1, NULL);
    device_t device = alc.OpenDevice(name);
    if(!device)
        {
        CheckErrorAlc(L, NULL);
        return 0;
        }
    newdevice(L, device);
    return 1;
    }


/*---------------------------------------------------------------------------*/

static int CaptureOpenDevice(lua_State *L)
    {
    ud_t *ud;
    udinfo_t *udinfo;
    device_t device;
    const ALCchar *devicename = luaL_optstring(L, 1, NULL);
    ALCuint frequency = luaL_checkinteger(L, 2);
    ALCenum format = checkformat(L, 3);
    ALCsizei maxframes = luaL_checkinteger(L, 4); /* buffersize in no. of frames */
    if(maxframes <= 0)
        return luaL_argerror(L, 4, errstring(ERR_VALUE));
    udinfo = (udinfo_t*)Malloc(L, sizeof(udinfo_t));
    if(!udinfo)
        { return luaL_error(L, errstring(ERR_MEMORY)); }

    udinfo->maxframes = maxframes;
    udinfo->framesize = formatframesize(L, format, 0);
    udinfo->buffersize = udinfo->framesize * udinfo->maxframes;
    udinfo->buffer = MallocNoErr(L, udinfo->buffersize);    
    if(!udinfo->buffer)
        { Free(L, udinfo); luaL_error(L, errstring(ERR_MEMORY)); }

    device = alc.CaptureOpenDevice(devicename, frequency, format, udinfo->buffersize);
    if(!device)
        {
        Free(L, udinfo->buffer);
        Free(L, udinfo);
        CheckErrorAlc(L, NULL);
        return 0;
        }
    ud = newdevice(L, device);
    ud->info = udinfo;
    MarkCaptureDevice(ud);
    return 1;
    }


static int CaptureStart(lua_State *L)
    {
    ud_t *ud;
    device_t device = checkdevice(L, 1, &ud);
    if(!IsCaptureDevice(ud)) 
        return luaL_argerror(L, 1, "not a capture device");
    alc.CaptureStart(device);
    CheckErrorAlc(L, device);
    return 0;
    }

static int CaptureStop(lua_State *L)
    {
    ud_t *ud;
    device_t device = checkdevice(L, 1, &ud);
    if(!IsCaptureDevice(ud)) 
        return luaL_argerror(L, 1, "not a capture device");
    alc.CaptureStop(device);
    CheckErrorAlc(L, device);
    return 0;
    }

static int CaptureSamples(lua_State *L)
    {
    ud_t *ud;
    ALCint avail;
    device_t device = checkdevice(L, 1, &ud);
    udinfo_t *udinfo = (udinfo_t*)ud->info;
    ALCsizei frames = luaL_checkinteger(L, 2); 
    if(!IsCaptureDevice(ud)) 
        return luaL_argerror(L, 1, "not a capture device");
    if(frames > udinfo->maxframes) /* check that frames fit in buffer */
        return luaL_argerror(L, 2, "requested too many frames");
    alc.GetIntegerv(device, ALC_CAPTURE_SAMPLES, 1, &avail);    
    CheckErrorAlc(L, device);
    if(avail < frames)
        return 0; /* not enough available frames */
    alc.CaptureSamples(device, udinfo->buffer, frames);
    CheckErrorAlc(L, device);
    lua_pushlstring(L, (const char*)udinfo->buffer, frames * udinfo->framesize);
    return 1;
    }

/*---------------------------------------------------------------------------*/

static int LoopbackOpenDevice(lua_State *L) 
    {
    ud_t *ud;
    udinfo_t *udinfo;
    device_t device;
    const ALCchar *devicename = luaL_optstring(L, 1, NULL);
    ALCsizei maxframes = luaL_checkinteger(L, 2);
    ALCsizei maxframesize = luaL_checkinteger(L, 3);
    ALCsizei maxbytes = luaL_checkinteger(L, 2);
    if(maxbytes <= 0)
        return luaL_argerror(L, 2, errstring(ERR_VALUE));

    CheckAlcPfn(L, LoopbackOpenDeviceSOFT); 

    udinfo = (udinfo_t*)Malloc(L, sizeof(udinfo_t));
    if(!udinfo)
        { return luaL_error(L, errstring(ERR_MEMORY)); }
    udinfo->buffersize = maxbytes;
    udinfo->maxframes = maxframes;
    udinfo->framesize = maxframesize;
    udinfo->buffer = MallocNoErr(L, udinfo->buffersize);    
    if(!udinfo->buffer)
        { Free(L, udinfo); luaL_error(L, errstring(ERR_MEMORY)); }

    device = alc.LoopbackOpenDeviceSOFT(devicename);
    if(!device)
        {
        Free(L, udinfo->buffer);
        Free(L, udinfo);
        CheckErrorAlc(L, NULL);
        return 0;
        }
    ud = newdevice(L, device);
    ud->info = udinfo;
    MarkLoopbackDevice(ud);
    return 1;
    }


static int IsRenderFormatSupported(lua_State *L) 
    {
    ALCboolean res;
    ud_t *ud;
    device_t device = checkdevice(L, 1, &ud);
    ALCsizei freq = luaL_checkinteger(L, 2);
    ALCenum channels = checkchannels(L, 3);
    ALCenum type = checktypesoft(L, 4);
    if(!IsLoopbackDevice(ud)) 
        return luaL_argerror(L, 1, "not a loopback device");
    CheckAlcPfn(L, IsRenderFormatSupportedSOFT); 
    res = alc.IsRenderFormatSupportedSOFT(device, freq, channels, type);
    CheckErrorAlc(L, device);
    lua_pushboolean(L, res);
    return 1;
    }

static int RenderSamples(lua_State *L)
    {
    ud_t *ud;
    device_t device = checkdevice(L, 1, &ud);
    udinfo_t *udinfo = (udinfo_t*)ud->info;
    ALCsizei frames = luaL_checkinteger(L, 2);
    ALCsizei framesize = luaL_checkinteger(L, 3);
    ALCsizei bytes = frames * framesize;

    if(!IsLoopbackDevice(ud)) 
        return luaL_argerror(L, 1, "not a loopback device");
    CheckAlcPfn(L, RenderSamplesSOFT); 

    if(bytes > udinfo->buffersize)
        return luaL_argerror(L, 2, "requested too many bytes of data");
    memset(udinfo->buffer, 0, frames);
    alc.RenderSamplesSOFT(device, udinfo->buffer, frames);
    CheckErrorAlc(L, device);
    lua_pushlstring(L, (const char*)udinfo->buffer, frames);
    return 1;
    }

//@@TODO: API alternative che non passano data a Lua ma tra C e C sotto il controllo di Lua

/*---------------------------------------------------------------------------*/

#define ALL_EXT (alc.IsExtensionPresent(NULL, "ALC_ENUMERATE_ALL_EXT") == ALC_TRUE)

static int DeviceName(lua_State *L)
    {
    ud_t *ud;
    device_t device = checkdevice(L, 1, &ud);
    const ALCchar* val;
    if(IsCaptureDevice(ud))
        {
        val = alc.GetString(device, ALC_CAPTURE_DEVICE_SPECIFIER);
        CheckErrorAlc(L, device);
        lua_pushstring(L, val);
        return 1;
        }
    val = ALL_EXT ?
        alc.GetString(device, ALC_ALL_DEVICES_SPECIFIER) :
        alc.GetString(device, ALC_DEVICE_SPECIFIER);
    CheckErrorAlc(L, device);
    lua_pushstring(L, val);
    return 1;
    }

static int DefaultDevices(lua_State *L)
    {
    const ALCchar* val = ALL_EXT ?
        alc.GetString(NULL, ALC_DEFAULT_ALL_DEVICES_SPECIFIER) :
        alc.GetString(NULL, ALC_DEFAULT_DEVICE_SPECIFIER); 
    CheckErrorAlc(L, NULL);
    lua_pushstring(L, val);
    val = NULL;
    val = alc.GetString(NULL, ALC_CAPTURE_DEFAULT_DEVICE_SPECIFIER);
    CheckErrorAlc(L, NULL);
    lua_pushstring(L, val);
    return 2;
    }

static int AvailableDevices(lua_State *L)
    {
    int i;
    char *p;
    const ALCchar* val = ALL_EXT ?
        alc.GetString(NULL, ALC_ALL_DEVICES_SPECIFIER) : 
        alc.GetString(NULL, ALC_DEVICE_SPECIFIER);
    CheckErrorAlc(L, NULL);
    lua_newtable(L);
    
    i = 0;
    p = firststringN(val);
    while(p)
        {
        lua_pushstring(L, p);
        lua_rawseti(L, -2, ++i);
        p = nextstringN(p);
        }
    
    val = NULL;
    val = alc.GetString(NULL, ALC_CAPTURE_DEVICE_SPECIFIER);
    CheckErrorAlc(L, NULL);
    lua_newtable(L);
    
    i = 0;
    p = firststringN(val);
    while(p)
        {
        lua_pushstring(L, p);
        lua_rawseti(L, -2, ++i);
        p = nextstringN(p);
        }

    return 2;
    }


static int HrtfSpecifiers(lua_State *L)
    {
    ud_t *ud;
    ALCint i, count;
    const ALCchar *name;
    device_t device = checkdevice(L, 1, &ud);
    alc.GetIntegerv(device, ALC_NUM_HRTF_SPECIFIERS_SOFT, 1, &count);
    CheckErrorAlc(L, device);
    
    CheckDevicePfn(L, ud, GetStringiSOFT);
    lua_newtable(L);
    for (i = 0; i < count; i++)
        {
        name = ud->ddt->GetStringiSOFT(device, ALC_HRTF_SPECIFIER_SOFT, i);
        CheckErrorAlc(L, device);
        lua_pushstring(L, name);
        lua_rawseti(L, -2, i+1);
        }
    return 1;
    }


static int DevicePause(lua_State *L)
    {
    ud_t *ud;
    device_t device = checkdevice(L, 1, &ud);
    CheckDevicePfn(L, ud, DevicePauseSOFT);
    ud->ddt->DevicePauseSOFT(device);
    CheckErrorAlc(L, device);
    return 0;
    }

static int DeviceResume(lua_State *L)
    {
    ud_t *ud;
    device_t device = checkdevice(L, 1, &ud);
    CheckDevicePfn(L, ud, DeviceResumeSOFT);
    ud->ddt->DeviceResumeSOFT(device);
    CheckErrorAlc(L, device);
    return 0;
    }


/*---------------------------------------------------------------------------*/

RAW_FUNC(device)
TYPE_FUNC(device)
PARENT_FUNC(device)
DELETE_FUNC(device)

static const struct luaL_Reg Methods[] = 
    {
        { "raw", Raw },
        { "type", Type },
        { "parent", Parent },
        { "close", Delete },
        { "name", DeviceName },
        { "start", CaptureStart },
        { "stop", CaptureStop },
        { "samples", CaptureSamples },
        { "pause", DevicePause },
        { "resume", DeviceResume },
        { "is_format_supported", IsRenderFormatSupported },
        { "render", RenderSamples },
        { NULL, NULL } /* sentinel */
    };

static const struct luaL_Reg MetaMethods[] = 
    {
        { "__gc",  Delete },
        { NULL, NULL } /* sentinel */
    };

static const struct luaL_Reg Functions[] = 
    {
        { "open_device", OpenDevice },
        { "capture_open_device", CaptureOpenDevice },
        { "close_device", Delete },
        { "capture_start", CaptureStart },
        { "capture_stop", CaptureStop },
        { "capture_samples", CaptureSamples },
        { "device_name", DeviceName },
        { "default_devices", DefaultDevices },
        { "available_devices", AvailableDevices },
        { "hrtf_specifiers", HrtfSpecifiers },
        { "device_pause", DevicePause },
        { "device_resume", DeviceResume },
        { "loopback_open_device", LoopbackOpenDevice },
        { "is_render_format_supported", IsRenderFormatSupported },
        { "render_samples", RenderSamples },
        { NULL, NULL } /* sentinel */
    };


void moonal_open_device(lua_State *L)
    {
    udata_define(L, DEVICE_MT, Methods, MetaMethods);
    luaL_setfuncs(L, Functions, 0);
    }

