#!/usr/bin/env texlua  

VERSION = "0.4"

--[[
     m-tx.lua: processes MusiXTeX files using prepmx and pmxab as pre-processors (and deletes intermediate files)

     (c) Copyright 2011-2012 Bob Tennent rdt@cs.queensu.ca

     This program is free software; you can redistribute it and/or modify it
     under the terms of the GNU General Public License as published by the
     Free Software Foundation; either version 2 of the License, or (at your
     option) any later version.

     This program is distributed in the hope that it will be useful,
     but WITHOUT ANY WARRANTY; without even the implied warranty of
     MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General
     Public License for more details.

     You should have received a copy of the GNU General Public License along
     with this program; if not, write to the Free Software Foundation, Inc.,
     51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.

--]]

--[[

  ChangeLog:

     version 0.4   2013-12-11 RDT
       Add -c option for pmxchords preprocessing of pmx file
       Add -F fmt option to use fmt as TeX processor

     version 0.3   2012-04-09 RDT
       Change name to m-tx to avoid clash with another program.

     version 0.2   2011-11-28 RDT
       Added -i (retain intermediate files) option

     version 0.1.1 2011-07-18 RDT

       Removed a file check that caused it to fail (only on Windows!)

     version 0.1   2011-07-15 RDT

--]]

function usage()
  print("Usage:  [texlua] m-tx.lua { option | basename[.mtx] } ... ")
  print("options: -v  version")
  print("         -h  help")
  print("         -l  latex (or pdflatex)")
  print("         -p  pdfetex (or pdflatex)")
  print("         -d  dvipdfm")
  print("         -c  preprocess pmx file using pmxchords")
  print("         -F fmt  use fmt as the TeX processor")
  print("         -s  stop at dvi")
  print("         -t  stop at tex/mid")
  print("         -m  stop at pmx")
  print("         -i  retain intermediate files")
  print("         -f  restore default processing")
end

function whoami ()
  print("This is m-tx.lua version ".. VERSION .. ".")
end

whoami()
if #arg == 0 then
  usage()
  os.exit(0)
end

-- defaults:
prepmx = "prepmx"
pmx = "pmxab"
tex = "etex"  
musixflx = "musixflx"
dvi = "dvips"
ps2pdf = "ps2pdf"
intermediate = 1

exit_code = 0
narg = 1
repeat
  this_arg = arg[narg]
  if this_arg == "-v" then
    os.exit(0)
  elseif this_arg == "-h" then
    usage()
    os.exit(0)
  elseif this_arg == "-l" then
    if tex == "pdfetex" then
      tex = "pdflatex"
    else
      tex = "latex"
    end
  elseif this_arg == "-p" then
    if tex == "latex" then
      tex = "pdflatex"
    else
      tex = "pdfetex"
    end
    dvi = ""; ps2pdf = ""
  elseif this_arg == "-d" then
    dvi = "dvipdfm"; ps2pdf = ""
  elseif this_arg == "-c" then
    pmx = "pmxchords"
  elseif this_arg == "-F" then
    narg = narg+1
    tex = arg[narg]
  elseif this_arg == "-s" then
    dvi = ""; ps2pdf = ""
  elseif this_arg == "-i" then
    intermediate = 0
  elseif this_arg == "-f" then
    pmx = "pmxab"; tex = "etex"; dvi = "dvips"; ps2pdf = "ps2pdf"; intermediate = 1
  elseif this_arg == "-t" then
    tex = ""; dvi = ""; ps2pdf = ""
  elseif this_arg == "-m" then
    pmx = ""; tex = ""; dvi = ""; ps2pdf = ""
  else
    filename = this_arg 
    if filename ~= "" and string.sub(filename, -4, -1) == ".mtx" then
        filename = string.sub(filename, 1, -5)
    end
    print("Processing ".. filename .. ".mtx.")
    OK = true
    if ( os.execute(prepmx .. " " .. filename ) ~= 0 ) then
      OK = false
    end
    if  (OK and pmx ~= "") then 
      os.execute(pmx .. " " .. filename)  -- does pmx return an error code?
      pmxaerr = io.open("pmxaerr.dat", "r")
      if (not pmxaerr) then
        print("No log file.")
        OK = false
      else
        linebuf = pmxaerr:read()
        err = tonumber(linebuf)
        if (err ~= 0) then
          OK = false
        end
        pmxaerr:close()
      end
    end
    os.remove( filename .. ".mx2" )
    if ( OK ) and
       (tex == "" or os.execute(tex .. " " .. filename) == 0) and
       (tex == "" or os.execute(musixflx .. " " .. filename) == 0) and
       (tex == "" or os.execute(tex .. " " .. filename) == 0) and
       ((tex ~= "latex" and tex ~= "pdflatex") 
         or (os.execute(tex .. " " .. filename) == 0)) and
       (dvi == "" or  (os.execute(dvi .. " " .. filename) == 0)) and
       (ps2pdf == "" or (os.execute(ps2pdf .. " " .. filename .. ".ps") == 0) )
    then 
      if ps2pdf ~= "" then 
        print(filename .. ".pdf generated by " .. ps2pdf .. ".")
      end
      if intermediate == 1 then -- clean-up:
        os.remove( "pmxaerr.dat" )
        os.remove( filename .. ".mx1" )
        os.remove( filename .. ".mx2" )
        if pmx ~= "" then
          os.remove( filename .. ".pmx" )
        end
        if dvi ~= "" then
          os.remove( filename .. ".dvi" )
        end
        if ps2pdf ~= "" then 
          os.remove( filename .. ".ps" )
        end
      end
    else
      print("M-Tx/pmx/MusiXTeX processing of " .. filename .. ".mtx fails.\n")
      exit_code = 2
      --[[ uncomment for debugging
      print("tex = ", tex)
      print("dvi = ", dvi)
      print("ps2pdf = ", ps2pdf)
      --]]
    end
  end --if this_arg == ...
  narg = narg+1
until narg > #arg 
os.exit( exit_code )
