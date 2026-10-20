#include <SPI.h>
#include <SD.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>

LiquidCrystal_I2C lcd(0x27, 16, 2);

const int chipSelect = 10;
File dataFile;


uint8_t frameBuffer[256];

void setup() {
  Wire.begin();
  Wire.setClock(400000);

  lcd.init();
  lcd.backlight();
  lcd.clear();

  if (!SD.begin(chipSelect)) {
    lcd.setCursor(0, 0);
    lcd.print("SD Init Fail!");
    while (1);
  }

  dataFile = SD.open("BADAPPLE.BIN");
  if (!dataFile) {
    lcd.setCursor(0, 0);
    lcd.print("File Not Found!");
    while (1);
  }


  uint8_t levels[8][8] = {
    { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 }, 
    { 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10 }, 
    { 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18 }, 
    { 0x1C, 0x1C, 0x1C, 0x1C, 0x1C, 0x1C, 0x1C, 0x1C },
    { 0x1E, 0x1E, 0x1E, 0x1E, 0x1E, 0x1E, 0x1E, 0x1E },
    { 0x1F, 0x1F, 0x1F, 0x1F, 0x1F, 0x1F, 0x1F, 0x1F },
    { 0x1F, 0x1F, 0x1F, 0x1F, 0x1F, 0x1F, 0x1F, 0x1F },
    { 0x1F, 0x1F, 0x1F, 0x1F, 0x1F, 0x1F, 0x1F, 0x1F }  
  };

  for (int i = 0; i < 8; i++) {
    lcd.createChar(i, levels[i]);
  }
  lcd.clear();
}

void loop() {
  if (dataFile.available() >= 256) {
    dataFile.read(frameBuffer, 256);


    for (int row = 0; row < 2; row++) {
      lcd.setCursor(0, row);
      for (int col = 0; col < 16; col++) {

        int sum = 0;
        int blockIndex = (row * 16 + col);
        
        for (int line = 0; line < 8; line++) {
          uint8_t b = frameBuffer[blockIndex * 8 + line];
          for (int bit = 0; bit < 5; bit++) {
            if (b & (1 << (4 - bit))) {
              sum++;
            }
          }
        }


        int charIndex = map(sum, 0, 40, 0, 5);
        lcd.write((uint8_t)charIndex);
      }
    }

    delay(20);

  } else {
    dataFile.seek(0);
  }
}