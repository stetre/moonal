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

static int freeauxslot(lua_State *L, ud_t *ud)
    {
    auxslot_t auxslot = (auxslot_t)ud->handle;
    LPALDELETEAUXILIARYEFFECTSLOTS DeleteAuxiliaryEffectSlots = ud->ddt->DeleteAuxiliaryEffectSlots;
    if(!freeuserdata(L, ud)) return 0;
    TRACE_DELETE(auxslot, "auxslot");
    DeleteAuxiliaryEffectSlots(1, &auxslot->name);
    Free(L, auxslot);
    CheckErrorAl(L); 
    return 0;
    }


static int Create(lua_State *L)
    {
    ALuint name;
    auxslot_t auxslot;
    ud_t *ud, *context_ud;
    context_t old_context = current_context(L);
    context_t context = checkcontext(L, 1, &context_ud);
    
    CheckDevicePfn(L, context_ud, GenAuxiliaryEffectSlots);
    context_ud->ddt->GenAuxiliaryEffectSlots(1, &name);
    CheckErrorRestoreAl(L, old_context);

    auxslot = (object_t*)MallocNoErr(L, sizeof(object_t));
    if(!auxslot)
        {
        make_context_current(L, old_context);
        context_ud->ddt->DeleteAuxiliaryEffectSlots(1, &name);
        return luaL_error(L, errstring(ERR_MEMORY));
        }
    
    auxslot->name = name;
    ud = newuserdata(L, auxslot, AUXSLOT_MT);
    ud->context = context;
    ud->device = context_ud->device;
    ud->parent_ud = context_ud;
    ud->destructor = freeauxslot;
    ud->ddt = context_ud->ddt;
    ud->cdt = context_ud->cdt;
    TRACE_CREATE(auxslot, "auxslot");
    make_context_current(L, old_context);
    return 1;
    }

#define GET_OBJECT_FUNC(FuncName, param, what)          \
static int FuncName(lua_State *L, auxslot_t auxslot)    \
    {                                                   \
    ALint name;                                         \
    what##_t obj;                                       \
    ud_t *ud = userdata(auxslot);                       \
    CheckDevicePfn(L, ud, GetAuxiliaryEffectSloti);     \
    ud->ddt->GetAuxiliaryEffectSloti(auxslot->name, param, &name);\
    CheckErrorAl(L);                                    \
    obj = search##what(L, name, NULL);                  \
    if(!obj) return 0;                                  \
    push##what(L, obj);                                 \
    return 1;                                           \
    }                                                   \

GET_OBJECT_FUNC(GetEffect, AL_EFFECTSLOT_EFFECT, effect)
#undef GET_OBJECT_FUNC


#define SET_OBJECT_FUNC(FuncName, param, what)          \
static int FuncName(lua_State *L, auxslot_t auxslot)    \
    {                                                   \
    ALint name;                                         \
    what##_t obj;                                       \
    ud_t *ud = userdata(auxslot);                       \
    CheckDevicePfn(L, ud, AuxiliaryEffectSloti);        \
    obj = test##what(L, 3, NULL);                       \
    name = obj ? obj->name : 0;                         \
    ud->ddt->AuxiliaryEffectSloti(auxslot->name, param, name);\
    CheckErrorAl(L);                                    \
    return 0;                                           \
    }

SET_OBJECT_FUNC(SetEffect, AL_EFFECTSLOT_EFFECT, effect)
#undef SET_OBJECT_FUNC




static int GetBoolean(lua_State *L, auxslot_t auxslot, ALenum param)
    {
    ALint val;
    ud_t *ud = userdata(auxslot);
    CheckDevicePfn(L, ud, AuxiliaryEffectSloti);
    ud->ddt->GetAuxiliaryEffectSloti(auxslot->name, param, &val);
    CheckErrorAl(L);
    lua_pushboolean(L, val);
    return 1;
    }

static int SetBoolean(lua_State *L, auxslot_t auxslot, ALenum param)
    {
    ALint val;
    ud_t *ud = userdata(auxslot);
    val = checkboolean(L, 3);
    CheckDevicePfn(L, ud, AuxiliaryEffectSloti);
    ud->ddt->AuxiliaryEffectSloti(auxslot->name, param, val);
    CheckErrorAl(L);
    return 0;
    }

static int GetFloat(lua_State *L, auxslot_t auxslot, ALenum param)
    {
    ALfloat val;
    ud_t *ud = userdata(auxslot);
    CheckDevicePfn(L, ud, AuxiliaryEffectSlotf);
    ud->ddt->GetAuxiliaryEffectSlotf(auxslot->name, param, &val);
    CheckErrorAl(L);
    lua_pushnumber(L, val);
    return 1;
    }

static int SetFloat(lua_State *L, auxslot_t auxslot, ALenum param)
    {
    ALfloat val;
    ud_t *ud = userdata(auxslot);
    val = luaL_checknumber(L, 3);
    CheckDevicePfn(L, ud, AuxiliaryEffectSlotf);
    ud->ddt->AuxiliaryEffectSlotf(auxslot->name, param, val);
    CheckErrorAl(L);
    return 0;
    }

static int GetAuxslot(lua_State *L)
    {
    auxslot_t auxslot = checkauxslot(L, 1, NULL);
    ALenum param = checkalparam(L, 2);
    switch(param)
        {
        case AL_EFFECTSLOT_GAIN:
                            return GetFloat(L, auxslot, param);
        case AL_EFFECTSLOT_AUXILIARY_SEND_AUTO:
                            return GetBoolean(L, auxslot, param);
        case AL_EFFECTSLOT_EFFECT: return GetEffect(L, auxslot);
        default:
            return erralparam(L, 2);
        }
    return 0;
    }

static int SetAuxslot(lua_State *L)
    {
    auxslot_t auxslot = checkauxslot(L, 1, NULL);
    ALenum param = checkalparam(L, 2);
    switch(param)
        {
        case AL_EFFECTSLOT_GAIN:
                            return SetFloat(L, auxslot, param);
        case AL_EFFECTSLOT_AUXILIARY_SEND_AUTO:
                            return SetBoolean(L, auxslot, param);
        case AL_EFFECTSLOT_EFFECT: return SetEffect(L, auxslot);
        default:
            return erralparam(L, 2);
        }
    return 0;
    }

RAW_FUNC(auxslot)
TYPE_FUNC(auxslot)
PARENT_FUNC(auxslot)
DELETE_FUNC(auxslot)

static const struct luaL_Reg Methods[] = 
    {
        { "raw", Raw },
        { "type", Type },
        { "parent", Parent },
        { "delete", Delete },
        { "get", GetAuxslot },
        { "set", SetAuxslot },
        { NULL, NULL } /* sentinel */
    };

static const struct luaL_Reg MetaMethods[] = 
    {
        { "__gc",  Delete },
        { NULL, NULL } /* sentinel */
    };

static const struct luaL_Reg Functions[] = 
    {
        { "create_auxslot", Create},
        { "delete_auxslot", Delete },
        { "auxslot_get", GetAuxslot },
        { "auxslot_set", SetAuxslot },
        { NULL, NULL } /* sentinel */
    };


void moonal_open_auxslot(lua_State *L)
    {
    udata_define(L, AUXSLOT_MT, Methods, MetaMethods);
    luaL_setfuncs(L, Functions, 0);
    }

