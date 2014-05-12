How to add the guslib library to your Visual Studio project.
0) Download the zip or 7z file and unpack it somewhere.

You can add the library directly to your project without building it into a lib.
1) Just add all the source and header files to your project.

If you want to use the library separately (either the "guslib.lib" or "guslibD.lib"
1) In your project, add the [guslib\guslib] folder to the [C/C++] -> [General] -> {Additional Include Directories}
2) Add the folder where the "guslib.lib" or "guslibD.lib" files are to the [Linker] -> [General] -> {Additional Include Directories}
3) Add "guslib.lib" (for Release builds) or "guslib_D.lib" (for Debug) to [Linker] -> [Input] -> {Additional Dependencies}