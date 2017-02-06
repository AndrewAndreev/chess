Using .pro file in Qt Creator
-------------

In order to use generated **.pro** file follow the steps below

> - Open **.pro** file in Qt Creator
> - Add project build in path like `../build/desktop_qt_build/`

There is already structured output as in *msvc* build and to be consistent in build structure I highly recommend to make both of `Release` and `Debug` build paths the same.

After this has been done you can build the project.
If you did all right you will get this structure:
```
 build
│
└── qt580_msvc2015
    ├── 64bit/32bit # depends on your Qt version
    │   ├── debug
    │   │   └── chess # target file
    │   └── release
    │       └── chess # target file
    ├── GeneratedFiles
    │   ├── debug
    │   │   └── moc_*.cpp
    │   ├── qrc_res.cpp
    │   ├── release
    │   │   └── moc_*.cpp
    │   └── ui_*.h
    ├── Makefile
    ├── Makefile.Debug
    ├── Makefile.Release
    └── objs
        └── 64bit/32bit # depends on your Qt version
            ├── debug
            │   └── *.obj
            └── release
                └── *.obj
```

*Note: qmake generates additional 'release' and 'debug' folders in __GeneratedFiles__ folder and I didn't figured out yet how to make it not to generate them.*
