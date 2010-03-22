
--[===================================================================[--

  Script and program wrappers in TeX Live for Windows
  
  License
  
    Public Domain
    
    Originally written in 2009 by Tomasz M. Trzeciak.
    
    Prior work:
    'tl-w32-wrapper.texlua' by Reinhard Kotucha and Norbert Preining.
    'tl-w32-wrapper.cmd' by Tomasz M. Trzeciak.
  
  Rationale
  
    Wrappers enable use of scripts on Windows as regular programs. 
    They are also required for some binary programs to set-up the 
    right environment for them. 
    
    Batch scripts can be used for wrapping but they are not as universal 
    as binaries (there are some odd cases where they don't work) and 
    it is hard to make them robust and secure. Compiled binary wrappers 
    don't suffer from these problems but they are harder to write, debug
    and maintain in comparison to scripts. For these reasons a hybrid 
    approach was adopted that offers the best of both worlds - a binary 
    stub combined with a launcher script.
  
  Source files
  
    runscript.tlu     launcher script for locating and dispatching 
                      target scripts/programs
    runscript_dll.c   common DLL part of the binary stubs; locates and
                      calls the launcher script
    runscript_exe.c   EXE proxy to the common DLL for CLI mode stubs
    wrunscript_exe.c  EXE proxy to the common DLL for GUI mode stubs
  
  Compilation of binaries (requires luatex.dll in the same directory)

    with gcc (size optimized):
    
    gcc -Os -s -shared -o runscript.dll runscript_dll.c -L./ -lluatex
    gcc -Os -s -o runscript.exe runscript_exe.c -L./ -lrunscript
    gcc -mwindows -Os -s -o wrunscript.exe wrunscript_exe.c -L./ -lrunscript

    with tcc (extra small size):
    
    tiny_impdef luatex.dll
    tcc -shared -o runscript.dll runscript_dll.c luatex.def
    tcc -o runscript.exe runscript_exe.c runscript.def
    tcc -o wrunscript.exe wrunscript_exe.c runscript.def

  Structure of the wrapper
  
    Wrappers consist of small binary stubs and a common texlua script. 
    The binary stubs are all the same, just different names (but CLI 
    and GUI stubs differ, see below, and GUI stubs are actually all 
    different due to different embedded icons).
    
    The job of the binary stub is twofold: (a) call the texlua launcher
    script 'runscript.tlu' from the same directory (or more precisely 
    from the directory containing 'runscript.dll') and (b) pass to it 
    argv[0] and the unparsed argument string as the last two arguments 
    (after adding a sentinel argument, which ends with a new line 
    character). Arbitrary C strings can be passed, because the script 
    is executed by linking with luatex.dll and calling its lua 
    interpreter directly rather than by spawning a new process.
    
    There are two flavours of the binary stub: one for CLI programs 
    and another one for GUI programs. The GUI variant does not open 
    a console window nor does it block the command prompt if started 
    from there. It also uses a dialog box to display an error message 
    in addition to outputting to stderr.
    
    The stubs are further split into a common DLL and EXE proxies 
    to it. This is for maintenance reasons - updates can be done by 
    replacement of a single DLL rather than all binary stubs.
    
    The launcher script knows, which variant has been used to invoke it 
    based on the sentinel argument. The lack of this argument means 
    that it was invoked in a standard way, i.e. through texlua.exe. 
    
    All the hard work of locating a script/program to execute happens 
    in the launcher script. The located script/program is always 
    executed directly by spawning its interpreter (or binary) in a new 
    process. The system shell (cmd.exe) is never called (except for 
    batch scripts, of course). If the located script happens to be 
    a (tex)lua script, it is loaded and called directly from within 
    this script, i.e. no new process is spawned.
    
  Adding wrappers for user scripts
  
    The script wrapping machinery is not limited to scripts included in 
    TeX Live. You can also use it for script programs from manually 
    installed packages, which should minimize the problems when using 
    them with TeX Live. 
    
    First, make sure that there is an interpreter program available on 
    your system for the script you want to use. Interpreters for Perl 
    and Lua are bundled with TeX Live, all others have to be installed 
    independently. The following script types and their file extensions 
    are currently supported and searched in that order:
    
      Lua      (.tlu;.texlua;.lua) --  included
      Perl     (.pl)               --  included
      Ruby     (.rb)               --  requires installation
      Python   (.py)               --  requires installation
      Java     (.jar)              --  requires installation
      VBScript (.vbs)              --  part of Windows
      Jscript  (.js)               --  part of Windows
      Batch    (.bat;.cmd)         --  part of Windows
      
    For Unix-style extensionless scripts their first line starting with
    she-bang (#!) is consulted for the interpreter program to run. This 
    program must be present on the search path.
    
    Next, the script program needs to be installed somewhere below the 
    'scripts' directory under one of the TEXMF trees (consult the 
    documentation or texmf/web2c/texmf.cnf file for a list). You may 
    need to update the file search database afterwards with:
    
      mktexlsr [TEXMFDIR]
      
    Test if the script can be located with:
    
      kpsewhich --format=texmfscripts <script-name>.<ext>
    
    Once installed the script can be called immediately with:
    
      runscript <script-name> [script arguments]
    
    If you prefer to call the script program simply by its name, copy 
    and rename bin/win32/runscript.exe to <script-name>.exe and put it 
    in bin/win32/ directory of your TeX Live installation or, if you 
    don't have the write permissions there, somewhere else on the search 
    path. 
    
  Changelog:
  
    2009/12/04 - initial version
    2009/12/15 - minor fixes for path & extension list parsing
    2010/01/09 - add support for GUI mode stubs
    2010/02/28 - enable GUI mode stubs for dviout, psv and texworks;
               - added generic handling of sys programs
               - added restricted repstopdf to alias_table
    2010/03/13 - added 'readme.txt' and changelog
               - added support and docs for calling user added scripts; 
                 (use path of 'runscript.dll' instead of .exe stub to 
                 locate 'runscript.tlu' script)
               - limit search for shell_escape_commands to system trees
               - added function for creating directory hierarchy 
               - fixed directory creation for dviout & texworks aliases
               - fixed arg[0] of repstopdf & rpdfcrop

--]===================================================================]--
