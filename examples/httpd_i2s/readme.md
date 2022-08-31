# httpd_Jcbasimul_i2s / httpd_Radiko_i2s

### 機能
- Jcbasimul / Radiko を受信し、I2S モジュールで出力します。
- 起動すると HTTPサーバー になり、ブラウザで選局できます。
- PSRAM:Enabled に最適化してあります。なくても動きますが、音の途切れが発生する場合があります。

### 配線

|PCM5102Aモジュール|ESP32|
|:----:|:----:|
|SCK|GND|
|BCK|G22|
|DIN|G25|
|SLCK|G26|
|GND|GND|
|VCC|3V3|

- 他のモジュールでも可能と思われます。
- 上記以外のピンを使用する場合は、スケッチ内 out.SetPinout() でピン番号を指定して下さい。

![image1](/docs/i2s.JPG)

