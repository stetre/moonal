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

context_t current_context(lua_State *L)
    {
    context_t context = alc.GetCurrentContext();
    if(!context) { luaL_error(L, "cannot get current context"); return NULL; }  
    return context;
    }

int make_context_current(lua_State *L, context_t context)
    {
    (void)L;
    alc.MakeContextCurrent(context);
    CheckErrorAlc(L, userdata(context)->device);
    return 0;
    }

device_t current_device(lua_State *L)
    {
    context_t context = current_context(L);
    return UD(context)->device; //alc.GetContextsDevice(context);
    }

static int freecontext(lua_State *L, ud_t *ud)
    {
    context_t context = (context_t)ud->handle;
    device_t device = ud->device;
    freechildren(L, AUXSLOT_MT, ud);
    freechildren(L, FILTER_MT, ud);
    freechildren(L, EFFECT_MT, ud);
    freechildren(L, SOURCE_MT, ud);
    freechildren(L, BUFFER_MT, ud);
    freechildren(L, LISTENER_MT, ud);
    if(!freeuserdata(L, ud)) return 0;
    TRACE_DELETE(context, "context");
    alc.DestroyContext(context);
    CheckErrorAlc(L, device);
    return 0;
    }

static int CreateContext(lua_State *L)
    {
    int err;
    ud_t *ud, *device_ud;
    context_t context;
    device_t device = checkdevice(L, 1, &device_ud);
    ALCint *attrlist = echeckdeviceattributeslist(L, 2, &err);
    if(err<0) return luaL_argerror(L, 2, lua_tostring(L, -1));
    if(err==ERR_NOTPRESENT) lua_pop(L, 1);

    context = alc.CreateContext(device, attrlist);
    if(attrlist) Free(L, attrlist);
    CheckErrorAlc(L, device);
    alc.MakeContextCurrent(context);
    CheckErrorAlc(L, device);
    ud = newuserdata(L, context, CONTEXT_MT);
    ud->device = device;
    ud->context = context;
    ud->parent_ud = device_ud;
    ud->destructor = freecontext;
    ud->ddt = device_ud->ddt;
    ud->cdt = getproc_context(L, context);
    TRACE_CREATE(context, "context");
    return 1;
    }

static int ResetDevice(lua_State *L)
    {
    int err;
    ALCboolean res;
    ud_t *ud;
    ALCint *attrlist;
    (void)checkcontext(L, 1, &ud);

    CheckDevicePfn(L, ud, ResetDeviceSOFT);
    attrlist = echeckdeviceattributeslist(L, 2, &err);
    if(err<0) return luaL_argerror(L, 2, lua_tostring(L, -1));
    if(err==ERR_NOTPRESENT) lua_pop(L, 1);
    res = ud->ddt->ResetDeviceSOFT(ud->device, attrlist);
    if(attrlist) Free(L, attrlist);
    lua_pushboolean(L, res);
    return 1;
    }


static int CurrentContext(lua_State *L)
    {
    ud_t *ud;
    ALCboolean res;
    context_t context;
    if(lua_isnone(L, 1)) /* get */
        {
        context = alc.GetCurrentContext();
        pushcontext(L, context);
        return 1;
        }
    context = checkcontext(L, 1, &ud);
    res = alc.MakeContextCurrent(context);
    CheckErrorAlc(L, ud->device);
    lua_pushboolean(L, res);
    return 1;
    }

static int ProcessContext(lua_State *L)
    {
    ud_t *ud;
    context_t context = checkcontext(L, 1, &ud);
    alc.ProcessContext(context);
    CheckErrorAlc(L, ud->device);
    return 0;
    }

static int SuspendContext(lua_State *L)
    {
    ud_t *ud;
    context_t context = checkcontext(L, 1, &ud);
    alc.SuspendContext(context);
    CheckErrorAlc(L, ud->device);
    return 0;
    }

static int GetContextsDevice(lua_State *L)
    {
    ud_t *ud;
    context_t context = checkcontext(L, 1, &ud);
    device_t device = alc.GetContextsDevice(context);
    CheckErrorAlc(L, ud->device);
    pushdevice(L, device);
    return 1;
    }


static int Enable(lua_State *L)
    {
    ud_t *ud;
    context_t old_context = current_context(L);
    context_t context = checkcontext(L, 1, &ud);
    ALenum capability = checkcapability(L, 2);
    make_context_current(L, context);
    al.Enable(capability);
    CheckErrorRestoreAlc(L, ud->device, old_context);
    make_context_current(L, old_context);
    return 0;
    }

static int Disable(lua_State *L)
    {
    ud_t *ud;
    context_t old_context = current_context(L);
    context_t context = checkcontext(L, 1, &ud);
    ALenum capability = checkcapability(L, 2);
    make_context_current(L, context);
    al.Disable(capability);
    CheckErrorRestoreAlc(L, ud->device, old_context);
    make_context_current(L, old_context);
    return 0;
    }


static int IsEnabled(lua_State *L)
    {
    ALboolean res;
    ud_t *ud;
    context_t old_context = current_context(L);
    context_t context = checkcontext(L, 1, &ud);
    ALenum capability = checkcapability(L, 2);
    make_context_current(L, context);
    res = al.IsEnabled(capability);
    al.Disable(capability);
    CheckErrorRestoreAlc(L, ud->device, old_context);
    make_context_current(L, old_context);
    lua_pushboolean(L, res);
    return 1;
    }


/*---- get/set --------------------------------------------------------------*/

static int GetBoolean(lua_State *L, ALenum param)
    {
    ALboolean val;
    val = al.GetBoolean(param);
    lua_pushboolean(L, val);
    return 1;
    }

static int GetFloat(lua_State *L, ALenum param)
    {
    ALfloat val;
    val = al.GetFloat(param);
    lua_pushnumber(L, val);
    return 1;
    }
    

#define GetDopplerFactor(L) GetFloat((L), AL_DOPPLER_FACTOR)
static int SetDopplerFactor(lua_State *L)
    {
    ALfloat val;
    val = luaL_checknumber(L, 3);
    al.DopplerFactor(val);
    return 0;
    }

#define GetDopplerVelocity(L) GetFloat((L), AL_DOPPLER_VELOCITY)
static int SetDopplerVelocity(lua_State *L)
    {
    ALfloat val;
    val = luaL_checknumber(L, 3);
    al.DopplerVelocity(val);
    return 0;
    }

#define GetSpeedOfSound(L) GetFloat((L), AL_SPEED_OF_SOUND)
static int SetSpeedOfSound(lua_State *L)
    {
    ALfloat val;
    val = luaL_checknumber(L, 3);
    al.SpeedOfSound(val);
    return 0;
    }

static int GetDistanceModel(lua_State *L)
    {
    ALenum val = al.GetInteger(AL_DISTANCE_MODEL);
    pushdistancemodel(L, val);
    return 1;
    }

static int SetDistanceModel(lua_State *L)
    {
    ALenum val = checkdistancemodel(L, 3);
    al.DistanceModel(val);
    return 0;
    }

#define GetGainLimit(L) GetFloat((L), AL_GAIN_LIMIT_SOFT)
#define GetDeferredUpdates(L) GetBoolean((L), AL_DEFERRED_UPDATES_SOFT)

static int GetDefaultResampler(lua_State *L)
    {
    ALenum val = al.GetInteger(AL_DEFAULT_RESAMPLER_SOFT);
    pushresampler(L, val);
    return 1;
    }

static int GetResamplerNames(lua_State *L) /* same as al.enum('resampler') */
    {
#if 0 /* not used: */
    n = al.Getinteger(AL_NUM_RESAMPLERS_SOFT)
    for(i = 0; i <n; +++)
        name[i] = al.GetStringiSOFT(NULL, AL_RESAMPLER_NAME_SOFT, i)
#endif
    return valuesresampler(L);
    }

static int GetContext(lua_State *L)
    {
    int res;
    context_t old_context = current_context(L);
    context_t context = checkcontext(L, 1, NULL);
    ALenum param = checkalparam(L, 2);
    make_context_current(L, context);
    switch(param)
        {
        case AL_DOPPLER_FACTOR: res = GetDopplerFactor(L); break;
        case AL_DOPPLER_VELOCITY:   res = GetDopplerVelocity(L); break;
        case AL_SPEED_OF_SOUND: res = GetSpeedOfSound(L); break;
        case AL_DISTANCE_MODEL: res = GetDistanceModel(L); break;
        case AL_GAIN_LIMIT_SOFT:    res = GetGainLimit(L); break;
        case AL_DEFERRED_UPDATES_SOFT:  res = GetDeferredUpdates(L); break;
        case AL_DEFAULT_RESAMPLER_SOFT: res = GetDefaultResampler(L); break;
        case AL_RESAMPLER_NAME_SOFT:    res = GetResamplerNames(L); break;
        default:
            make_context_current(L, old_context);
            return erralparam(L, 2);
        }
    CheckErrorAl(L);
    make_context_current(L, old_context);
    return res;
    }

static int SetContext(lua_State *L)
    {
    int res;
    context_t old_context = current_context(L);
    context_t context = checkcontext(L, 1, NULL);
    ALenum param = checkalparam(L, 2);
    make_context_current(L, context);
    switch(param)
        {
        case AL_DOPPLER_FACTOR: res = SetDopplerFactor(L); break;
        case AL_DOPPLER_VELOCITY:   res = SetDopplerVelocity(L); break;
        case AL_SPEED_OF_SOUND: res = SetSpeedOfSound(L); break;
        case AL_DISTANCE_MODEL: res = SetDistanceModel(L); break;
        case AL_GAIN_LIMIT_SOFT:
        case AL_DEFERRED_UPDATES_SOFT:
        case AL_DEFAULT_RESAMPLER_SOFT:
        case AL_RESAMPLER_NAME_SOFT:
                            make_context_current(L, old_context);
                            return errreadonlyparam(L, 2);
        default: 
            make_context_current(L, old_context);
            return erralparam(L, 2);
        }
    make_context_current(L, old_context);
    return res;
    }

static int Listener(lua_State *L)
    {
    context_t context = checkcontext(L, 1, NULL);
    return pushlistener(L, context);
    }

/*--- get attributes --------------------------------------------------------*/

static int AlcGetBoolean(lua_State *L, device_t device, ALCenum param)
    {
    ALint val;
    alc.GetIntegerv(device, param, 1, &val);
    lua_pushboolean(L, val);
    return 1;
    }

static int AlcGetInteger(lua_State *L, device_t device, ALCenum param)
    {
    ALint val;
    alc.GetIntegerv(device, param, 1, &val);
    lua_pushinteger(L, val);
    return 1;
    }

static int AlcGetString(lua_State *L, device_t device, ALCenum param)
    {
    const ALchar *val = alc.GetString(device, param);
    if(!val || val[0]=='\0') return 0;
    lua_pushstring(L, val);
    return 1;
    }


#define GET_ENUM_FUNC(FuncName, param, enumtype)        \
static int FuncName(lua_State *L, device_t device)      \
    {                                                   \
    ALCenum val;                                        \
    alc.GetIntegerv(device, param, 1, &val);            \
    push##enumtype(L, val);                             \
    return 1;                                           \
    }

GET_ENUM_FUNC(AlcGetFormatType, ALC_FORMAT_TYPE_SOFT, typesoft)
GET_ENUM_FUNC(AlcGetFormatChannels, ALC_FORMAT_CHANNELS_SOFT, channels)
GET_ENUM_FUNC(AlcGetHrtfStatus, ALC_HRTF_STATUS_SOFT, hrtfstatus)
#undef GET_ENUM_FUNC

static int GetAttribute(lua_State *L)
    {
    int res;
    ud_t *ud;
    context_t old_context = current_context(L);
    context_t context = checkcontext(L, 1, &ud);
    ALCenum param = checkalcparam(L, 2);
    device_t device = ud->device;
    make_context_current(L, context);
    switch(param)
        {
        case ALC_SYNC:
        case ALC_HRTF_SOFT:
        case ALC_OUTPUT_LIMITER_SOFT: res = AlcGetBoolean(L, device, param); break;
        case ALC_FREQUENCY:
        case ALC_REFRESH:
        case ALC_MONO_SOURCES:
        case ALC_STEREO_SOURCES:
        case ALC_MAX_AUXILIARY_SENDS:
        case ALC_HRTF_ID_SOFT: res = AlcGetInteger(L, device, param); break;
        case ALC_HRTF_SPECIFIER_SOFT:
                res = AlcGetString(L, device, param); break;
        case ALC_HRTF_STATUS_SOFT: res = AlcGetHrtfStatus(L, device); break;
        case ALC_FORMAT_TYPE_SOFT: res = AlcGetFormatType(L, device); break;
        case ALC_FORMAT_CHANNELS_SOFT:  res = AlcGetFormatChannels(L, device); break;
        default: 
            make_context_current(L, old_context);
            return erralcparam(L, 2);
        }
    make_context_current(L, old_context);
    return res;
    }

#if 0
static int GetAllAttributes(lua_State *L)
    {
    ud_t *ud;
    ALCint nattr;
    ALCint *values;
    context_t old_context = current_context(L);
    context_t context = checkcontext(L, 1, &ud);
    device_t device = ud->device;
    make_context_current(L, context);
    alc.GetIntegerv(device, ALC_ATTRIBUTES_SIZE, 1, &nattr);
    CheckErrorRestoreAlc(L, device, old_context);
    values = (ALCint*)Malloc(L, nattr*sizeof(ALCint));
    alc.GetIntegerv(device, ALC_ALL_ATTRIBUTES, nattr, values);
    CheckErrorRestoreAlc(L, device, old_context);
    make_context_current(L, old_context);
    pushdeviceattributeslist(L, values);
    Free(L, values);
    return 1;
    }
#endif

//@@TODO? ALC_EXT_thread_local_context
//ALCboolean  alcSetThreadContext(ALCcontext *context);
//ALCcontext* alcGetThreadContext(void);

static int DeferUpdates(lua_State *L)
    {
    ud_t *ud;
    context_t old_context = current_context(L);
    context_t context = checkcontext(L, 1, &ud);
    CheckContextPfn(L, ud, DeferUpdatesSOFT);
    make_context_current(L, context);
    ud->cdt->DeferUpdatesSOFT();
    make_context_current(L, old_context);
    return 0;
    }


static int ProcessUpdates(lua_State *L)
    {
    ud_t *ud;
    context_t old_context = current_context(L);
    context_t context = checkcontext(L, 1, &ud);
    CheckContextPfn(L, ud, ProcessUpdatesSOFT);
    make_context_current(L, context);
    ud->cdt->ProcessUpdatesSOFT();
    make_context_current(L, old_context);
    return 0;
    }



RAW_FUNC(context)
TYPE_FUNC(context)
PARENT_FUNC(context)
DELETE_FUNC(context)

static const struct luaL_Reg Methods[] = 
    {
        { "raw", Raw },
        { "type", Type },
        { "parent", Parent },
        { "delete", Delete },
        { "reset", ResetDevice },
        { "process", ProcessContext },
        { "suspend", SuspendContext },
        { "device", GetContextsDevice },
        { "enable", Enable },
        { "disable", Disable },
        { "is_enabled", IsEnabled },
        { "get", GetContext },
        { "set", SetContext },
        { "listener", Listener },
        { "get_attribute", GetAttribute },
        { "defer_updates", DeferUpdates },
        { "process_updates", ProcessUpdates },
        { NULL, NULL } /* sentinel */
    };

static const struct luaL_Reg MetaMethods[] = 
    {
        { "__gc",  Delete },
        { NULL, NULL } /* sentinel */
    };

static const struct luaL_Reg Functions[] = 
    {
        { "create_context", CreateContext },
        { "reset_context", ResetDevice },
        { "delete_context", Delete },
        { "current_context", CurrentContext },
        { "process_context", ProcessContext },
        { "suspend_context", SuspendContext },
        { "context_device", GetContextsDevice },
        { "enable", Enable },
        { "disable", Disable },
        { "is_enabled", IsEnabled },
        { "context_get", GetContext },
        { "context_set", SetContext },
        { "context_listener", Listener },
        { "get_attribute", GetAttribute },
//      { "get_all_attributes", GetAllAttributes }, --> use get_attribute()
        { "defer_updates", DeferUpdates },
        { "process_updates", ProcessUpdates },
        { NULL, NULL } /* sentinel */
    };


void moonal_open_context(lua_State *L)
    {
    udata_define(L, CONTEXT_MT, Methods, MetaMethods);
    luaL_setfuncs(L, Functions, 0);
    }

