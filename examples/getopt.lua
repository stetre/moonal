-- getopt.lua
--
-- This code is a slight modification of lua-alt-getopt 
-- (http://sourceforge.net/projects/lua-alt-getopt/ )
-- The copyright notice of the original version is included below.
--
-- Stefano Trettel
--
------------------------------------------------------------------------
-- Original version copyright notice
-- Copyright (c) 2009 Aleksey Cheusov <vle@gmx.net>
--
-- Permission is hereby granted, free of charge, to any person obtaining
-- a copy of this software and associated documentation files (the
-- "Software"), to deal in the Software without restriction, including
-- without limitation the rights to use, copy, modify, merge, publish,
-- distribute, sublicense, and/or sell copies of the Software, and to
-- permit persons to whom the Software is furnished to do so, subject to
-- the following conditions:
--
-- The above copyright notice and this permission notice shall be
-- included in all copies or substantial portions of the Software.
--
-- THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
-- EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
-- MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
-- NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE
-- LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION
-- OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION
-- WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

local type, pairs, ipairs, io, os = type, pairs, ipairs, io, os

-- module ("alt_getopt") @@

local function convert_short2long (opts)
   local i = 1
   local len = #opts
   local ret = {}

   for short_opt, accept_arg in opts:gmatch("(%w)(:?)") do
      ret[short_opt]=#accept_arg
   end

   return ret
end

local opterr = nil

local function exit_with_error(msg, exit_status)
   opterr = msg
   return nil
--[[
   io.stderr:write (msg)
   os.exit (exit_status)
--]]
end

local function err_unknown_opt(opt)
   opterr = "Unknown option `-" .. (#opt > 1 and "-" or "") .. opt .. "'"
end

local function canonize(options, opt)
   if not options [opt] then
      err_unknown_opt(opt)
      return opt
   end

   while type (options [opt]) == "string" do
      opt = options [opt]
      if not options [opt] then
         err_unknown_opt(opt)
         return opt
      end
   end
   return opt
end

local function get_ordered_opts(arg, sh_opts, long_opts)
   local i      = 1
   local count  = 1
   local opts   = {}
   local optarg = {}

   local options = convert_short2long (sh_opts)
   for k,v in pairs (long_opts) do
      options [k] = v
   end

   while i <= #arg do
      local a = arg [i]

      if a == "--" then i = i + 1 break
      elseif a == "-" then break
      elseif a:sub (1, 2) == "--" then
         local pos = a:find ("=", 1, true)
         if pos then
            local opt = a:sub (3, pos-1)
            opt = canonize(options, opt)
            if not opt or opterr then return nil end
            if options [opt] == 0 then
               return exit_with_error("Bad usage of option `" .. a .. "'", 1)
            end

       optarg [count] = a:sub (pos+1)
       opts [count] = opt
    else
       local opt = a:sub (3)

       opt = canonize (options, opt)
      if not opt or opterr then return nil end

       if options [opt] == 0 then
          opts [count] = opt
       else
          if i == #arg then
        return exit_with_error ("Missed value for option `" .. a .. "'", 1)
          end

          optarg [count] = arg [i+1]
          opts [count] = opt
          i = i + 1
       end
    end
    count = count + 1

      elseif a:sub (1, 1) == "-" then
    local j
    for j=2,a:len () do
       local opt = canonize (options, a:sub (j, j))
      if not opt or opterr then return nil end

       if options [opt] == 0 then
          opts [count] = opt
          count = count + 1
       elseif a:len () == j then
          if i == #arg then
        return exit_with_error ("Missed value for option `-" .. opt .. "'", 1)
          end

          optarg [count] = arg [i+1]
          opts [count] = opt
          i = i + 1
          count = count + 1
          break
       else
          optarg [count] = a:sub (j+1)
          opts [count] = opt
          count = count + 1
          break
       end
    end
      else
    break
      end

      i = i + 1
   end

   return opts,i,optarg
end


local function get_opts(arg, sh_opts, long_opts)
   local ret = {}

   local opts,optind,optarg = get_ordered_opts(arg, sh_opts, long_opts)

   if not opts then return nil, nil, opterr end

   for i,v in ipairs(opts) do
      if optarg [i] then
          ret[v] = optarg [i]
      else
         ret[v] = true --@@
      end
   end
   return ret, optind, nil
end

return get_opts --@@

--[==[
-------------------------------------------------------------------------------
-- USAGE
-------------------------------------------------------------------------------

-- The module is to be loaded in the usual way, with require(), e.g.:
--
-- getopt = require("getopt") 
-- 
-- This returns a getopt() function that parses an arg table (usually the standard
-- Lua arg table) according to the passed parameters, which specify the options:
--
-- SYNOPSIS 
--    optarg, optind, opterr = getopt(arg, short_opts, long_opts)
--
-- ARGUMENTS
-- arg   
--       A table of command line arguments. 
--       getopt() considers arg[1], arg[2], ... up to the first nil element.
--
-- short_opts
--       A string specifying the short flags for the options.
--       Each flag is one character, optionally followed by a colon (:) to 
--       indicate that the flag must be followed by a value.
--
--       e.g. short_opts = "ht:vno:" specifies the following options:
--             -h, -t value, -v, -n, -o value
--
-- long_opts 
--       A table whose elements are the above short flags, indexed by strings
--       that denote the corresponding long versions of the options flags.
--       e.g. long_opts = {
--               "help" = "h", --  --help can be used instead of -h
--               "trim" = "t", --  --trim can be used instead of -t
--               "output" = -o, 
--               ...
--            }
--
--  RETURN VALUES
--  optarg
--       A table of option values, indexed with the short flags.
--       The value for an option is nil if the option was not present in the 
--       command line, otherwise it is the value associated with the option
--       (if it was specified with ":"), or the boolean true value.
--
--       For example, assuming the "x" option (long "xxxx"):
--       option not present -----> optarg.x = nil     
--       -x value           -----> optarg.x = value   
--       -xxx value         ----->    "       "
--       -x                 -----> optarg.x = true    
--       -xxx               ----->    "       "
--
--  optind
--       The index of the first nil element in the arg table, i.e. the index
--       of the first element after the option flags and any option value.
--       Non-option arguments, if any, start from here.
--
--       e.g. mycmd -t 123 -v myfile.txt
--                   |  |   |    |
--        arg[i]  i= 1  2   3    4=optind
--
--  opterr
--       Error message. This is nil if the command line was parsed successfully,
--       otherwise it is an error message (a string).
--           

-------------------------------------------------------------------------------
-- EXAMPLE
-------------------------------------------------------------------------------
getopt = require("getopt") 

short_opts = "hVvo:n:S:"
long_opts = { verbose="v", help="h", output="o", set_value="S", ["set-output"] = "o" }

progname = arg[0]
usage_message = "Usage: lua "..progname..[[ [options] ARG1 [ ARG2 ... ]

Options:
  -h, --help
  -v, --verbose
  -o FILE, --output=FILE
  -S VALUE, --set_value=VALUE

]]

function Usage() print(usage_message) os.exit(true) end

-- Parse the options:
optarg, optind, opterr = getopt(arg, short_opts, long_opts)

if opterr then print(opterr,"\n") Usage() end
if optarg.h then Usage() end -- -h or --help

if not arg[optind] then -- #arg < optind
   print("Not enough arguments\n") 
   Usage()
end

-- Print options:
print("Opt:", "Value:")
for k,v in pairs(optarg) do print(k,v) end
print("")

-- Print non-option arguments that follow:
print("Arg:", "value:")
for i=optind, #arg do print(i, arg[i]) end
print("")

-- Number of non-option arguments:
local nargs = #arg - optind + 1
print("nargs = " .. nargs)

--]==]

