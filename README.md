## 一、简介 ##
使用DirectFB对NUC972的2D图像加速驱动进行封装，可供qt使用或单独建立基于DirectFB的图形应用。

目前移植支持了NUC972的画线、矩形填充和Bitblt块传输功能。

关于DirectFB简介请参考：http://baike.baidu.com/item/DirectFB

DirectFB主页：http://directfb.net/

github：https://github.com/Distrotech/DirectFB

## 二、使用 ##
1、下载

    git clone https://github.com/nuc97xx/directfb

2、编译DirectFB

	cd ./DirectFB-1.4.5
	./configure --prefix=/home/share/mnt CC=arm-linux-gcc --host=arm-linux --build=i386-linux --disable-x11 --with-gfxdrivers=vmware --with-tests --enable-debug --enable-debug-support --disable-multi
	make
	make install

3、单独编译NUC972 gfxdrivers 驱动
	
	cd ./gfxdrivers/nuc970
	make
编译完成后拷贝当前目录下.lib目录下的libdirectfb_nuc970.so到prefix指定的安装目录下的lib/directfb-1.4-5/gfxdrivers目录下即可。

更多信息请参考个人博客：http://blog.csdn.net/jxgz_leo/article/details/70137304

## 三、运行 ##
1、设置系统模块路径

	export DFBARGS=module-dir=/mnt/lib/directfb-1.4-5

路径根据自己实际情况设定

2、执行test程序

切换到安装目录下的bin目录下，执行test程序，如dfbtest_fillrect即可看到dfb启动信息

## 四、主要事项 ##
当DFB使用的system为fb时，必须要满足以下两点要求DFB才会启用2D硬件加速。

1、拿到更多的物理内存。我们知道当使用fb驱动时实际上是映射了一段物理内存到用户空间来进行操作，这里我们需要得到更多的物理内存则需要在fb驱动初始化的时候从系统分配到更多的内存。

2、fb需双缓冲的支持。关于这一点请参考我的个人博客：http://blog.csdn.net/jxgz_leo/article/details/70216901

关于更多详细信息欢迎访问我的个人博客：http://blog.csdn.net/jxgz_leo/article/category/6846987

若使用过程中遇到疑问请尽量用留言的方式，但我能力和时间有限也不一定多能一一解答，感谢支持！