# PTCR
## Requirements
CPU with **at least** SSE 4.1 support, ideally fma3 too...<br>
Release version is compiled with -msse4.1 and -mfma <br>
Thus only Haswell or AMD equivalent architectures and newer are supported. <br>
For older CPUs (Sandy/Ivy bridge), remove **-mfma** flag from additional options in project settings and compile yourself.
![image](https://user-images.githubusercontent.com/82727531/196032853-6ed06734-ae9e-4a0d-9276-fb89fe104cd1.png)

## Compile instructions:
1) Requires **Visual Studio 202x** version with **Clang compiler** installed !!!
2) Instructions for adding Clang to VS here:<br>
https://learn.microsoft.com/en-us/cpp/build/clang-support-msbuild?view=msvc-170
3) Open .sln file
4) Compile and run
5) You can also use the precompiled version in "Releases" github section (only Haswell and newer).
