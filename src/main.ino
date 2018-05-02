#include <Arduino.h>
#include <Adafruit_NeoPixel.h>

int                 BEEP_PIN         = 11;
const unsigned long BEEP_DURATION_MS = 50;
/**
 * 262 C
 * 294 D
 * 330 E
 * 349 F
 * 392 G
 * 440 A
 * 494 B
 */
const unsigned int   BEEP_FREQUENCY   = 1396;

const int NEO_PIXEL_PIN     = 12;
const int NUM_OF_NEO_PIXELS = 1;
const int NEO_PIXEL_STOCK_0 = 0;
Adafruit_NeoPixel pixels    = Adafruit_NeoPixel(NUM_OF_NEO_PIXELS,
                                                NEO_PIXEL_PIN,
                                                NEO_GRB + NEO_KHZ800);

const int BUTTON_PIN        = 13;
const int DETECTED_BUTTON_DELAY_MS = 300;

const int MINIMUM_DELAY_MS  = 100;


enum Mode {
    E_MODE_NO,
    E_MODE_60,
    E_MODE_70,
    E_MODE_80,
    E_MODE_90,
    E_MODE_100,
    E_MODE_110,
    E_MODE_120,
};

Mode nextMode(Mode mode)
{
    if (mode >= E_MODE_120) {
        return E_MODE_NO;
    }
    return Mode(mode + 1);
}

int getInterval(Mode mode)
{
    switch (mode) {
    case E_MODE_60:  return 60000 / 60;
    case E_MODE_70:  return 60000 / 70;
    case E_MODE_80:  return 60000 / 80;
    case E_MODE_90:  return 60000 / 90;
    case E_MODE_100: return 60000 / 100;
    case E_MODE_110: return 60000 / 110;
    case E_MODE_120: return 60000 / 120;
    default:         return 1000;
    }
}

uint32_t getColor(Mode mode)
{
    switch (mode) {
    case E_MODE_60:  return Adafruit_NeoPixel::Color(255, 0,   0);
    case E_MODE_70:  return Adafruit_NeoPixel::Color(0,   255, 0);
    case E_MODE_80:  return Adafruit_NeoPixel::Color(0,   0,   255);
    case E_MODE_90:  return Adafruit_NeoPixel::Color(255, 255, 0);
    case E_MODE_100: return Adafruit_NeoPixel::Color(255, 0,   255);
    case E_MODE_110: return Adafruit_NeoPixel::Color(0,   255, 255);
    case E_MODE_120: return Adafruit_NeoPixel::Color(255, 255, 255);
    default:         return Adafruit_NeoPixel::Color(0,   0,   0);
    }
}

void setup()
{
    Serial.begin(9600);

    pinMode(BUTTON_PIN, INPUT);

    pixels.begin();
    pixels.setBrightness(255);
    pixels.setPixelColor(0, Adafruit_NeoPixel::Color(0,0,0));
    pixels.show();

    Serial.println("Booted...");
}

void loop()
{
    static Mode mode = E_MODE_NO;
    static int  remainsInterval = 0;

    if (digitalRead(BUTTON_PIN) == LOW) {
        mode = nextMode(mode);
        Serial.print("change mode = ");
        Serial.print(mode);
        Serial.println("");
        delay(DETECTED_BUTTON_DELAY_MS);
        remainsInterval = 0;
        return;
    }

    if (mode == E_MODE_NO) {
        pixels.setPixelColor(0, Adafruit_NeoPixel::Color(0,0,0));
        pixels.show();
        delay(MINIMUM_DELAY_MS);
        return;
    }

    if (remainsInterval <= 0) {
        uint32_t color = getColor(mode);
        pixels.setPixelColor(0, color);
        pixels.show();
        tone(BEEP_PIN, BEEP_FREQUENCY, BEEP_DURATION_MS);
        remainsInterval = getInterval(mode);
    }
    if (remainsInterval > MINIMUM_DELAY_MS) {
        delay(MINIMUM_DELAY_MS);
        remainsInterval -= MINIMUM_DELAY_MS;
    } else {
        delay(remainsInterval);
        remainsInterval = 0;
    }
}