// This #include statement was automatically added by the Particle IDE.
#include "Adafruit_SSD1306.h"

#define ONE_DAY_MILLIS (24 * 60 * 60 * 1000)
#define ONE_MINUTE_MILLIS (60 * 1000)
#define OLED_I2C_ADDRESS 0x3C
#define OLED_RESET D4

Adafruit_SSD1306 display(OLED_RESET);

#if (SSD1306_LCDHEIGHT != 64)
#error("Height incorrect, please fix Adafruit_SSD1306.h!");
#endif

struct StateObject {
  char tz[3];
  char status[100];
};

StateObject state;

unsigned long lastSync = millis();
unsigned long lastTimeUpdate = millis();

void setup()   {
    EEPROM.get(0, state);
    Serial.begin(9600);
    Particle.syncTime();


    // by default, we'll generate the high voltage from the 3.3v line internally! (neat!)
    display.begin(SSD1306_SWITCHCAPVCC, OLED_I2C_ADDRESS);  // initialize with the I2C addr  (for the 128x64)
    // init done
    display.clearDisplay();   // clears the screen and buffer
    display.display();

    // cloud functions: up to 4 per device.
    // function name: up to 12 characters
    Particle.function("SetStatus", SetStatus);
    Particle.function("SetTimeZone", SetTimeZone);
    Particle.function("Clear", Clear);

    String statusStr = state.status;
    String tzStr = state.tz;
    if (statusStr.length() > 0) {
      SetStatus(statusStr);
    } else {
      SetStatus("Unknown");
    }
    if (tzStr.length() > 0) {
      SetTimeZone(tzStr);
    } else {
      SetTimeZone("-7");
    }
}


void loop() {
  if (millis() - lastSync > ONE_DAY_MILLIS)
   {
       // Request time sync from Spark Cloud
       Particle.syncTime();
       lastSync = millis();
   }

   if (millis() - lastTimeUpdate > ONE_MINUTE_MILLIS)
    {
      UpdateTime();
      lastTimeUpdate = millis();
    }
}

void showMsg(int n, String message) {

    int fontSize = (message.length() > 10 ? 1 : 2);
    /*if (n == 2) {
      fontSize = 14;
    }*/
    display.setTextSize(fontSize);   // 1 = 8 pixel tall, 2 = 16 pixel tall...
    display.setTextColor(WHITE);
    display.setCursor(0, n*16);
    display.println(message);
    display.display();
}

void ClearMessage(int n) {
    display.fillRect(0, 16*n, 128, 16, 0);
    display.display();
}

int SetStatus(String message) {
    ClearMessage(0);
    Serial.print("Status: ");
    Serial.println(message);

    if (message.length() > 0) {
      showMsg(0, message);
    }
    message.toCharArray(state.status, 100);
    EEPROM.put(0, state);
    return 0;
}

int SetTimeZone(String tz) {
    tz.toCharArray(state.tz, 100);
    Serial.print("Timezone: ");
    Serial.println(tz);
    EEPROM.put(0, state);
    Time.zone(tz.toInt());
    UpdateTime();
    return 0;
}

void UpdateTime() {
  ClearMessage(2);
  showMsg(2, Time.format(Time.now(), "%I:%M%p"));
}

int Clear(String message) {
    display.clearDisplay();
    display.display();
    return 0;
}
