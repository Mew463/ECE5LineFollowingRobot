#define LED_PIN     52
#define NUM_LEDS    15
#define BRIGHTNESS  100
#define LED_TYPE    WS2811
#define COLOR_ORDER GRB
CRGB leds[NUM_LEDS];
uint8_t hue = 0; // Maxes hue at 255
#define UPDATES_PER_SECOND 100

DEFINE_GRADIENT_PALETTE( Sunset_Real_gp ) {
  0, 120,  0,  0,
  22, 179, 22,  0,
  51, 255, 104,  0,
  85, 167, 22, 18,
  135, 100,  0, 103,
  198,  16,  0, 130,
  255,   0,  0, 160
};

DEFINE_GRADIENT_PALETTE( Fall_Off_Board ) {
  0, 0, 0, 0,
  68, 255, 0, 0,
  142, 0, 0, 0,
  255, 255, 0, 0
};


CRGBPalette16 currentpal = Sunset_Real_gp;

