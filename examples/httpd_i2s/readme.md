# httpd_Jcbasimul_i2s / httpd_Radiko_i2s

### 機能
- Jcbasimul / Radiko を受信し、I2S モジュールで出力します。
- 起動すると HTTPサーバー になり、ブラウザで選局できます。
- PSRAM:Enabled に最適化してあります。なくても動きますが、音の途切れが発生する場合があります。

### ビルドに必要なライブラリ
#### Jcbasimul / Radiko 共通
- [espressif/arduino-esp32](https://github.com/espressif/arduino-esp32)
- [Lovyan03/LovyanGFX](https://github.com/lovyan03/LovyanGFX)
- [wakwak-koba/ESP8266Audio](https://github.com/wakwak-koba/ESP8266Audio) forked from [earlephilhower/ESP8266Audio](https://github.com/earlephilhower/ESP8266Audio)
#### Jcbasimul のみ
- [Links2004/arduinoWebSockets](https://github.com/Links2004/arduinoWebSockets)
- [arduino-libraries/Arduino_JSON](https://github.com/arduino-libraries/Arduino_JSON)

### 配線

|PCM5102Aモジュール|ESP32|
|:----:|:----:|
|SCK|GND|
|BCK|G26|
|DIN|G22|
|LCK|G25|
|GND|GND|
|VIN|3V3|

- 他のモジュールでも可能と思われます。
- 上記以外のピンを使用する場合は、スケッチ内 out.SetPinout() でピン番号を指定して下さい。
- ESP-WROOM32(PSRAMなし)、ESP32-WROVER-B(PSRAMあり) でテストしています。

![image1](/docs/i2s.JPG)
