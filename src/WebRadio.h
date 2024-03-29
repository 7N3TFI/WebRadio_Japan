/*
 * https://twitter.com/wakwak_koba/
 */

#ifndef _WAKWAK_KOBA_WEBRADIO_HPP_
#define _WAKWAK_KOBA_WEBRADIO_HPP_

#include <functional>
#include <vector>
#include <nvs.h>

static void voidDownloadTask(void *radio);
static void voidDecodeTask(void *radio);

struct area_t {
    const char * name;
    const float lat;
    const float lon;
};

// 構造体配列の宣言・初期化
static const area_t areas[] = {
  { nullptr, 0, 0},
  { "北海道", 43.064615F, 141.346807F},
  { "青森", 40.824308F, 140.739998F},
  { "岩手", 39.703619F, 141.152684F},
  { "宮城", 38.268837F, 140.8721F},
  { "秋田", 39.718614F, 140.102364F},
  { "山形", 38.240436F, 140.363633F},
  { "福島", 37.750299F, 140.467551F},
  { "茨城", 36.341811F, 140.446793F},
  { "栃木", 36.565725F, 139.883565F},
  { "群馬", 36.390668F, 139.060406F},
  { "埼玉", 35.856999F, 139.648849F},
  { "千葉", 35.605057F, 140.123306F},
  { "東京", 35.689488F, 139.691706F},
  { "神奈川", 35.447507F, 139.642345F},
  { "新潟", 37.902552F, 139.023095F},
  { "富山", 36.695291F, 137.211338F},
  { "石川", 36.594682F, 136.625573F},
  { "福井", 36.065178F, 136.221527F},
  { "山梨", 35.664158F, 138.568449F},
  { "長野", 36.651299F, 138.180956F},
  { "岐阜", 35.391227F, 136.722291F},
  { "静岡", 34.97712F, 138.383084F},
  { "愛知", 35.180188F, 136.906565F},
  { "三重", 34.730283F, 136.508588F},
  { "滋賀", 35.004531F, 135.86859F},
  { "京都", 35.021247F, 135.755597F},
  { "大阪", 34.686297F, 135.519661F},
  { "兵庫", 34.691269F, 135.183071F},
  { "奈良", 34.685334F, 135.832742F},
  { "和歌山", 34.225987F, 135.167509F},
  { "鳥取", 35.503891F, 134.237736F},
  { "島根", 35.472295F, 133.0505F},
  { "岡山", 34.661751F, 133.934406F},
  { "広島", 34.39656F, 132.459622F},
  { "山口", 34.185956F, 131.470649F},
  { "徳島", 34.065718F, 134.55936F},
  { "香川", 34.340149F, 134.043444F},
  { "愛媛", 33.841624F, 132.765681F},
  { "高知", 33.559706F, 133.531079F},
  { "福岡", 33.606576F, 130.418297F},
  { "佐賀", 33.249442F, 130.299794F},
  { "長崎", 32.744839F, 129.873756F},
  { "熊本", 32.789827F, 130.741667F},
  { "大分", 33.238172F, 131.612619F},
  { "宮崎", 31.911096F, 131.423893F},
  { "鹿児島", 31.560146F, 130.557978F},
  { "沖縄", 26.2124F, 127.680932},
};

class WebRadio {
  public:
    class Station {
      public:
        virtual const char * getName()  { return nullptr; }
        virtual bool play() { return radio->play(this); }
      protected:
        Station(WebRadio * _radio) : radio(_radio) {}
        WebRadio * radio = nullptr;
    };
  protected:
    WebRadio(AudioOutput * _out) : out(_out) {}
    
    WebRadio(AudioOutput * _out, const byte _cpuDecode, const uint16_t _stackDecode, const UBaseType_t _priorityDecode) : out(_out), cpuDecode(_cpuDecode), stackDecode(_stackDecode), priorityDecode(_priorityDecode) {
//    xTaskCreatePinnedToCore(&voidDecodeTask  , "decode"  , stackDecode  , this, priorityDecode, &decode_handle  , cpuDecode  );
    }
    
    WebRadio(AudioOutput * _out, const byte _cpuDecode, const uint16_t _stackDecode, const UBaseType_t _priorityDecode, const byte _cpuDownload, const uint16_t _stackDownload) : out(_out), cpuDecode(_cpuDecode), stackDecode(_stackDecode), priorityDecode(_priorityDecode), cpuDownload(_cpuDownload), stackDownload(_stackDownload) {
//    xTaskCreatePinnedToCore(&voidDecodeTask  , "decode"  , stackDecode  , this, priorityDecode, &decode_handle  , cpuDecode  );
//    xTaskCreatePinnedToCore(&voidDownloadTask, "download", stackDownload, this, 1, &download_handle, cpuDownload);
    }
    
    ~WebRadio() {
      if(download_handle)
        vTaskDelete(download_handle);
      if(decode_handle)
        vTaskDelete(decode_handle);
      deInit();
    }
    
    void startTask() {
      if(decode_handle)
        return;
      
      if(!stackDownload)
        xTaskCreatePinnedToCore(&voidDecodeTask  , "decode"  , stackDecode  , this, priorityDecode, &decode_handle  , cpuDecode  );
      else {
        xTaskCreatePinnedToCore(&voidDecodeTask  , "decode"  , stackDecode  , this, priorityDecode, &decode_handle  , cpuDecode  ); 
        xTaskCreatePinnedToCore(&voidDownloadTask, "download", stackDownload, this, 1, &download_handle, cpuDownload);
      }
    }
    
    int getIndex(Station * station) {
      for(int i = 0; i < stations.size(); i++)
        if(stations[i] == station)
          return i;
      return -1;
    }

    const area_t * getLocation(uint8_t pref) {
      if(pref >= sizeof(areas) / sizeof(areas[0]))
        return nullptr;
      return &areas[pref];
    }

    virtual void downloadTask() {
      for(;;) {
        delay(10);
        downloadTaskCore();
      }
    }

    virtual void decodeTask() {
      for(;;) {
        delay(1);
        decodeTaskCore();
      }
    }

    virtual void saveStation(WebRadio::Station * station) {
      uint32_t nvs_handle;
      if (!nvs_open("WebRadio", NVS_READWRITE, &nvs_handle)) {
        saveStationCore(nvs_handle, station);
        nvs_close(nvs_handle);
      }
    }
    
    virtual WebRadio::Station * restoreStation() {
      WebRadio::Station * result = nullptr;
      uint32_t nvs_handle;
      if (!nvs_open("WebRadio", NVS_READONLY, &nvs_handle)) {
        result = restoreStationCore(nvs_handle);
        nvs_close(nvs_handle);
      }
      return result;
    }
    
    virtual void deInit() {
      for (auto itr : stations)
        delete itr;
      stations.clear();
    }
    
    virtual void downloadTaskCore() {;}
    virtual void decodeTaskCore() {;}
    virtual void saveStationCore(uint32_t nvs_handle, WebRadio::Station * station) {}
    virtual WebRadio::Station * restoreStationCore(uint32_t nvs_handle) {return nullptr;}  

    virtual void sendSyslog(const char *text) {
      if(syslog_udp_address != nullptr && syslog_udp_port) {
        WiFiUDP udp;
        static uint64_t seq = 0;
        
        udp.beginPacket(syslog_udp_address, syslog_udp_port);
        udp.printf("SEQ:%llu %s", ++seq, text);
        udp.endPacket();
      }
    }
    
    void sendLog(const char *text, bool error = false) {
      sendSyslog(text);
      if(error && onError)
        onError(text);
      if(!error && onInfo)
        onInfo(text);
    }
    void sendLog(const String text, bool error = false) {
      sendLog(text.c_str(), error);
    }
    
  public:
    virtual bool begin() { return false; }
    virtual bool play(Station * station = nullptr) {
      if(!station)
        station = restoreStation();

      if(!station && stations.size() > defaultStationIdx)
        station = stations[defaultStationIdx];
      if(!station && stations.size() > 0)
        station = stations[0];

      if(station) {
        select_station = station;
        return true;
      }
      return false;
    }
    virtual bool play(bool next) {
      auto sn = getNumOfStations();
      return play(getStation((getIndex(current_station) + sn + (next ? 1 : -1)) % sn));
    } // true:next false:previous
    
    virtual void stop() {}
    virtual void handle() {;}
    
    virtual size_t getNumOfStations() { return stations.size(); }
    virtual Station * getStation() {
      return current_station;
    }
    virtual Station * getStation(size_t index) {
      if(index >= stations.size())
        return nullptr;

      return stations[index];
    }
    
    virtual bool RegisterMetadataCB(AudioStatus::metadataCBFn fn, void *data) {
      fnCbMetadata = fn;
      fnCbMetadata_data = data;
      return true;
    }

    virtual bool RegisterStatusCB(AudioStatus::statusCBFn fn, void *data) {
      fnCbStatus = fn;
      fnCbStatus_data = data;
      return true;
    }

    virtual void setSyslog() {
      if(syslog_udp_address)
        delete []syslog_udp_address;
      syslog_udp_address = nullptr;
      syslog_udp_port = 0;
    }

    virtual void setSyslog(const char *address, uint16_t port = 514) {
      setSyslog();
      syslog_udp_address = new char[strlen(address) + 1];
      strcpy(syslog_udp_address, address);
      syslog_udp_port = port;
    }

    String getInfoStack() {
      if(download_handle && decode_handle)
        return "Stack: Decode:" + String(uxTaskGetStackHighWaterMark(decode_handle)) + " Download:" + String(uxTaskGetStackHighWaterMark(download_handle));
      else if(decode_handle)
        return "Stack: Decode:" + String(uxTaskGetStackHighWaterMark(decode_handle));
      else
        return "";
    }
    
    virtual void charm() {
      Serial.printf("===============================================================\n");
      Serial.printf("Mem Test\n");
      Serial.printf("===============================================================\n");
      Serial.printf("esp_get_free_heap_size()                              : %7d\n", esp_get_free_heap_size() );
      Serial.printf("esp_get_minimum_free_heap_size()                      : %7d\n", esp_get_minimum_free_heap_size() );
      //xPortGetFreeHeapSize()（データメモリ）ヒープの空きバイト数を返すFreeRTOS関数です。これはを呼び出すのと同じheap_caps_get_free_size(MALLOC_CAP_8BIT)です。
      Serial.printf("xPortGetFreeHeapSize()                                : %7d\n", xPortGetFreeHeapSize() );
      //xPortGetMinimumEverFreeHeapSize()また、関連heap_caps_get_minimum_free_size()するものを使用して、ブート以降のヒープの「最低水準点」を追跡できます。
      Serial.printf("xPortGetMinimumEverFreeHeapSize()                     : %7d\n", xPortGetMinimumEverFreeHeapSize() );
      //heap_caps_get_free_size() さまざまなメモリ機能の現在の空きメモリを返すためにも使用できます。

      Serial.printf("heap_caps_get_free_size(MALLOC_CAP_EXEC)              : %7d\n", heap_caps_get_free_size(MALLOC_CAP_EXEC) );
      Serial.printf("heap_caps_get_free_size(MALLOC_CAP_32BIT)             : %7d\n", heap_caps_get_free_size(MALLOC_CAP_32BIT) );
      Serial.printf("heap_caps_get_free_size(MALLOC_CAP_8BIT)              : %7d\n", heap_caps_get_free_size(MALLOC_CAP_8BIT) );
      Serial.printf("heap_caps_get_free_size(MALLOC_CAP_DMA)               : %7d\n", heap_caps_get_free_size(MALLOC_CAP_DMA) );
      Serial.printf("heap_caps_get_free_size(MALLOC_CAP_PID2)              : %7d\n", heap_caps_get_free_size(MALLOC_CAP_PID2) );
      Serial.printf("heap_caps_get_free_size(MALLOC_CAP_PID3)              : %7d\n", heap_caps_get_free_size(MALLOC_CAP_PID3) );
      Serial.printf("heap_caps_get_free_size(MALLOC_CAP_PID4)              : %7d\n", heap_caps_get_free_size(MALLOC_CAP_PID4) );
      Serial.printf("heap_caps_get_free_size(MALLOC_CAP_PID5)              : %7d\n", heap_caps_get_free_size(MALLOC_CAP_PID5) );
      Serial.printf("heap_caps_get_free_size(MALLOC_CAP_PID6)              : %7d\n", heap_caps_get_free_size(MALLOC_CAP_PID6) );
      Serial.printf("heap_caps_get_free_size(MALLOC_CAP_PID7)              : %7d\n", heap_caps_get_free_size(MALLOC_CAP_PID7) );

      Serial.printf("heap_caps_get_free_size(MALLOC_CAP_SPIRAM)            : %7d\n", heap_caps_get_free_size(MALLOC_CAP_SPIRAM) );
      Serial.printf("heap_caps_get_free_size(MALLOC_CAP_INTERNAL)          : %7d\n", heap_caps_get_free_size(MALLOC_CAP_INTERNAL) );
      Serial.printf("heap_caps_get_free_size(MALLOC_CAP_DEFAULT)           : %7d\n", heap_caps_get_free_size(MALLOC_CAP_DEFAULT) );
      //Serial.printf("heap_caps_get_free_size(MALLOC_CAP_IRAM_8BIT)         : %7d\n", heap_caps_get_free_size(MALLOC_CAP_IRAM_8BIT) );
      Serial.printf("heap_caps_get_free_size(MALLOC_CAP_INVALID)           : %7d\n", heap_caps_get_free_size(MALLOC_CAP_INVALID) );
      //heap_caps_get_largest_free_block()ヒープ内の最大の空きブロックを返すために使用できます。これは、現在可能な最大の単一割り当てです。この値を追跡し、合計空きヒープと比較すると、ヒープの断片化を検出できます。
      Serial.printf("heap_caps_get_largest_free_block(MALLOC_CAP_EXEC)     : %7d\n", heap_caps_get_largest_free_block(MALLOC_CAP_EXEC) );
      Serial.printf("heap_caps_get_largest_free_block(MALLOC_CAP_32BIT)    : %7d\n", heap_caps_get_largest_free_block(MALLOC_CAP_32BIT) );
      Serial.printf("heap_caps_get_largest_free_block(MALLOC_CAP_8BIT)     : %7d\n", heap_caps_get_largest_free_block(MALLOC_CAP_8BIT) );
      Serial.printf("heap_caps_get_largest_free_block(MALLOC_CAP_DMA)      : %7d\n", heap_caps_get_largest_free_block(MALLOC_CAP_DMA) );
      Serial.printf("heap_caps_get_largest_free_block(MALLOC_CAP_PID2)     : %7d\n", heap_caps_get_largest_free_block(MALLOC_CAP_PID2) );
      Serial.printf("heap_caps_get_largest_free_block(MALLOC_CAP_PID3)     : %7d\n", heap_caps_get_largest_free_block(MALLOC_CAP_PID3) );
      Serial.printf("heap_caps_get_largest_free_block(MALLOC_CAP_PID4)     : %7d\n", heap_caps_get_largest_free_block(MALLOC_CAP_PID4) );
      Serial.printf("heap_caps_get_largest_free_block(MALLOC_CAP_PID5)     : %7d\n", heap_caps_get_largest_free_block(MALLOC_CAP_PID5) );
      Serial.printf("heap_caps_get_largest_free_block(MALLOC_CAP_PID6)     : %7d\n", heap_caps_get_largest_free_block(MALLOC_CAP_PID6) );
      Serial.printf("heap_caps_get_largest_free_block(MALLOC_CAP_PID7)     : %7d\n", heap_caps_get_largest_free_block(MALLOC_CAP_PID7) );
      Serial.printf("heap_caps_get_largest_free_block(MALLOC_CAP_SPIRAM)   : %7d\n", heap_caps_get_largest_free_block(MALLOC_CAP_SPIRAM) );
      Serial.printf("heap_caps_get_largest_free_block(MALLOC_CAP_INTERNAL) : %7d\n", heap_caps_get_largest_free_block(MALLOC_CAP_INTERNAL) );
      Serial.printf("heap_caps_get_largest_free_block(MALLOC_CAP_DEFAULT)  : %7d\n", heap_caps_get_largest_free_block(MALLOC_CAP_DEFAULT) );
      //Serial.printf("heap_caps_get_largest_free_block(MALLOC_CAP_IRAM_8BIT): %7d\n", heap_caps_get_largest_free_block(MALLOC_CAP_IRAM_8BIT) );
      Serial.printf("heap_caps_get_largest_free_block(MALLOC_CAP_INVALID)  : %7d\n", heap_caps_get_largest_free_block(MALLOC_CAP_INVALID) );
      //heap_caps_get_minimum_free_size()指定された機能を持つすべての領域の合計最小空きメモリを取得します。
      Serial.printf("heap_caps_get_minimum_free_size(MALLOC_CAP_EXEC)      : %7d\n", heap_caps_get_minimum_free_size(MALLOC_CAP_EXEC) );
      Serial.printf("heap_caps_get_minimum_free_size(MALLOC_CAP_32BIT)     : %7d\n", heap_caps_get_minimum_free_size(MALLOC_CAP_32BIT) );
      Serial.printf("heap_caps_get_minimum_free_size(MALLOC_CAP_8BIT)      : %7d\n", heap_caps_get_minimum_free_size(MALLOC_CAP_8BIT) );
      Serial.printf("heap_caps_get_minimum_free_size(MALLOC_CAP_DMA)       : %7d\n", heap_caps_get_minimum_free_size(MALLOC_CAP_DMA) ); 
      Serial.printf("heap_caps_get_minimum_free_size(MALLOC_CAP_PID2)      : %7d\n", heap_caps_get_minimum_free_size(MALLOC_CAP_PID2) );
      Serial.printf("heap_caps_get_minimum_free_size(MALLOC_CAP_PID3)      : %7d\n", heap_caps_get_minimum_free_size(MALLOC_CAP_PID3) );
      Serial.printf("heap_caps_get_minimum_free_size(MALLOC_CAP_PID4)      : %7d\n", heap_caps_get_minimum_free_size(MALLOC_CAP_PID4) );
      Serial.printf("heap_caps_get_minimum_free_size(MALLOC_CAP_PID5)      : %7d\n", heap_caps_get_minimum_free_size(MALLOC_CAP_PID5) );
      Serial.printf("heap_caps_get_minimum_free_size(MALLOC_CAP_PID6)      : %7d\n", heap_caps_get_minimum_free_size(MALLOC_CAP_PID6) );
      Serial.printf("heap_caps_get_minimum_free_size(MALLOC_CAP_PID7)      : %7d\n", heap_caps_get_minimum_free_size(MALLOC_CAP_PID7) );
      Serial.printf("heap_caps_get_minimum_free_size(MALLOC_CAP_SPIRAM)    : %7d\n", heap_caps_get_minimum_free_size(MALLOC_CAP_SPIRAM) );
      Serial.printf("heap_caps_get_minimum_free_size(MALLOC_CAP_INTERNAL)  : %7d\n", heap_caps_get_minimum_free_size(MALLOC_CAP_INTERNAL) );
      Serial.printf("heap_caps_get_minimum_free_size(MALLOC_CAP_DEFAULT)   : %7d\n", heap_caps_get_minimum_free_size(MALLOC_CAP_DEFAULT) );
      //Serial.printf("heap_caps_get_minimum_free_size(MALLOC_CAP_IRAM_8BIT) : %7d\n", heap_caps_get_minimum_free_size(MALLOC_CAP_IRAM_8BIT) );
      Serial.printf("heap_caps_get_minimum_free_size(MALLOC_CAP_INVALID)   : %7d\n", heap_caps_get_minimum_free_size(MALLOC_CAP_INVALID) );
      //heap_caps_get_info()multi_heap_info_t上記の関数からの情報に加えて、ヒープ固有の追加データ（割り当て数など）を含む構造体を返します。
    } // https://lang-ship.com/blog/work/esp32-heap-1/
    
    std::function<void(const char *station_name, const size_t station_index)> onPlay = nullptr;
    std::function<void(const char *message)> onError = nullptr;
    std::function<void(const char *message)> onInfo = nullptr;
    std::function<void(const char *program_title)> onProgram = nullptr;
    std::function<void(const char *message)> onUnrecover = nullptr;

  protected:
    AudioOutput * out = nullptr;
    std::vector<Station *> stations;
    Station * current_station = nullptr;
    Station * select_station = nullptr;
    unsigned long long saveSettings = 0;
    size_t defaultStationIdx = 0;
    
    TaskHandle_t download_handle;
    TaskHandle_t decode_handle;
    
    AudioStatus::metadataCBFn fnCbMetadata = nullptr;
    void *fnCbMetadata_data = nullptr;
    AudioStatus::statusCBFn fnCbStatus = nullptr;
    void *fnCbStatus_data = nullptr;
    
  private:
    byte cpuDecode;
    uint16_t stackDecode;
    UBaseType_t priorityDecode;
    byte cpuDownload;
    uint16_t stackDownload = 0;

    static void voidDownloadTask(void *radio) {
      ((WebRadio*)radio)->downloadTask();
    }
    
    static void voidDecodeTask(void *radio) {
      ((WebRadio*)radio)->decodeTask();
    }

    char * syslog_udp_address = nullptr;
    uint16_t syslog_udp_port = 0;
};

#endif
