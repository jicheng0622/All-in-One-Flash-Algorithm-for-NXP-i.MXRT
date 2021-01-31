# All in One i.MXRT1050/RT1020 SPI Flash Algorithm for Keil

在阅读此文之前强烈建议先通读一遍我在[AllinOne Flash Alorigthm for IAR](https://github.com/jicheng0622/All-in-One-Flash-Algorithm-for-RT1050-RT1020/tree/master/IAR)项目里的Readme以了解AllinOne的初衷以及原理机制，Keil的flash算法实现在原理上跟IAR下是一致的。此次把J-Flash和Keil下的flash算法也更新完之后，此AllinOne项目算是告一段落了，后面会在有需要的时候更新些功能或者修正用户反馈的bug，望大家在RT Design的路上无此忧，偶心足矣。。。

## 测试平台

*开发板：*i.MXRT1021_EVK, i.MXRT1050_EVK

*SPI Flash:* IS25LP64, GD25Q32, S26KS512(HyperFlash)

*Tool:* J-Link_v9, Keil_v5.31

SDK: SDK_2.6.1

## 使用方法：

（1）从我的[Github repo](https://github.com/jicheng0622/All-in-One-Flash-Algorithm-for-RT1050-RT1020)下载AllinOne Flash Algorithm for Keil代码工程并在Keil环境下分别选择FlashRT1020_SPINor和FlashRT1050_SPINor编译通过，此时可以在工程根目录下生成FlashIMXRT1020_SPINor.FLM和FlashIMXRT1050_SPINor.FLM算法文件（一个是针对RT1020系列，一个是针对RT1050系列），将两个Flash算法文件一块拷贝到Keil安装目录下C:\Keil_v5\ARM\Flash文件夹里；

**注意：Keil下的FLM算法文件与J-Flash下的FLM算法文件不能通用**

（2）打开Keil应用工程（以RT1021 SDK2.6.1路径\driver_examples\gpio\led_output\mdk为例），进入Options->Debugger->Settings->Flash Download，点击“Add”选择MIMXRT1021_SPINor_AllinOne（如果板载芯片是RT1050的话需要选择MIMXRT1050_SPINor_AllinOne），然后点击"OK"退出；

<img src="Figures/Flashdownload.png" alt="image-20210103212049329" style="zoom:67%;" />

（3）上述两步已经将新添加的Flash烧写算法load到对应工程了，此时如果是官方RT1021/RT1052_EVK默认板载Flash的话，在把当前工程编译成功之后即可点击Keil菜单栏Flash->Download或者debug按钮将编译后的image烧写到片外SPI Nor Flash里。

### 如何给Keil Flash烧写算法传入命令配置字

如我在AllinOne Flash Algorithm for IAR项目里的详细解释，该算法可以支持符合JEDEC-216A及以后版本直接烧写，因为QE bit即4线使能位可以通过SFDP命令读取出来，这样算法可以解析命令并自动把QE bit置位，而对于老版本的JEDEC216 SPI Flash（市面上很多常用的QSPI Flash仍然是该版本），flash算法无法自动获取QE的位置和对应的使能命令，此时就需要手动地给Flash算法输入一个Option0命令告诉它怎么去使能QE（至于如何获取不同JEDEC216 QSPI Flash的Option0命令请参考AllinOne Flash Algorithm for IAR项目里面的说明），此外如果外部QSPI Flash挂在RT1050的Secondary pinmux组上作为启动设备的话还要另外输入Option1命令。那在Keil里如何给Flash算法输入这两个参数呢？

我这里给了一种解决方案，打开Keil工程目录下的evkmimxrt1020_flexspi_nor.ini文件，添加如下图红色圆圈里的代码，即在指定的memory address（0x20201000~0x20201010 RT1020/1050的OCRAM寻址空间）里输入Option0和Option1的标志和参数（0x4F707430位Opt0的ASCII码，另外之所以加标志判断是确保Opt0/1的value是我们输入的而不是OCRAM本身的不确定的值），然后在flash算法里添加对应的判断和解析处理，这样即可完成外部参数的输入了。同时需要在Options->Utilites选项卡里选择如下图2，这样即可在触发Flash Download之前把Option0和Option1参数传入Flash算法之中。

<img src="Figures/Keil_ini.png" alt="image-20210103215329682" style="zoom:67%;" />

<img src="Figures/Keil_utilites.png" alt="image-20210103215643182" style="zoom:67%;" />

至于Flash烧写算法是如何判断和解析这Option0和Option1这两个参数，参见下图：

<img src="Figures/options_handle_at_keil.png" alt="options_handle_at_keil" style="zoom:67%;" />

至此我们就完全实现了AllinOne Flash Algorithm for Keil的功能了，此时我们再点击Keil菜单栏Flash->Download或者debug按钮将编译后的image烧写到对应匹配的片外SPI Nor Flash里了。实际上相比于IAR里的区别，Keil只是需要解决Option0和Option1的外部参数输入问题，下面我也是给出了几个常用的QSPI Norflash配置字的示例供大家参考。

### 常用QSPI Norflash配置字示例

|                          Flash型号                           |    Opt0配置字    |
| :----------------------------------------------------------: | :--------------: |
|                         GD25Q32/Q64                          |    0xC0000406    |
|                      W25Q80/Q16/Q32/Q64                      |    0xC0000205    |
|                          MX25L6433F                          |    0xC0000105    |
|                  IS25LP032/064（JEDEC216A）                  | 0xC0000107(选配) |
| S26KS512SDPBHI02<br />(HyperFlash 1.8v, EVK_RT1050默认Flash) |    0xC0233009    |

当然如果是用户片外的SPI Flash型号固定不变的话，最简单的方式就是直接打开Keil Flash算法工程然后修改默认的Option0和Option1的配置，重新编译生成你想要的FLM算法文件之参考上面提到第（1）和第（2）步去做即可。

<img src="Figures/DefaultOptions2.png" alt="DefaultOptions2" style="zoom:67%;" />

## 项目维护者：

jicheng0622

## 联系方式：

jicheng0622@163.com