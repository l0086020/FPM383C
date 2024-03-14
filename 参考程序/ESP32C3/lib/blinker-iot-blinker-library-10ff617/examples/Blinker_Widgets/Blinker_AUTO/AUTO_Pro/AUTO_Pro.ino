/* *****************************************************************
 *
 * Download latest Blinker library here:
 * https://github.com/blinker-iot/blinker-library/archive/master.zip
 * 
 * 
 * Blinker is a cross-hardware, cross-platform solution for the IoT. 
 * It provides APP, device and server support, 
 * and uses public cloud services for data transmission and storage.
 * It can be used in smart home, data monitoring and other fields 
 * to help users build Internet of Things projects better and faster.
 * 
 * Make sure installed 2.7.4 or later ESP8266/Arduino package,
 * if use ESP8266 with Blinker.
 * https://github.com/esp8266/Arduino/releases
 * 
 * Make sure installed 1.0.5 or later ESP32/Arduino package,
 * if use ESP32 with Blinker.
 * https://github.com/espressif/arduino-esp32/releases
 * 
 * Docs: https://diandeng.tech/doc
 *       
 * 
 * *****************************************************************
 * 
 * Blinker 库下载地址:
 * https://github.com/blinker-iot/blinker-library/archive/master.zip
 * 
 * Blinker 是一套跨硬件、跨平台的物联网解决方案，提供APP端、设备端、
 * 服务器端支持，使用公有云服务进行数据传输存储。可用于智能家居、
 * 数据监测等领域，可以帮助用户更好更快地搭建物联网项目。
 * 
 * 如果使用 ESP8266 接入 Blinker,
 * 请确保安装了 2.7.4 或更新的 ESP8266/Arduino 支持包。
 * https://github.com/esp8266/Arduino/releases
 * 
 * 如果使用 ESP32 接入 Blinker,
 * 请确保安装了 1.0.5 或更新的 ESP32/Arduino 支持包。
 * https://github.com/espressif/arduino-esp32/releases
 * 
 * 文档: https://diandeng.tech/doc
 *       
 * 
 * *****************************************************************/

#define BLINKER_PRO_ESP
#define BLINKER_BUTTON
#if defined(ESP32)
    #define BLINKER_BUTTON_PIN 4
#else
    #define BLINKER_BUTTON_PIN D7
#endif

#define BLINKER_OTA_VERSION_CODE "0.1.1"

#include <Blinker.h>

char type[] = "Your Device Type";
char auth[] = "Your Device Secret Key";

BlinkerButton Button1("btn-abc");
BlinkerNumber Number1("num-abc");

int counter = 0;

void button1_callback(const String & state)
{
    BLINKER_LOG("get button state: ", state);
    digitalWrite(LED_BUILTIN, !digitalRead(LED_BUILTIN));
}

/* 
 * Add your command parse code in this function
 * 
 * When get a command and device not parsed this command, device will call this function
 */
bool dataParse(const JsonObject & data)
{
    String getData;

    serializeJson(data, getData);
    
    BLINKER_LOG("Get user command: ", getData);

    // if you parsed this data, return TRUE.
    // return true;
    return false;
}

/* 
 * Add your heartbeat message detail in this function
 * 
 * When get heartbeat command {"get": "state"}, device will call this function
 * For example, you can print message back
 * 
 * Every 30s will get a heartbeat command from app 
 */
void heartbeat()
{
    BLINKER_LOG("heartbeat!");
}

#if defined(BLINKER_BUTTON)
/* 
 * Blinker provide a button parse function for user if you defined BLINKER_BUTTON
 * 
 * Blinker button can detect singal click/ double click/ long press
 * 
 * Blinker.tick() will run by default, use interrupt will be better
 */
ICACHE_RAM_ATTR void buttonTick()
{
    Blinker.tick();
}

/* 
 * Add your code in this function
 * 
 * When button clicked, device will call this function
 */
void singleClick()
{
    BLINKER_LOG("Button clicked!");
}

/* 
 * Add your code in this function
 * 
 * When button double clicked, device will call this function
 */
void doubleClick()
{
    BLINKER_LOG("Button double clicked!");
}
#endif

void dataRead(const String & data)
{
    BLINKER_LOG("Blinker readString: ", data);

    counter++;
    Number1.print(counter);
}

void setup()
{
    Serial.begin(115200);
    BLINKER_DEBUG.stream(Serial);
    BLINKER_DEBUG.debugAll();

    pinMode(LED_BUILTIN, OUTPUT);
    digitalWrite(LED_BUILTIN, LOW);

    Blinker.begin(auth, type);
    Blinker.attachData(dataRead);
    Blinker.attachParse(dataParse);
    Blinker.attachHeartbeat(heartbeat);
    
    Button1.attach(button1_callback);

#if defined(BLINKER_BUTTON)
    Blinker.attachClick(singleClick);
    Blinker.attachDoubleClick(doubleClick);    
    attachInterrupt(BLINKER_BUTTON_PIN, buttonTick, CHANGE);
#endif
}

uint32_t run_time = millis();

uint8_t humis[10] = {19,18,17,18,21,20,19,18,17,16};
uint8_t humi_num = 0;

void auto_check()
{
    if (millis() - run_time >= 10000)
    {
        run_time = millis();

        Blinker.autoInput("num-abc", humis[humi_num]);
        Blinker.autoRun();

        humi_num = (humi_num + 1) % 10;
    }
}

void loop()
{
    Blinker.run();
    auto_check();
}