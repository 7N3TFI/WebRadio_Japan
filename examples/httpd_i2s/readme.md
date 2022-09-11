# httpd_Jcbasimul_i2s / httpd_Radiko_i2s

### 機能
- Jcbasimul / Radiko を受信し、I2S モジュールで出力します。
- 起動すると HTTPサーバー になり、ブラウザで選局できます。
- PSRAM:Enabled に最適化してあります。なくても動きますが、音の途切れが発生する場合があります。

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



# httpd_Jcbasimul_i2s_magimajyo / httpd_Radiko_i2s_magimajyo

### 機能
- httpd_Jcbasimul_i2s / httpd_Radiko_i2s に加えて、マジョカアイリス を接続する前提で局名などが表示されます

### 配線

|マジョカアイリス|信号名|ESP32|
|:----:|:--:|:----:|
|1|GND|GND|
|2|RST|G33|
|3|GND|GND|
|4|D0|G23|
|5|D1|G21|
|6|D2|G19|
|7|D3|G18|
|8|D4|G5|
|9|D5|G4|
|10|D6|G2|
|11|D7|G15|
|12|CS1|G14|
|13|RD|G13|
|14|WR|G12|
|15|CS2|G14|
|16|DC|G27|
|18|VDD|3V3|
|19|VDDIO|3V3|
|20|LED+||
|21|LED-||
|22|GND|
