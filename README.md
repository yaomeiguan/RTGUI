# 简介

RTGUI 是一系列 RT-Thread 组件的一部分。它的目的是为开发人员提供一个便于在嵌入式系统中使用的图形界面库。以其和 RT-Thread 天然集成的优势，方便开发人员开发。这个图形用户界面组件能够为RT-Thread上的应用程序提供人机界面交互的功能，例如人机界面设备，设备信息显示，播放器界面等。图形用户界面组件的功能包括：

*  多线程图形用户界面；
*  依赖于RT-Thread线程调度器的实时图形用户界面；
*  C语言方式的全面向对象设计：
    -  对象具备运行时类型信息；
    -  对象自动销毁，使得内存的管理更为轻松；
*  界面主题支持；
*  中文文本显示支持；
*  丰富的控件支持：
    -  button，checkbox，radiobox
    -  textbox
    -  progressbar，slider
    -  列表视图，文件列表视图
    -  等等

# 使用方法

1. 直接使用(在 windows 中模拟运行)

    安装好 VS(2008+，express版应该就可以)，在根目录下运行 scons。这时在 executeables 目录下会有两个可执行文件。一个是 demo，一个是 realtouch。把 win32/SDL/lib 里的 SDL.dll 复制到 executeables 里即可运行以上应用程序。

2. 在 RT-Thread 中使用

    在系统或运行环境中设置 RTT_RTGUI 变量，使其指向 components/rtgui 。然后再在你的工程目录里重新运行 scons 即可。

