# LogOSMViewer
#How To Run

Clone into a new repository
Run: 
```git submodule update --init --recursive```

Use cmake to build and run in a build directory

Should work across both linux and Mac though I only have linux so I cannot test with mac

Uses C++17, https://github.com/nlohmann/json, and https://github.com/whoshuu/cpr. Submodules should properly install these for you

#My versions:
```g++ (GCC) 8.2.1 20181127

cmake version 3.14.3
```