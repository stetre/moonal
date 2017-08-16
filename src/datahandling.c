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
    
static size_t sizeoftype(int type)
    {
    switch(type)
        {
        case NONAL_TYPE_CHAR:   return sizeof(int8_t);
        case NONAL_TYPE_UCHAR:  return sizeof(uint8_t);
        case NONAL_TYPE_BYTE:   return sizeof(int8_t);
        case NONAL_TYPE_UBYTE:  return sizeof(uint8_t);
        case NONAL_TYPE_SHORT:  return sizeof(int16_t);
        case NONAL_TYPE_USHORT: return sizeof(uint16_t);
        case NONAL_TYPE_INT:    return sizeof(int32_t);
        case NONAL_TYPE_UINT:   return sizeof(uint32_t);
        case NONAL_TYPE_LONG:   return sizeof(int64_t);
        case NONAL_TYPE_ULONG: return sizeof(uint64_t);
        case NONAL_TYPE_FLOAT:  return sizeof(float);
        case NONAL_TYPE_DOUBLE: return sizeof(double);
        default:
            return 0;
        }
    return 0;
    }

static int Sizeof(lua_State *L)
/* size = sizeof(type) */
    {
    int type = checktype(L, 1);
    lua_pushinteger(L, sizeoftype(type));
    return 1;
    }

static int Flatten1_(lua_State *L, int table_index, int cur_index, int arg)
    {
    int len, i, top, m, n=0;

    if(lua_type(L, arg) != LUA_TTABLE)
        return luaL_error(L, "table expected");

    lua_len(L, arg);
    len = lua_tointeger(L, -1);
    lua_pop(L, 1);

    if(len==0) return n;

    for(i=1; i<=len; i++)
        {
        lua_geti(L, arg, i);
        top = lua_gettop(L);
        if(lua_type(L, top) == LUA_TTABLE)
            {
            m = Flatten1_(L, table_index, cur_index, top);
            n += m;
            cur_index += m;
            lua_remove(L, top);
            }
        else
            {
            n++;
            cur_index++;
            lua_rawseti(L, table_index, cur_index);
            }
        }
    return n;
    }

static int toflattable(lua_State *L, int arg)
/* Creates a flat table with all the arguments starting from arg, and leaves 
 * it on top of the stack.
 */
    {
    int table_index, last_arg, i, n;
    if(lua_type(L, arg) == LUA_TTABLE)
        {
        lua_newtable(L);
        n = Flatten1_(L, lua_gettop(L), 0, arg);
        }
    else
        {
        /* create a table with all the arguments, and flatten it */
        last_arg = lua_gettop(L);
        lua_newtable(L);
        table_index = lua_gettop(L);
        for(i=arg; i <= last_arg; i++)
            {
            lua_pushvalue(L, i);
            lua_rawseti(L, table_index, i-arg+1);
            }
        lua_newtable(L);
        n = Flatten1_(L, lua_gettop(L), 0, table_index);
        }
    return n;
    }

static int Flatten(lua_State *L)
    {
    int n, i, table_index;
    n = toflattable(L, 1);
    table_index = lua_gettop(L);
    luaL_checkstack(L, n, "too many elements, cannot grow Lua stack");
    for(i = 0; i < n; i++)
        lua_rawgeti(L, table_index, i+1);
    return n;
    }

static int FlattenTable(lua_State *L)
    {
    toflattable(L, 1);
    return 1;
    }

/*-----------------------------------------------------------------------------*/

#define PACK(T, what) /* what= number or integer */ \
static int Pack##T(lua_State *L, size_t n, void *dst, size_t dstsize, int *faulty_element)  \
    {                                       \
    int isnum;                              \
    size_t i;                               \
    T *data = (T*)dst;                      \
    if(faulty_element) *faulty_element = 0; \
    if(dstsize < (n * sizeof(T)))           \
        return ERR_LENGTH;                  \
    for(i = 0; i < n; i++)                  \
        {                                   \
        lua_rawgeti(L, -1, i+1);            \
        data[i] = lua_to##what##x(L, -1, &isnum); \
        if(!isnum)                          \
            {                               \
            if(faulty_element) *faulty_element = i+1;   \
            return ERR_TYPE; /* element i+1 is not a #what */ \
            }                               \
        lua_pop(L, 1);                      \
        }                                   \
    return 0;                               \
    }

#define PACK_NUMBERS(T)     PACK(T, number)
#define PACK_INTEGERS(T)    PACK(T, integer)

PACK_NUMBERS(float)
PACK_NUMBERS(double)
PACK_INTEGERS(int8_t)
PACK_INTEGERS(uint8_t)
PACK_INTEGERS(int16_t)
PACK_INTEGERS(uint16_t)
PACK_INTEGERS(int32_t)
PACK_INTEGERS(uint32_t)
PACK_INTEGERS(int64_t)
PACK_INTEGERS(uint64_t)

static int Pack(lua_State *L)
    {
    int err = 0;
    void *dst = NULL;
    size_t dstsize = 0;
    int type = checktype(L, 1);
    size_t n = toflattable(L, 2);
    switch(type)
        {
#define P(T) do { dstsize = n * sizeof(T);      \
                  dst = Malloc(L, dstsize);     \
                  err = Pack##T(L, n, dst, dstsize, NULL); } while(0)

        case NONAL_TYPE_CHAR:   P(int8_t); break;
        case NONAL_TYPE_UCHAR:  P(uint8_t); break;
        case NONAL_TYPE_BYTE:   P(int8_t); break;
        case NONAL_TYPE_UBYTE:  P(uint8_t); break;
        case NONAL_TYPE_SHORT:  P(int16_t); break;
        case NONAL_TYPE_USHORT: P(uint16_t); break;
        case NONAL_TYPE_INT:    P(int32_t); break;
        case NONAL_TYPE_UINT:   P(uint32_t); break;
        case NONAL_TYPE_LONG:   P(int64_t); break;
        case NONAL_TYPE_ULONG:  P(uint64_t); break;
        case NONAL_TYPE_FLOAT:  P(float); break;
        case NONAL_TYPE_DOUBLE: P(double); break;
        default:
            return unexpected(L);
#undef P
        }
    if(err)
        {
        Free(L, dst);
        return luaL_argerror(L, 2, errstring(err));
        }
    lua_pushlstring(L, (char*)dst, dstsize);
    Free(L, dst);
    return 1;
    }

/*-----------------------------------------------------------------------------*/

#define UNPACK(T, what) /* what= number or integer */   \
static int Unpack##T(lua_State *L, const void* data, size_t len) \
    {                                                   \
    size_t n;                                           \
    size_t i=0;                                         \
    if((len < sizeof(T)) || (len % sizeof(T)) != 0)     \
        return ERR_LENGTH;                              \
    n = len / sizeof(T);                                \
    lua_newtable(L);                                    \
    for(i = 0; i < n; i++)                              \
        {                                               \
        lua_push##what(L, ((T*)data)[i]);               \
        lua_rawseti(L, -2, i+1);                        \
        }                                               \
    return 0;                                           \
    }

#define UNPACK_NUMBERS(T)   UNPACK(T, number)
#define UNPACK_INTEGERS(T)  UNPACK(T, integer)


UNPACK_NUMBERS(float)
UNPACK_NUMBERS(double)
UNPACK_INTEGERS(int8_t)
UNPACK_INTEGERS(uint8_t)
UNPACK_INTEGERS(int16_t)
UNPACK_INTEGERS(uint16_t)
UNPACK_INTEGERS(int32_t)
UNPACK_INTEGERS(uint32_t)
UNPACK_INTEGERS(int64_t)
UNPACK_INTEGERS(uint64_t)

static int Unpack_(lua_State *L, int type, const void *data, size_t len)
    {
    int err = 0;
    switch(type)
        {
        case NONAL_TYPE_CHAR:   err = Unpackint8_t(L, data, len); break;
        case NONAL_TYPE_UCHAR:  err = Unpackuint8_t(L, data, len); break;
        case NONAL_TYPE_BYTE:   err = Unpackint8_t(L, data, len); break;
        case NONAL_TYPE_UBYTE:  err = Unpackuint8_t(L, data, len); break;
        case NONAL_TYPE_SHORT:  err = Unpackint16_t(L, data, len); break;
        case NONAL_TYPE_USHORT: err = Unpackuint16_t(L, data, len); break;
        case NONAL_TYPE_INT:    err = Unpackint32_t(L, data, len); break;
        case NONAL_TYPE_UINT:   err = Unpackuint32_t(L, data, len); break;
        case NONAL_TYPE_LONG:   err = Unpackint64_t(L, data, len); break;
        case NONAL_TYPE_ULONG:  err = Unpackuint64_t(L, data, len); break;
        case NONAL_TYPE_FLOAT:  err = Unpackfloat(L, data, len); break;
        case NONAL_TYPE_DOUBLE: err = Unpackdouble(L, data, len); break;
        default:
            return unexpected(L);
        }
    if(err)
        return luaL_error(L, errstring(err));
    return 1;
    }


static int Unpack(lua_State *L)
    {
    size_t len;
    int type = checktype(L, 1);
    const void *data = luaL_checklstring(L, 2, &len);
    return Unpack_(L, type, data, len);
    }

static int FormatSize(lua_State *L)
    {
    ALenum fmt = checkformat(L, 1);
    ALsizei align =luaL_optinteger(L, 2, 0);
    size_t framesize = formatframesize(L, fmt, align);
    size_t channels = formatchannels(L, fmt);
    lua_pushinteger(L, framesize);
    lua_pushinteger(L, channels);
    return 2;
    }

static const struct luaL_Reg Functions[] = 
    {
        { "flatten", Flatten },
        { "flatten_table", FlattenTable },
        { "sizeof", Sizeof },
        { "pack", Pack },
        { "unpack", Unpack },
        { "formatsize", FormatSize },
        { NULL, NULL } /* sentinel */
    };

void moonal_open_datahandling(lua_State *L)
    {
    luaL_setfuncs(L, Functions, 0);
    }


