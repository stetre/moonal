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

#ifndef getprocDEFINED
#define getprocDEFINED

#define TestDevicePfn(L, ud, pfn) ((ud)->ddt->pfn!=NULL)

#define CheckAlcPfn(L, pfn) \
    do { if(alc.pfn==NULL) return luaL_error((L),   \
            ""#pfn" address not loaded (extension not available)"); } while(0)

#define CheckDevicePfn(L, ud, pfn) \
    do { if((ud)->ddt->pfn==NULL) return luaL_error((L),    \
            ""#pfn" address not loaded (extension not available)"); } while(0)
#define CheckContextPfn(L, ud, pfn) \
    do { if((ud)->cdt->pfn==NULL) return luaL_error((L),    \
            ""#pfn" address not loaded (extension not available)"); } while(0)

/* Dispatch tables */

/* Global functions */
typedef struct {
    LPALENABLE Enable;
    LPALDISABLE Disable;
    LPALISENABLED IsEnabled;
    LPALGETSTRING GetString;
    LPALGETBOOLEANV GetBooleanv;
    LPALGETINTEGERV GetIntegerv;
    LPALGETFLOATV GetFloatv;
    LPALGETDOUBLEV GetDoublev;
    LPALGETBOOLEAN GetBoolean;
    LPALGETINTEGER GetInteger;
    LPALGETFLOAT GetFloat;
    LPALGETDOUBLE GetDouble;
    LPALGETERROR GetError;
    LPALISEXTENSIONPRESENT IsExtensionPresent;
    LPALGETPROCADDRESS GetProcAddress;
    LPALGETENUMVALUE GetEnumValue;
    LPALLISTENERF Listenerf;
    LPALLISTENER3F Listener3f;
    LPALLISTENERFV Listenerfv;
    LPALLISTENERI Listeneri;
    LPALLISTENER3I Listener3i;
    LPALLISTENERIV Listeneriv;
    LPALGETLISTENERF GetListenerf;
    LPALGETLISTENER3F GetListener3f;
    LPALGETLISTENERFV GetListenerfv;
    LPALGETLISTENERI GetListeneri;
    LPALGETLISTENER3I GetListener3i;
    LPALGETLISTENERIV GetListeneriv;
    LPALGENSOURCES GenSources;
    LPALDELETESOURCES DeleteSources;
    LPALISSOURCE IsSource;
    LPALSOURCEF Sourcef;
    LPALSOURCE3F Source3f;
    LPALSOURCEFV Sourcefv;
    LPALSOURCEI Sourcei;
    LPALSOURCE3I Source3i;
    LPALSOURCEIV Sourceiv;
    LPALGETSOURCEF GetSourcef;
    LPALGETSOURCE3F GetSource3f;
    LPALGETSOURCEFV GetSourcefv;
    LPALGETSOURCEI GetSourcei;
    LPALGETSOURCE3I GetSource3i;
    LPALGETSOURCEIV GetSourceiv;
    LPALSOURCEPLAYV SourcePlayv;
    LPALSOURCESTOPV SourceStopv;
    LPALSOURCEREWINDV SourceRewindv;
    LPALSOURCEPAUSEV SourcePausev;
    LPALSOURCEPLAY SourcePlay;
    LPALSOURCESTOP SourceStop;
    LPALSOURCEREWIND SourceRewind;
    LPALSOURCEPAUSE SourcePause;
    LPALSOURCEQUEUEBUFFERS SourceQueueBuffers;
    LPALSOURCEUNQUEUEBUFFERS SourceUnqueueBuffers;
    LPALGENBUFFERS GenBuffers;
    LPALDELETEBUFFERS DeleteBuffers;
    LPALISBUFFER IsBuffer;
    LPALBUFFERDATA BufferData;
    LPALBUFFERF Bufferf;
    LPALBUFFER3F Buffer3f;
    LPALBUFFERFV Bufferfv;
    LPALBUFFERI Bufferi;
    LPALBUFFER3I Buffer3i;
    LPALBUFFERIV Bufferiv;
    LPALGETBUFFERF GetBufferf;
    LPALGETBUFFER3F GetBuffer3f;
    LPALGETBUFFERFV GetBufferfv;
    LPALGETBUFFERI GetBufferi;
    LPALGETBUFFER3I GetBuffer3i;
    LPALGETBUFFERIV GetBufferiv;
    LPALDOPPLERFACTOR DopplerFactor;
    LPALDOPPLERVELOCITY DopplerVelocity;
    LPALSPEEDOFSOUND SpeedOfSound;
    LPALDISTANCEMODEL DistanceModel;
} moonal_al_dt_t;

typedef struct {
    LPALCCREATECONTEXT CreateContext;
    LPALCMAKECONTEXTCURRENT MakeContextCurrent;
    LPALCPROCESSCONTEXT ProcessContext;
    LPALCSUSPENDCONTEXT SuspendContext;
    LPALCDESTROYCONTEXT DestroyContext;
    LPALCGETCURRENTCONTEXT GetCurrentContext;
    LPALCGETCONTEXTSDEVICE GetContextsDevice;
    LPALCOPENDEVICE OpenDevice;
    LPALCCLOSEDEVICE CloseDevice;
    LPALCGETERROR GetError;
    LPALCISEXTENSIONPRESENT IsExtensionPresent;
    LPALCGETPROCADDRESS GetProcAddress;
    LPALCGETENUMVALUE GetEnumValue;
    LPALCGETSTRING GetString;
    LPALCGETINTEGERV GetIntegerv;
    LPALCCAPTUREOPENDEVICE CaptureOpenDevice;
    LPALCCAPTURECLOSEDEVICE CaptureCloseDevice;
    LPALCCAPTURESTART CaptureStart;
    LPALCCAPTURESTOP CaptureStop;
    LPALCCAPTURESAMPLES CaptureSamples;
    /* Optional : */
    LPALCLOOPBACKOPENDEVICESOFT LoopbackOpenDeviceSOFT;
    LPALCISRENDERFORMATSUPPORTEDSOFT IsRenderFormatSupportedSOFT;
    LPALCRENDERSAMPLESSOFT RenderSamplesSOFT;
} moonal_alc_dt_t;

/* Device functions (alc extensions) */
#define device_dt_t moonal_device_dt_t
typedef struct {
    PFNALCSETTHREADCONTEXTPROC SetThreadContext;
    PFNALCGETTHREADCONTEXTPROC GetThreadContext;
    LPALCDEVICEPAUSESOFT DevicePauseSOFT;
    LPALCDEVICERESUMESOFT DeviceResumeSOFT;
    LPALCGETSTRINGISOFT GetStringiSOFT;
    LPALCRESETDEVICESOFT ResetDeviceSOFT;
    LPALCGETINTEGER64VSOFT GetInteger64vSOFT;
    /* EFX */
    LPALGENEFFECTS GenEffects;
    LPALDELETEEFFECTS DeleteEffects;
    LPALISEFFECT IsEffect;
    LPALEFFECTI Effecti;
    LPALEFFECTIV Effectiv;
    LPALEFFECTF Effectf;
    LPALEFFECTFV Effectfv;
    LPALGETEFFECTI GetEffecti;
    LPALGETEFFECTIV GetEffectiv;
    LPALGETEFFECTF GetEffectf;
    LPALGETEFFECTFV GetEffectfv;
    LPALGENFILTERS GenFilters;
    LPALDELETEFILTERS DeleteFilters;
    LPALISFILTER IsFilter;
    LPALFILTERI Filteri;
    LPALFILTERIV Filteriv;
    LPALFILTERF Filterf;
    LPALFILTERFV Filterfv;
    LPALGETFILTERI GetFilteri;
    LPALGETFILTERIV GetFilteriv;
    LPALGETFILTERF GetFilterf;
    LPALGETFILTERFV GetFilterfv;
    LPALGENAUXILIARYEFFECTSLOTS GenAuxiliaryEffectSlots;
    LPALDELETEAUXILIARYEFFECTSLOTS DeleteAuxiliaryEffectSlots;
    LPALISAUXILIARYEFFECTSLOT IsAuxiliaryEffectSlot;
    LPALAUXILIARYEFFECTSLOTI AuxiliaryEffectSloti;
    LPALAUXILIARYEFFECTSLOTIV AuxiliaryEffectSlotiv;
    LPALAUXILIARYEFFECTSLOTF AuxiliaryEffectSlotf;
    LPALAUXILIARYEFFECTSLOTFV AuxiliaryEffectSlotfv;
    LPALGETAUXILIARYEFFECTSLOTI GetAuxiliaryEffectSloti;
    LPALGETAUXILIARYEFFECTSLOTIV GetAuxiliaryEffectSlotiv;
    LPALGETAUXILIARYEFFECTSLOTF GetAuxiliaryEffectSlotf;
    LPALGETAUXILIARYEFFECTSLOTFV GetAuxiliaryEffectSlotfv;
} device_dt_t;

/* Context functions (al extensions) */
#define context_dt_t moonal_context_dt_t
typedef struct {
    PFNALBUFFERDATASTATICPROC BufferDataStatic;
    PFNALBUFFERSUBDATASOFTPROC BufferSubDataSOFT;
    LPALREQUESTFOLDBACKSTART RequestFoldbackStart;
    LPALREQUESTFOLDBACKSTOP RequestFoldbackStop;
    LPALBUFFERSAMPLESSOFT BufferSamplesSOFT;
    LPALBUFFERSUBSAMPLESSOFT BufferSubSamplesSOFT;
    LPALGETBUFFERSAMPLESSOFT GetBufferSamplesSOFT;
    LPALISBUFFERFORMATSUPPORTEDSOFT IsBufferFormatSupportedSOFT;
    LPALSOURCEDSOFT SourcedSOFT;
    LPALSOURCE3DSOFT Source3dSOFT;
    LPALSOURCEDVSOFT SourcedvSOFT;
    LPALGETSOURCEDSOFT GetSourcedSOFT;
    LPALGETSOURCE3DSOFT GetSource3dSOFT;
    LPALGETSOURCEDVSOFT GetSourcedvSOFT;
    LPALSOURCEI64SOFT Sourcei64SOFT;
    LPALSOURCE3I64SOFT Source3i64SOFT;
    LPALSOURCEI64VSOFT Sourcei64vSOFT;
    LPALGETSOURCEI64SOFT GetSourcei64SOFT;
    LPALGETSOURCE3I64SOFT GetSource3i64SOFT;
    LPALGETSOURCEI64VSOFT GetSourcei64vSOFT;
    LPALDEFERUPDATESSOFT DeferUpdatesSOFT;
    LPALPROCESSUPDATESSOFT ProcessUpdatesSOFT;
    LPALGETSTRINGISOFT GetStringiSOFT;
} context_dt_t;

#undef F

#define al moonal_al
extern moonal_al_dt_t al;
#define alc moonal_alc
extern moonal_alc_dt_t alc;

#define getproc_device moonal_getproc_device
device_dt_t* getproc_device(lua_State *L, device_t device);
#define getproc_context moonal_getproc_context
context_dt_t* getproc_context(lua_State *L, context_t context);

#endif /* getprocDEFINED */

