// This example shows how a custom VGA resolution can be created for one of the base modes
// You need to connect a VGA screen cable to the pins specified below.
// cc by-sa 4.0 license
// bitluni

// including the needed header
#include <ESP32Lib.h>
#include <Ressources/CodePage437_8x8.h>
#include <Ressources/CodePage437_8x14.h>
#include <Ressources/CodePage437_8x16.h>
#include <Ressources/CodePage437_8x19.h>
#include <Ressources/CodePage437_9x16.h>
#include <Ressources/Font6x8.h>
#include <Ressources/Font8x8.h>

// VGA Device using an interrupt to unpack the pixels from 4bit to 16bit for the I²S
// This takes some CPU time in the background but is able to fit a frame buffer in the memory
VGA6Bit vga;
const PinConfig &pinConfig = VGA6Bit::Pico3bVGA;

void FontTest()
{
  static int cual = 0;
  static unsigned long T = 0;
  if (millis() > T)
  {

    T = millis() + 4000;
    cual++;
    if (cual == 5)
      cual = 0;
  }

  if (cual == 0)
  { // selecting the font
    vga.setFont(Font6x8);
    // set color
    // vga.setTextColor(vga.RGB(255, 0, 0), vga.RGB(0, 0, 255));
    // displaying the character set
    vga.println("Font6x8");
    for (int i = 0; i < 256; i++)
      vga.print((char)i);
  }
  else if (cual == 1)
  {
    vga.setFont(CodePage437_8x8);
    // vga.setTextColor(vga.RGB(0, 255, 0), vga.RGB(255, 0, 0));
    vga.println("CodePage437_8x8");
    for (int i = 0; i < 256; i++)
      vga.print((char)i);
  }
  else if (cual == 2)
  {
    vga.setFont(CodePage437_8x14);
    // vga.setTextColor(vga.RGB(0, 0, 255), vga.RGB(0, 255, 0));
    vga.println("CodePage437_8x14");
    for (int i = 0; i < 256; i++)
      vga.print((char)i);
  }
  else if (cual == 3)
  {
    vga.setFont(CodePage437_8x16);
    // vga.setTextColor(vga.RGB(255, 255, 0), vga.RGB(0, 255, 255));
    vga.println("CodePage437_8x16");
    for (int i = 0; i < 256; i++)
      vga.print((char)i);
  }
  else if (cual == 3)
  {
    vga.setFont(CodePage437_8x19);
    // vga.setTextColor(vga.RGB(255, 0, 255), vga.RGB(255, 255, 0));
    vga.println("CodePage437_8x19");
    for (int i = 0; i < 256; i++)
      vga.print((char)i);
  }
  else if (cual == 4)
  {
    vga.setFont(CodePage437_9x16);
    // vga.setTextColor(vga.RGB(0, 255, 255), vga.RGB(255, 0, 255));
    vga.println("CodePage437_9x16");
    for (int i = 0; i < 256; i++)
      vga.print((char)i);
  }
}

void setup()
{
  Serial.begin(115200);
  // enabling double buffering
  vga.setFrameBufferCount(2);

  Mode MiMonitor(8, 54, 28, 360, 11, 2, 32, 480, 2, 14161000, 1, 0);
  // MiMonitor.print<HardwareSerial>(Serial);
  vga.init(MiMonitor, pinConfig);
  // vga.init(vga.MODE320x240, pinConfig); // ok, muy bien
  // vga.init(vga.MODE360x200, pinConfig);
  vga.setFont(Font8x8);
}

/// draws a bouncing balls
void balls()
{
  // some basic gravity physics
  static VGA6Bit::Color c[4] = {vga.RGB(0, 255, 0), vga.RGB(0, 255, 255), vga.RGB(255, 0, 255), vga.RGB(255, 255, 0)};
  static float y[4] = {20, 20, 20, 20};
  static float x[4] = {20, 20, 20, 20};
  static float vx[4] = {.01, -0.07, .05, -.03};
  static float vy[4] = {0, 1, 2, 3};
  static unsigned long lastT = 0;
  unsigned long t = millis();
  float dt = (t - lastT) * 0.001f;
  lastT = t;
  const int r = 6;
  for (int i = 0; i < 4; i++)
  {
    int rx = r;
    int ry = r;
    vy[i] += -9.81f * dt * 100;
    x[i] += vx[i];
    y[i] += vy[i] * dt;
    // check for boundaries and bounce back
    if (y[i] < r && vy[i] < 0)
    {
      vy[i] = 200 + i * 10;
      ry = y[i];
    }
    if (x[i] < r && vx[i] < 0)
    {
      vx[i] = -vx[i];
      rx = x[i];
    }
    if (x[i] >= vga.xres - r && vx[i] > 0)
    {
      vx[i] = -vx[i];
      rx = vga.xres - x[i];
    }
    // draw a filled ellipse
    vga.fillEllipse(x[i], vga.yres - y[i] - 1, rx, ry, c[i]);
    vga.ellipse(x[i], vga.yres - y[i] - 1, rx, ry, 0);
  }
}

// mainloop
void loop()
{
  // draw a background
  for (int y = 0; y * 10 < vga.yres; y++)
    for (int x = 0; x * 10 < vga.xres; x++)
      vga.fillRect(x * 10, y * 10, 10, 10, (x + y) & 1 ? vga.RGB(255, 0, 0) : vga.RGB(255, 255, 255));

  // text position
  vga.setCursor(2, 2);
  // black text color no background color
  vga.setTextColor(vga.RGB(0));
  // show the remaining memory
  vga.print(vga.xres);
  vga.print("X");
  vga.println(vga.yres);
  vga.print("FREE MEMORY: ");
  vga.println((int)heap_caps_get_free_size(MALLOC_CAP_DEFAULT));

  FontTest();

  // draw bouncing balls
  balls();
  // show the backbuffer (only needed when using backbuffering)
  vga.show();
}