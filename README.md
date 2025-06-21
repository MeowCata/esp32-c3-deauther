# esp32-c3-deauther
A powerful WiFi deauther based on ESP32-C3 board

ORIGIN: [here](https://github.com/wolfyy59/esp32c3-deauther)

Developing Environment: VS Code+Platformio

Board: ESP32-C3 SuperMini

## 对于原项目的修改：
* 考虑到隐蔽性，关闭了原项目对于LED的控制(换掉了原先定义的LED Pin)，但代码未删除，可以自己改回来
* `main.cpp`: 添加了新的LED控制，具体来说，刷入固件后给板子上电，待Web界面准备完毕后LED会blink一下(duration 100ms); 添加了关于Boot按钮的控制，按下Boot按钮LED也会blink~; 同时，现在**板子在开机时会先扫描一遍周围网络(passive)，这样在Web界面里面就不用再按Rescan按钮啦**
* `web_interface.cpp`:修改UI，添加主芯片温度显示，添加DEAUTH ALL和StopAP按钮，控制更方便.(当然，StopAP按钮也是为隐蔽性而设计的)
* `deauth.cpp`:改动最大，*Grok3*重写了Deauth All的代码，实现了真正的踢掉周围所有设备的网络(逻辑来源：[here](https://github.com/zRCrackiiN/DeauthKeychain)). 注：Deauth All时，板子发出的AP将会断开
* `Others`: 小改动，懒得统计啦

> [!NOTE]
> 板子运行时，芯片温度将会很高！(70°C+/158°F+) 请注意不要烫到手

> 所有代码在ESP32-C3 SuperMini上测试通过，应该可以在所有搭载ESP32-C3主控的板子上运行

> [!WARNING]
> 技术本无罪，善恶在人心！在中国等许多国家，非法入侵、干扰他人信号属于违法行为！本项目所有测试在内网进行，请妥善使用项目，造成的一切后果与作者无关（若不同意此协议，请勿使用本项目）

ps:尊重原项目作者，本项目也使用GNU v3 License
