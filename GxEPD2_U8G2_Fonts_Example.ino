#define ENABLE_GxEPD2_GFX 0

#include <iostream>
#include <GxEPD2_BW.h>
#include <GxEPD2_3C.h>
#include <GxEPD2_7C.h>
#include <Fonts/FreeMonoBold9pt7b.h>
#include <U8g2_for_Adafruit_GFX.h>
#include <WiFi.h>

// select the display class and display driver class in the following file (new style):
#include "GxEPD2_display_selection_new_style.h"

U8G2_FOR_ADAFRUIT_GFX u8g2Fonts;
WiFiClient wifiClient;

// wifi config
const char *ssid = "ssid";
const char *password = "xxxxxxxx";

// stock api
const char *targetHost = "qt.gtimg.cn";  //  腾讯股票
String targetUrl = "/utf8/q=s_sh000001"; //  上证指数

const char *streamId = "....................";
const char *privateKey = "....................";

const int targetHttpPort = 80;
int loopCount = 0;

void setup()
{
  Serial.begin(115200);
  Serial.println();

  display.init();
  u8g2Fonts.begin(display); // connect u8g2 procedures to Adafruit GFX

  Serial.println();
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());

  Serial.println("init screen and font");
  display.setFullWindow();
  display.setRotation(0);
  u8g2Fonts.setFontMode(1);
  u8g2Fonts.setFontDirection(0);
  u8g2Fonts.setForegroundColor(GxEPD_BLACK);
  u8g2Fonts.setBackgroundColor(GxEPD_WHITE);
  showFWString("ae-coreink-stock", u8g2_font_t0_16b_tf, 10, 60)

      WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(".");
  }
}

void loop()
{
  delay(10000);
  ++loopCount;

  Serial.print("LOOP ===> ");
  Serial.println(loopCount);

  Serial.print("CONNECTING TO: ");
  Serial.println(targetHost);

  if (!wifiClient.connect(targetHost, targetHttpPort))
  {
    Serial.println("CONNECTION FAILED");
    return;
  }

  Serial.print("Requesting URL: ");
  Serial.println(targetUrl);

  wifiClient.print(String("GET ") + targetUrl + " HTTP/1.1\r\n" +
                   "Host: " + targetHost + "\r\n" +
                   "Connection: close\r\n\r\n");
  unsigned long timeout = millis();
  while (wifiClient.available() == 0)
  {
    if (millis() - timeout > 5000)
    {
      Serial.println("!!! CONNECTION TIMEOUT !!!");
      wifiClient.stop();
      return;
    }
  }

  while (wifiClient.available())
  {
    String line = wifiClient.readStringUntil('\r');
    int pos = line.indexOf("v_s_");
    int idx = 0;
    String fragment;
    String fCache3;
    String symbol1;
    if (pos >= 0)
    {
      while ((pos = line.indexOf("~")) >= 0)
      {
        fragment = line.substring(0, pos);
        if (idx == 1 && loopCount == 1)
        {
          showPWString(fragment, u8g2_font_wqy16_t_gb2312a, 10, 20, 32, 8);
        }
        if (idx == 2 && loopCount == 1)
        {
          showPWString(fragment, u8g2_font_t0_16b_tf, 10, 52, 32, 8);
        }
        if (idx == 3)
        {
          fCache3 = fragment;
        }
        if (idx == 4)
        {
          symbol1 = fragment.toFloat() >= 0 ? "+" : "-";
          showPWString(fCache3 + " " + symbol1 + fragment, u8g2_font_t0_16b_tf, 10, 84, 32, 8);
        }
        if (idx == 5)
        {
          showPWString(symbol1 + fragment + "%", u8g2_font_fub30_tf, 10, 116, 46, 8);
        }
        Serial.print(idx);
        Serial.print(": ");
        Serial.println(fragment);
        line = line.substring(fragment.length() + 1);
        idx++;
      }
    }
  }
}

void showFWString(String str, const uint8_t *font, int x, int y)
{
  display.setFullWindow();
  u8g2Fonts.setFont(font);
  display.firstPage();
  do
  {
    display.fillScreen(GxEPD_WHITE);
    if (str.length() > 0)
    {
      u8g2Fonts.setCursor(x, y);
      u8g2Fonts.println(str);
    }
  } while (display.nextPage());
}

void showPWString(String str, const uint8_t *font, int x, int y, int height, int paddingBottom)
{
  display.setPartialWindow(x, y, display.width(), height);
  u8g2Fonts.setFont(font);
  display.firstPage();
  do
  {
    display.fillScreen(GxEPD_WHITE);
    u8g2Fonts.setCursor(x, y + height - paddingBottom);
    u8g2Fonts.println(str);
  } while (display.nextPage());
}
