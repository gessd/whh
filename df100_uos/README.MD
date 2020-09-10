# 指纹设备适配

## 文件说明-
### 配置文件
+ 根目录下的com.deepin.test.VFingerprint.conf dbus配置文件
+ com.deepin.test.VFingerprint.service dbus配置文件
+ vfAdaptation.conf 适配deepin指纹配置文件
+ 以上文件可以在 vfingerprint-service.pro 中看到其安装的路径

### 源码文件
+ main.cpp 注册服务
+ testdbusservcie 定义指纹服务

## 使用方法
1. 进入工程目录
2. mkdir build
3. cd build
4. qmake ..
5. make -j4
6. sudo make install

## 测试方法
1. sudo pkill -ef deepin-authenti
2. qdbus --system com.deepin.daemon.Authenticate /com/deepin/daemon/Authenticate/Fingerprint com.deepin.daemon.Authenticate.Fingerprint.Devices 
3. 可以看到 [{"name":"vdevice","available":true,"capability":1}] 的指纹设备数据。

