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

moonal_al_dt_t al;   /* al.h dispatch table */
moonal_alc_dt_t alc; /* alc.h dispatch table */

#define FP(f) *(void**)(&(f))
/* Cast to silence compiler warnings without giving up the -Wpedantic flag.
 *("ISO C forbids conversion of function pointer to object pointer type")
 */

#if defined(LINUX)
#include <dlfcn.h>
static void *Handle = NULL;
#define LIBNAME "libopenal.so"

#elif defined(MINGW)
#include <windows.h>
#define LIBNAME "libopenal-1.dll"
#define LLIBNAME L"libopenal-1.dll"
#define LIBNAME1 "openal.dll"
#define LLIBNAME1 L"openal.dll"
static HMODULE Handle = NULL;

#else
#error "Cannot determine platform"
#endif


static LPALGETPROCADDRESS AlGetProcAddress;
static LPALCGETPROCADDRESS AlcGetProcAddress;

static int Init(lua_State *L)
    {
#if defined(LINUX)
    char *err;
    Handle = dlopen(LIBNAME, RTLD_LAZY | RTLD_LOCAL);
    if(!Handle)
        {
        err = dlerror();
        return luaL_error(L, err != NULL ? err : "cannot load " LIBNAME);
        }

    FP(AlGetProcAddress) = dlsym(Handle, "alGetProcAddress");
    FP(AlcGetProcAddress) = dlsym(Handle, "alcGetProcAddress");

#elif defined(MINGW)
    Handle = LoadLibraryW(LLIBNAME);
    if(!Handle)
        Handle = LoadLibraryW(LLIBNAME1);
    if(!Handle)
        return luaL_error(L, "cannot load " LIBNAME " or " LIBNAME1);

    AlGetProcAddress = (LPALGETPROCADDRESS)GetProcAddress(Handle, "alGetProcAddress");
    AlcGetProcAddress = (LPALCGETPROCADDRESS)GetProcAddress(Handle, "alcGetProcAddress");

#endif

    if(!AlGetProcAddress)
        return luaL_error(L, "cannot find alGetProcAddress");
    if(!AlcGetProcAddress)
        return luaL_error(L, "cannot find alcGetProcAddress");

    /* Fill the global dispatch tables */
#define GET(fn) do {                                            \
    FP(al.fn) = AlGetProcAddress("al"#fn);                      \
    if(!al.fn) return luaL_error(L, "cannot find al"#fn);       \
} while(0)
    GET(Enable);
    GET(Disable);
    GET(IsEnabled);
    GET(GetString);
    GET(GetBooleanv);
    GET(GetIntegerv);
    GET(GetFloatv);
    GET(GetDoublev);
    GET(GetBoolean);
    GET(GetInteger);
    GET(GetFloat);
    GET(GetDouble);
    GET(GetError);
    GET(IsExtensionPresent);
    GET(GetProcAddress);
    GET(GetEnumValue);
    GET(Listenerf);
    GET(Listener3f);
    GET(Listenerfv);
    GET(Listeneri);
    GET(Listener3i);
    GET(Listeneriv);
    GET(GetListenerf);
    GET(GetListener3f);
    GET(GetListenerfv);
    GET(GetListeneri);
    GET(GetListener3i);
    GET(GetListeneriv);
    GET(GenSources);
    GET(DeleteSources);
    GET(IsSource);
    GET(Sourcef);
    GET(Source3f);
    GET(Sourcefv);
    GET(Sourcei);
    GET(Source3i);
    GET(Sourceiv);
    GET(GetSourcef);
    GET(GetSource3f);
    GET(GetSourcefv);
    GET(GetSourcei);
    GET(GetSource3i);
    GET(GetSourceiv);
    GET(SourcePlayv);
    GET(SourceStopv);
    GET(SourceRewindv);
    GET(SourcePausev);
    GET(SourcePlay);
    GET(SourceStop);
    GET(SourceRewind);
    GET(SourcePause);
    GET(SourceQueueBuffers);
    GET(SourceUnqueueBuffers);
    GET(GenBuffers);
    GET(DeleteBuffers);
    GET(IsBuffer);
    GET(BufferData);
    GET(Bufferf);
    GET(Buffer3f);
    GET(Bufferfv);
    GET(Bufferi);
    GET(Buffer3i);
    GET(Bufferiv);
    GET(GetBufferf);
    GET(GetBuffer3f);
    GET(GetBufferfv);
    GET(GetBufferi);
    GET(GetBuffer3i);
    GET(GetBufferiv);
    GET(DopplerFactor);
    GET(DopplerVelocity);
    GET(SpeedOfSound);
    GET(DistanceModel);
#undef GET

#define GET(fn) do {                                            \
    FP(alc.fn) = AlcGetProcAddress(NULL, "alc"#fn);             \
    if(!alc.fn) return luaL_error(L, "cannot find alc"#fn);     \
} while(0)
#define OPT(fn) do {                                            \
    FP(alc.fn) = AlcGetProcAddress(NULL, "alc"#fn);             \
} while(0)
    GET(CreateContext);
    GET(MakeContextCurrent);
    GET(ProcessContext);
    GET(SuspendContext);
    GET(DestroyContext);
    GET(GetCurrentContext);
    GET(GetContextsDevice);
    GET(OpenDevice);
    GET(CloseDevice);
    GET(GetError);
    GET(IsExtensionPresent);
    GET(GetProcAddress);
    GET(GetEnumValue);
    GET(GetString);
    GET(GetIntegerv);
    GET(CaptureOpenDevice);
    GET(CaptureCloseDevice);
    GET(CaptureStart);
    GET(CaptureStop);
    GET(CaptureSamples);
//  IF("ALC_SOFT_loopback")
//      {
        GET(LoopbackOpenDeviceSOFT);
        GET(IsRenderFormatSupportedSOFT);
        GET(RenderSamplesSOFT);
//      }
#undef OPT
#undef GET
    return 0;
    }

/*----------------------------------------------------------------------------------*/

device_dt_t* getproc_device(lua_State *L, device_t device)
    {
    device_dt_t *dt = (device_dt_t*)Malloc(L, sizeof(device_dt_t));
#define IF(extensionname) if(alc.IsExtensionPresent(device, extensionname))
#define GET(fn) do {                                            \
    FP(dt->fn) = AlcGetProcAddress(device, "alc"#fn);           \
    if(!dt->fn) { Free(L, dt); luaL_error(L, "cannot find alc"#fn); return NULL; } \
} while(0)

    IF("ALC_EXT_thread_local_context")
        {
        GET(SetThreadContext);
        GET(GetThreadContext);
        }
    IF("ALC_SOFT_pause_device")
        {
        GET(DevicePauseSOFT);
        GET(DeviceResumeSOFT);
        }
    IF("ALC_SOFT_HRTF")
        {
        GET(GetStringiSOFT);
        GET(ResetDeviceSOFT);
        }
#undef GET
#define GET(fn) do {                                            \
    FP(dt->fn) = AlGetProcAddress("al"#fn);                     \
    if(!dt->fn) { Free(L, dt); luaL_error(L, "cannot find al"#fn); return NULL; } \
} while(0)
    IF("ALC_EXT_EFX")
        {
        GET(GenEffects);
        GET(DeleteEffects);
        GET(IsEffect);
        GET(Effecti);
        GET(Effectiv);
        GET(Effectf);
        GET(Effectfv);
        GET(GetEffecti);
        GET(GetEffectiv);
        GET(GetEffectf);
        GET(GetEffectfv);
        GET(GenFilters);
        GET(DeleteFilters);
        GET(IsFilter);
        GET(Filteri);
        GET(Filteriv);
        GET(Filterf);
        GET(Filterfv);
        GET(GetFilteri);
        GET(GetFilteriv);
        GET(GetFilterf);
        GET(GetFilterfv);
        GET(GenAuxiliaryEffectSlots);
        GET(DeleteAuxiliaryEffectSlots);
        GET(IsAuxiliaryEffectSlot);
        GET(AuxiliaryEffectSloti);
        GET(AuxiliaryEffectSlotiv);
        GET(AuxiliaryEffectSlotf);
        GET(AuxiliaryEffectSlotfv);
        GET(GetAuxiliaryEffectSloti);
        GET(GetAuxiliaryEffectSlotiv);
        GET(GetAuxiliaryEffectSlotf);
        GET(GetAuxiliaryEffectSlotfv);
        }
#undef GET
#undef IF
    return dt;
    }

context_dt_t* getproc_context(lua_State *L, context_t context)
    {
    context_dt_t *dt = (context_dt_t*)Malloc(L, sizeof(context_dt_t));
    context_t old_context = alc.GetCurrentContext();
    alc.MakeContextCurrent(context);

#define GET(fn) do {                                            \
    FP(dt->fn) = AlGetProcAddress("al"#fn);                     \
    if(!dt->fn) { Free(L, dt); luaL_error(L, "cannot find alc"#fn); return NULL; } \
} while(0)
#define IF(extensionname) if(al.IsExtensionPresent(extensionname))
    IF("AL_EXT_STATIC_BUFFER")
        {
        GET(BufferDataStatic);
        }
    IF("AL_SOFT_buffer_sub_data")
        {
        GET(BufferSubDataSOFT);
        }
    IF("AL_EXT_FOLDBACK")
        {
        GET(RequestFoldbackStart);
        GET(RequestFoldbackStop);
        }
    IF("AL_SOFT_buffer_samples")
        {
        GET(BufferSamplesSOFT);
        GET(BufferSubSamplesSOFT);
        GET(GetBufferSamplesSOFT);
        GET(IsBufferFormatSupportedSOFT);
        }
    IF("AL_SOFT_source_latency")
        {
        GET(SourcedSOFT);
        GET(Source3dSOFT);
        GET(SourcedvSOFT);
        GET(GetSourcedSOFT);
        GET(GetSource3dSOFT);
        GET(GetSourcedvSOFT);
        GET(Sourcei64SOFT);
        GET(Source3i64SOFT);
        GET(Sourcei64vSOFT);
        GET(GetSourcei64SOFT);
        GET(GetSource3i64SOFT);
        GET(GetSourcei64vSOFT);
        }
    IF("AL_SOFT_deferred_updates")
        {
        GET(DeferUpdatesSOFT);
        GET(ProcessUpdatesSOFT);
        }
    IF("AL_SOFT_source_resampler")
        {
        GET(GetStringiSOFT);
        }
#undef IF
#undef GET
    alc.MakeContextCurrent(old_context);
    return dt;
    }
 

void moonal_atexit_getproc(void)
    {
#if defined(LINUX)
    if(Handle) dlclose(Handle);
#elif defined(MINGW)
    if(Handle) FreeLibrary(Handle);
#endif
    }

/*---------------------------------------------------------------------------*/

static int GetExtensions(lua_State *L)
    {
    int i = 0;
    size_t len;
    char *p;
    const ALCchar *val;
    ud_t *ud;
    context_t old_context = current_context(L);
    context_t context = checkcontext(L, 1, &ud);
    device_t device = ud->device;
    make_context_current(L, context);

    lua_newtable(L);
    val = al.GetString(AL_EXTENSIONS);
    CheckErrorRestoreAl(L, old_context);

    p = firststringS(val, &len);
    while(p)
        {
        lua_pushlstring(L, p, len);
        lua_rawseti(L, -2, ++i);
        p = nextstringS(p, &len);
        }

    val = alc.GetString(device, ALC_EXTENSIONS);
    CheckErrorRestoreAl(L, old_context);
    p = firststringS(val, &len);
    while(p)
        {
        lua_pushlstring(L, p, len);
        lua_rawseti(L, -2, ++i);
        p = nextstringS(p, &len);
        }
    make_context_current(L, old_context);
    return 1;
    }

static int IsExtensionPresent(lua_State *L)
    {
    ALCboolean res;
    ud_t *ud;
    context_t old_context = current_context(L);
    context_t context = checkcontext(L, 1, &ud);
    const ALCchar *extname = luaL_checkstring(L, 2);
    device_t device = ud->device;
    make_context_current(L, context);

    if(strncmp(extname, "AL_", 3)==0)
        {
        res = al.IsExtensionPresent(extname);
        CheckErrorRestoreAl(L, old_context);
        }
    else if(strncmp(extname, "ALC_", 4)==0)
        {
        res = alc.IsExtensionPresent(device, extname);
        CheckErrorRestoreAlc(L, device, old_context);
        }
    else
        {
        make_context_current(L, old_context);
        return luaL_argerror(L, 2, "invalid extension name");
        }
    lua_pushboolean(L, res);
    make_context_current(L, old_context);
    return 1;
    }

static const struct luaL_Reg Functions[] = 
    {
        { "is_extension_present", IsExtensionPresent },
        { "get_extensions", GetExtensions },
        { NULL, NULL } /* sentinel */
    };


int moonal_open_getproc(lua_State *L)
    {
    Init(L);
    luaL_setfuncs(L, Functions, 0);
    return 0;
    }

