#include <Adafruit_MCP4728.h>
#include <Wire.h>
// For QtPy QWIIC connector, use &Wire1

//#include <esp_now.h>

#include <Adafruit_NeoPixel.h>
// How many internal neopixels do we have? some boards have more than one!
#define NUMPIXELS        1
Adafruit_NeoPixel pixels(NUMPIXELS, PIN_NEOPIXEL, NEO_GRB + NEO_KHZ800);

uint32_t colors[] = {0xFF0000, 0x00FF00, 0x0000FF};
uint32_t color_ind = 0;

Adafruit_MCP4728 mcp;

int16_t SinArray[4096];

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  delay(2000);

#if defined(NEOPIXEL_POWER)
  // If this board has a power control pin, we must set it to output and high
  // in order to enable the NeoPixels. We put this in an #if defined so it can
  // be reused for other boards without compilation errors
  pinMode(NEOPIXEL_POWER, OUTPUT);
  digitalWrite(NEOPIXEL_POWER, HIGH);
#endif

  pixels.begin(); // INITIALIZE NeoPixel strip object (REQUIRED)
  pixels.setBrightness(20); // not so bright
  // set color to red
  pixels.fill(0xFF0000);
  pixels.show();

 delay(1000); // wait half a second

  // Try to initialize!
  if (!mcp.begin(MCP4728_I2CADDR_DEFAULT, &Wire1)) {
    Serial.println("Failed to find MCP4728 chip");
    while (1) {
      delay(10);
    }
  }

  // set color to green
  pixels.fill(0x00FF00);
  pixels.show();
  delay(1000); // wait half a second

  // set color to blue
  pixels.fill(0x0000FF);
  pixels.show();
  delay(1000); // wait half a second

  mcp.setChannelValue(MCP4728_CHANNEL_A, 4095);
  mcp.setChannelValue(MCP4728_CHANNEL_B, 2048);
  mcp.setChannelValue(MCP4728_CHANNEL_C, 1024);
  mcp.setChannelValue(MCP4728_CHANNEL_D, 0);

  for (int n=0; n<4096; n++)
  {
    SinArray[n] = int16_t((sin(TWO_PI * n / 4095.0) * (2048 - 1)) + 2048);
  }
  
}

void loop() {
  // put your main code here, to run repeatedly:
  static int16_t Voltage = 0;

//  mcp.setChannelValue(MCP4728_CHANNEL_A, Voltage);

  mcp.setChannelValue(MCP4728_CHANNEL_A, SinArray[Voltage]);

  Voltage = (Voltage + 4) & 0xfff;
  
}
