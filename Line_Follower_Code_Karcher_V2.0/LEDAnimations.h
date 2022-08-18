#define LED_PIN     52
#define NUM_LEDS    7
#define BRIGHTNESS  100
#define LED_TYPE    WS2811
#define COLOR_ORDER GRB
CRGB ledstrip[NUM_LEDS];
uint8_t hue = 0; // Maxes hue at 255
#define UPDATES_PER_SECOND 100
CRGBPalette16 currentpal;

/*You can use https://cssgradient.io/ to make your own color palettes!!

 0,         0,         0,         0
 ^          ^          ^          ^
 A * 2.55  Red       Green      Blue
(Position)
  
*/

DEFINE_GRADIENT_PALETTE( LineTracking_Animation ) {
  0, 120,  0,  0,
  22, 179, 22,  0,
  51, 255, 104,  0,
  85, 167, 22, 18,
  135, 100,  0, 103,
  198,  16,  0, 130,
  255,   0,  0, 160
};

DEFINE_GRADIENT_PALETTE (Error_Animation)
{
  0, 0, 0, 0,
  30*2.55, 255, 0, 0,
  68*2.55, 0, 0, 0,
  255, 0, 0, 0,
};

DEFINE_GRADIENT_PALETTE (Off_Board_Animation)
{
  0, 0, 0, 0,
  124, 0, 0, 0,
  125, 245, 152, 0,
  255, 255, 154, 0,
};

void LEDStripInit() {
  FastLED.addLeds<LED_TYPE, LED_PIN, COLOR_ORDER>(ledstrip, NUM_LEDS).setCorrection( TypicalLEDStrip );
  FastLED.setBrightness(  BRIGHTNESS );
  static uint8_t starthue = 0;
  for (int x = 0; x < 200; x++)
  {
    fill_rainbow( ledstrip, NUM_LEDS, ++starthue, 3);
    FastLED.show();
    delay(3);
  }
}

void updateLEDStrip() {
  fill_palette(ledstrip, NUM_LEDS, hue, 255 / NUM_LEDS, currentpal, 255, LINEARBLEND);
  EVERY_N_MILLISECONDS(10) hue += 1;
  FastLED.show();
}
