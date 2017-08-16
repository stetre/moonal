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

static lua_State *moonal_L = NULL;

static void AtExit(void)
    {
    if(moonal_L)
        {
        enums_free_all(moonal_L);
        moonal_atexit_getproc();
        moonal_L = NULL;
        }
    }

int luaopen_moonal(lua_State *L)
/* Lua calls this function to load the module */
    {
    moonal_L = L;

    moonal_utils_init(L);
    atexit(AtExit);

    lua_newtable(L); /* the cl table */

    /* add cl functions: */
    moonal_open_version(L);
    moonal_open_getproc(L);
    moonal_open_enums(L);
    moonal_open_tracing(L);
    moonal_open_device(L);
    moonal_open_context(L);
    moonal_open_listener(L);
    moonal_open_buffer(L);
    moonal_open_source(L);
    moonal_open_effect(L);
    moonal_open_filter(L);
    moonal_open_auxslot(L);
    moonal_open_datahandling(L);
    moonal_open_ranges(L);

#if 0 //@@
    /* Add functions implemented in Lua */
    lua_pushvalue(L, -1); lua_setglobal(L, "moonal");
    if(luaL_dostring(L, "require('moonal.utils')") != 0) lua_error(L);
    lua_pushnil(L);  lua_setglobal(L, "moonal");
#endif

    return 1;
    }

