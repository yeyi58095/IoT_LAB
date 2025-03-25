//-------------------------------------------------------------------//
// Warning：
// SysTick_Handler() and SysTick_Config() should exist at the same time
// or your compiled device would encounter fatal error and dead.
//---------------------------------------------------------------------//
#include <bluefruit.h>
#define BUF_LENGTH          20
#define DEVICE_NAME       "TriBLE_1"
#define TICK_INTERVAL_us    2000
#define test_s_uuid 0xFF01
#define test_uuid 0xAA01


static uint8_t adc_value[BUF_LENGTH] = {0,};
static int counter = 0;
static bool connected2 = false;

BLEDis  bledis;    // DIS (Device Information Service) helper class instance
BLEService        hs(test_s_uuid);
BLECharacteristic hsraw(test_uuid);

extern "C"
{
  void SysTick_Handler(void)
  {
     if (connected2){
      adc_value[counter] = analogRead(A5);
      //Serial.println(adc_value[counter]);
      counter++;
      if (counter >= BUF_LENGTH) {
        hsraw.notify(adc_value, BUF_LENGTH);
        counter = 0;
      }
    }
  }
} // extern C

void setup()
{
  pinMode(LED_BUILTIN, OUTPUT);
  //Serial.begin(115200);
  //Serial.println("Bluefruit52 nRF Blinky Example");
  //Serial.println("------------------------------\n");
  analogReadResolution(8);

  //Serial.println("Initialise the Bluefruit nRF52 module");
  Bluefruit.configPrphBandwidth(BANDWIDTH_MAX);  
  Bluefruit.begin();
  Bluefruit.setName(DEVICE_NAME);
  Bluefruit.setTxPower(4);

  Bluefruit.Periph.setConnectCallback(connect_callback);
  Bluefruit.Periph.setDisconnectCallback(disconnect_callback);
  Bluefruit.Periph.setConnInterval(6, 12);

  //Serial.println("Configuring the Device Information Service");
  bledis.setManufacturer("Yutech, Taiwan");
  bledis.setModel("TriAnswer");
  bledis.begin();

  hs.begin();
  //1
  hsraw.setProperties(CHR_PROPS_READ | CHR_PROPS_NOTIFY);
  hsraw.setPermission(SECMODE_OPEN, SECMODE_NO_ACCESS);
  hsraw.setMaxLen(BUF_LENGTH);
  hsraw.setFixedLen(BUF_LENGTH);
  hsraw.begin();
  //Serial.println("Setting up the advertising");
  startAdv();
  SysTick_Config( (F_CPU/1000000)*TICK_INTERVAL_us );
}

void startAdv(void)
{
  // Advertising packet
  Bluefruit.Advertising.addFlags(BLE_GAP_ADV_FLAGS_LE_ONLY_GENERAL_DISC_MODE);
  Bluefruit.Advertising.addTxPower();
  Bluefruit.Advertising.addService(hs);
  Bluefruit.ScanResponse.addName();
  Bluefruit.Advertising.setInterval(32, 244);    // in unit of 0.625 ms
  Bluefruit.Advertising.setFastTimeout(30);      // number of seconds in fast mode
  Bluefruit.Advertising.start(0);                // 0 = Don't stop advertising after n seconds  
}

void loop()
{
  //Do nothing
}

void connect_callback(uint16_t conn_handle)
{
  BLEConnection* conn = Bluefruit.Connection(conn_handle);
  //Serial.println("Connected");
  delay(1000);
  connected2 = true;
}

void disconnect_callback(uint16_t conn_handle, uint8_t reason)
{
  (void) conn_handle;
  (void) reason;
  connected2 = false;
}
