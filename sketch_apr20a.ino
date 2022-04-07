#include <Wire.h>
#include <Adafruit_RGBLCDShield.h>
#include <utility/Adafruit_MCP23017.h>

Adafruit_RGBLCDShield lcd = Adafruit_RGBLCDShield();

byte arrowRight[] = { B00000, B00100, B00010, B11111, B11111, B00010, B00100, B00000 };
byte dot[] = { B00000, B00000, B00000, B01110, B01110, B00000, B00000, B00000 };
byte editt[] = { B00000, B00000, B01010, B11010, B01010, B01011, B01010, B00000 };
//enum screens {menuS, firstFloorS, groundFloorS, bedroom1S, bedroom2S, bathroomS, kitchenS, hallS, livingRoomS};

#ifdef __arm__
// should use uinstd.h to define sbrk but Due causes a conflict
extern "C" char* sbrk(int incr);
#else // __ARM__
extern char *__brkval;
#endif // __arm__
int freeMemory() {
  char top;
#ifdef __arm__
  return &top - reinterpret_cast<char*>(sbrk(0));
#elif defined(CORE_TEENSY) || (ARDUINO > 103 && ARDUINO != 151)
  return &top - __brkval;
#else // __arm__
  return __brkval ? &top - __brkval : &top - __malloc_heap_start;
#endif // __arm__
}

enum menu {firstFloor, groundFloor, bedroom1, bedroom2, bathroom, exitSel, lamp, heat, exitRoom, brightness, onOff, exitLamp, temperature, onOffHeat, exitHeat, kitchen, hall, livingRoom, exitGround, returnVal, lights} mainMenuEnum;
enum brit {level, exitBrit} bright;
enum onOf {on, off, timeState, exitOnOff} onOffTime;
String groundFlo[4] = {"kitchen", "hall", "living room", "exit"};
//"menu", "firstFloor", "groundFloor", "bedroom1", "bedroom2", "bathroom", "kitchen", "hall", "livingRoom", "lamp", "heat", "brightness", "OnOff", "temperature", "onOffHeat"
//int screens[15] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14};
String bedroom[4] = {"light", "lamp", "heat", "exit"};
String mainMenu[3] = {"First Floor", "Ground Floor", "Return Values"};
String lampS[3] = {"brightness", "On/Off", "exit"};
String warmth[3] = {"Temperature", "On/Off", "exit"};

String firstFloorAr[4] = {"Bedroom 1", "Bedroom 2", "Bathroom", "exit"};

struct light {
  unsigned int onOffTimeMin : 6;
  unsigned int onOffTimeHour : 5;
  const char* On;
  const char* location;
  unsigned int brightness : 7;
  const char* lampName;
  const char* floorLevel;
};

light *lamps = (light*) malloc(sizeof(light) * 6);
light *lightss = (light*) malloc(sizeof(light) * 6);

struct heats {
  unsigned int onOffTimeMin : 6;
  unsigned int onOffTimeHour : 5;
  const char* On;
  const char* location;
  unsigned int temperature : 7;
  const char* lampName;
  const char* floorLevel;
};

heats *heaters = (heats*) malloc(sizeof(heats) * 6);

byte selectedd = 0;
byte lampFrom = 0;
bool lightDominant = false;
bool edit = false;



void returnAllVals() {
  Serial.println("-----------------LIGHTS ~ BRIGHTNESS-----------------");
  for (byte i = 0; i < 6; ++i) {
    Serial.print(lightss[i].floorLevel);
    Serial.print("/");
    Serial.print(lightss[i].location);
    Serial.print("/light/main/Level");
    Serial.print(":");
    Serial.println(lightss[i].brightness);
  }
  Serial.println("--------------------LIGHTS ~ ON/OFF--------------------");
  for (byte i = 0; i < 6; ++i) {
    if (lightss[i].onOffTimeHour >= 10 && lightss[i].onOffTimeMin < 10) {
      Serial.print(lightss[i].floorLevel);
      Serial.print("/");
      Serial.print(lightss[i].location);
      Serial.print("/light/main/");
      Serial.print(lightss[i].On);
      Serial.print(":");
      Serial.print(lightss[i].onOffTimeHour);
      Serial.print(".");
      Serial.print("0");
      Serial.println(lightss[i].onOffTimeMin);
    }
    else if (lightss[i].onOffTimeHour < 10 && lightss[i].onOffTimeMin >= 10) {
      Serial.print(lightss[i].floorLevel);
      Serial.print("/");
      Serial.print(lightss[i].location);
      Serial.print("/light/main/");
      Serial.print(lightss[i].On);
      Serial.print(":");
      Serial.print("0");
      Serial.print(lightss[i].onOffTimeHour);
      Serial.print(".");
      Serial.println(lightss[i].onOffTimeMin);
    }
    else if (lightss[i].onOffTimeHour >= 10 && lightss[i].onOffTimeMin >= 10) {
      Serial.print(lightss[i].floorLevel);
      Serial.print("/");
      Serial.print(lightss[i].location);
      Serial.print("/light/main/");
      Serial.print(lightss[i].On);
      Serial.print(":");
      Serial.print(lightss[i].onOffTimeHour);
      Serial.print(".");
      Serial.println(lightss[i].onOffTimeMin);
    }
    else if (lightss[i].onOffTimeHour < 10 && lightss[i].onOffTimeMin < 10) {
      Serial.print(lightss[i].floorLevel);
      Serial.print("/");
      Serial.print(lightss[i].location);
      Serial.print("/light/main/");
      Serial.print(lightss[i].On);
      Serial.print(":");
      Serial.print("0");
      Serial.print(lightss[i].onOffTimeHour);
      Serial.print(".");
      Serial.print("0");
      Serial.println(lightss[i].onOffTimeMin);
    }
  }
  Serial.println("-----------------HEATERS ~ TEMPERATURE-----------------");
  for (byte i = 0; i < 6; ++i) {
    Serial.print(heaters[i].floorLevel);
    Serial.print("/");
    Serial.print(heaters[i].location);
    Serial.print("/heat/main/Level");
    Serial.print(":");
    Serial.println(heaters[i].temperature);
  }
  Serial.println("--------------------HEATERS ~ ON/OFF--------------------");
  for (byte i = 0; i < 6; ++i) {

    if (heaters[i].onOffTimeHour >= 10 && heaters[i].onOffTimeMin < 10) {
      Serial.print(heaters[i].floorLevel);
      Serial.print("/");
      Serial.print(heaters[i].location);
      Serial.print("/heat/main/");
      Serial.print(heaters[i].On);
      Serial.print(":");
      Serial.print(heaters[i].onOffTimeHour);
      Serial.print(".");
      Serial.print("0");
      Serial.println(heaters[i].onOffTimeMin);
    }
    else if (heaters[i].onOffTimeHour < 10 && heaters[i].onOffTimeMin >= 10) {
      Serial.print(heaters[i].floorLevel);
      Serial.print("/");
      Serial.print(heaters[i].location);
      Serial.print("/heat/main/");
      Serial.print(heaters[i].On);
      Serial.print(":");
      Serial.print("0");
      Serial.print(heaters[i].onOffTimeHour);
      Serial.print(".");
      Serial.println(heaters[i].onOffTimeMin);
    }
    else if (heaters[i].onOffTimeHour >= 10 && heaters[i].onOffTimeMin >= 10) {
      Serial.print(heaters[i].floorLevel);
      Serial.print("/");
      Serial.print(heaters[i].location);
      Serial.print("/heat/main/");
      Serial.print(heaters[i].On);
      Serial.print(":");
      Serial.print(heaters[i].onOffTimeHour);
      Serial.print(".");
      Serial.println(heaters[i].onOffTimeMin);
    }
    else if (heaters[i].onOffTimeHour < 10 && heaters[i].onOffTimeMin < 10) {
      Serial.print(heaters[i].floorLevel);
      Serial.print("/");
      Serial.print(heaters[i].location);
      Serial.print("/heat/main/");
      Serial.print(heaters[i].On);
      Serial.print(":");
      Serial.print("0");
      Serial.print(heaters[i].onOffTimeHour);
      Serial.print(".");
      Serial.print("0");
      Serial.println(heaters[i].onOffTimeMin);
    }
  }
  Serial.println("-----------------LAMPS ~ BRIGHTNESS-----------------");
  for (byte i = 0; i < 6; ++i) {
    Serial.print(lamps[i].floorLevel);
    Serial.print("/");
    Serial.print(lamps[i].location);
    Serial.print("/lamp/");
    Serial.print(lamps[i].lampName);
    Serial.print("/Level");
    Serial.print(":");
    Serial.println(lamps[i].brightness);
  }
  Serial.println("-------------------LAMPS ~ ON/OFF-------------------");
  for (byte i = 0; i < 6; ++i) {
    if (lamps[i].onOffTimeHour >= 10 && lamps[i].onOffTimeMin < 10) {
      Serial.print(lamps[i].floorLevel);
      Serial.print("/");
      Serial.print(lamps[i].location);
      Serial.print("/lamp/");
      Serial.print(lamps[i].lampName);
      Serial.print("/");
      Serial.print(lamps[i].On);
      Serial.print(":");
      Serial.print(lamps[i].onOffTimeHour);
      Serial.print(".");
      Serial.print("0");
      Serial.println(lamps[i].onOffTimeMin);
    }
    else if (lamps[i].onOffTimeHour < 10 && lamps[i].onOffTimeMin >= 10) {
      Serial.print(lamps[i].floorLevel);
      Serial.print("/");
      Serial.print(lamps[i].location);
      Serial.print("/lamp/");
      Serial.print(lamps[i].lampName);
      Serial.print("/");
      Serial.print(lamps[i].On);
      Serial.print(":");
      Serial.print("0");
      Serial.print(lamps[i].onOffTimeHour);
      Serial.print(".");
      Serial.println(lamps[i].onOffTimeMin);
    }
    else if (lamps[i].onOffTimeHour >= 10 && lamps[i].onOffTimeMin >= 10) {
      Serial.print(lamps[i].floorLevel);
      Serial.print("/");
      Serial.print(lamps[i].location);
      Serial.print("/lamp/");
      Serial.print(lamps[i].lampName);
      Serial.print("/");
      Serial.print(lamps[i].On);
      Serial.print(":");
      Serial.print(lamps[i].onOffTimeHour);
      Serial.print(".");
      Serial.println(lamps[i].onOffTimeMin);
    }
    else if (lamps[i].onOffTimeHour < 10 && lamps[i].onOffTimeMin < 10) {
      Serial.print(lamps[i].floorLevel);
      Serial.print("/");
      Serial.print(lamps[i].location);
      Serial.print("/lamp/");
      Serial.print(lamps[i].lampName);
      Serial.print("/");
      Serial.print(lamps[i].On);
      Serial.print(":");
      Serial.print("0");
      Serial.print(lamps[i].onOffTimeHour);
      Serial.print(".");
      Serial.print("0");
      Serial.println(lamps[i].onOffTimeMin);
    }
  }
}

void drawRoomOps(int changes) {
  switch (mainMenuEnum) {
    case lights:
      if (changes & BUTTON_DOWN) {
        mainMenuEnum = lamp;
      }
      else if (changes & BUTTON_SELECT) {
        selectedd = 9;
        mainMenuEnum = brightness;
        lightDominant = true;
      }
      break;
    case lamp:
      if (changes & BUTTON_DOWN) {
        mainMenuEnum = heat;
      }
      else if (changes & BUTTON_SELECT) {
        selectedd = 9;
        mainMenuEnum = brightness;
        lightDominant = false;
      }
      else if (changes & BUTTON_UP) {
        mainMenuEnum = lights;
      }
      break;
    case heat:
      if (changes & BUTTON_DOWN) {
        mainMenuEnum = exitRoom;
      }
      else if (changes & BUTTON_UP) {
        mainMenuEnum = lamp;
      }
      else if (changes & BUTTON_SELECT) {
        mainMenuEnum = temperature;
        selectedd = 10;
      }
      break;
    case exitRoom:
      if (changes & BUTTON_SELECT) {
        switch (lampFrom) {
          case 0:
          case 1:
          case 2:
            selectedd = 1;
            mainMenuEnum = bedroom1;
            break;
          case 3:
          case 4:
          case 5:
            selectedd = 2;
            mainMenuEnum = kitchen;
            break;
        }

      }
      else if (changes & BUTTON_UP) {
        mainMenuEnum = heat;
      }
      break;
  }
}

void renderTime(byte type, byte where) {
  switch (type) {
    case 0:
      if (heaters[lampFrom].onOffTimeHour >= 10 && heaters[lampFrom].onOffTimeMin < 10) {
        lcd.setCursor(1, where);
        lcd.print(" ");
        lcd.setCursor(8, where);
        lcd.print(String(heaters[lampFrom].onOffTimeHour) + ":" + "0" + String(heaters[lampFrom].onOffTimeMin));

      }
      else if (heaters[lampFrom].onOffTimeHour >= 10 && heaters[lampFrom].onOffTimeMin >= 10) {
        lcd.setCursor(1, where);
        lcd.print(" ");
        lcd.setCursor(8, where);
        lcd.print(String(heaters[lampFrom].onOffTimeHour) + ":" + String(heaters[lampFrom].onOffTimeMin));
      }
      else if (heaters[lampFrom].onOffTimeHour < 10 && heaters[lampFrom].onOffTimeMin >= 10) {
        lcd.setCursor(1, where);
        lcd.print(" ");
        lcd.setCursor(8, where);
        lcd.print("0" + String(heaters[lampFrom].onOffTimeHour) + ":" + String(heaters[lampFrom].onOffTimeMin));
      }

      else if (heaters[lampFrom].onOffTimeHour < 10 && heaters[lampFrom].onOffTimeMin < 10) {
        lcd.setCursor(1, where);
        lcd.print(" ");
        lcd.setCursor(8, where);
        lcd.print("0" + String(heaters[lampFrom].onOffTimeHour) + ":" + "0" + String(heaters[lampFrom].onOffTimeMin));
      }
      break;
    case 1:
      if (lamps[lampFrom].onOffTimeHour >= 10 && lamps[lampFrom].onOffTimeMin < 10) {
        lcd.setCursor(1, where);
        lcd.print(" ");
        lcd.setCursor(8, where);
        lcd.print(String(lamps[lampFrom].onOffTimeHour) + ":" + "0" + String(lamps[lampFrom].onOffTimeMin));

      }
      else if (lamps[lampFrom].onOffTimeHour >= 10 && lamps[lampFrom].onOffTimeMin >= 10) {
        lcd.setCursor(1, where);
        lcd.print(" ");
        lcd.setCursor(8, where);
        lcd.print(String(lamps[lampFrom].onOffTimeHour) + ":" + String(lamps[lampFrom].onOffTimeMin));
      }
      else if (lamps[lampFrom].onOffTimeHour < 10 && lamps[lampFrom].onOffTimeMin >= 10) {
        lcd.setCursor(1, where);
        lcd.print(" ");
        lcd.setCursor(8, where);
        lcd.print("0" + String(lamps[lampFrom].onOffTimeHour) + ":" + String(lamps[lampFrom].onOffTimeMin));
      }

      else if (lamps[lampFrom].onOffTimeHour < 10 && lamps[lampFrom].onOffTimeMin < 10) {
        lcd.setCursor(1, where);
        lcd.print(" ");
        lcd.setCursor(8, where);
        lcd.print("0" + String(lamps[lampFrom].onOffTimeHour) + ":" + "0" + String(lamps[lampFrom].onOffTimeMin));
      }
      break;
    case 2:
      if (lightss[lampFrom].onOffTimeHour >= 10 && lightss[lampFrom].onOffTimeMin < 10) {
        lcd.setCursor(1, where);
        lcd.print(" ");
        lcd.setCursor(8, where);
        lcd.print(String(lightss[lampFrom].onOffTimeHour) + ":" + "0" + String(lightss[lampFrom].onOffTimeMin));

      }
      else if (lightss[lampFrom].onOffTimeHour >= 10 && lightss[lampFrom].onOffTimeMin >= 10) {
        lcd.setCursor(1, where);
        lcd.print(" ");
        lcd.setCursor(8, where);
        lcd.print(String(lightss[lampFrom].onOffTimeHour) + ":" + String(lightss[lampFrom].onOffTimeMin));
      }
      else if (lightss[lampFrom].onOffTimeHour < 10 && lightss[lampFrom].onOffTimeMin >= 10) {
        lcd.setCursor(1, where);
        lcd.print(" ");
        lcd.setCursor(8, where);
        lcd.print("0" + String(lightss[lampFrom].onOffTimeHour) + ":" + String(lightss[lampFrom].onOffTimeMin));
      }

      else if (lightss[lampFrom].onOffTimeHour < 10 && lightss[lampFrom].onOffTimeMin < 10) {
        lcd.setCursor(1, where);
        lcd.print(" ");
        lcd.setCursor(8, where);
        lcd.print("0" + String(lightss[lampFrom].onOffTimeHour) + ":" + "0" + String(lightss[lampFrom].onOffTimeMin));
      }
      break;
  }
  lcd.setCursor(2, where);
  lcd.print("Time: ");
}

void renderScreen(String menuList[], byte changes, byte sizeAr) {
  static int selected = 0;
  lcd.setCursor(0, 0);
  lcd.write(0);
  if (changes & BUTTON_DOWN) {
    selected += 1;
  }
  else if (changes & BUTTON_UP) {
    selected -= 1;
  }
  if (selected >= sizeAr) {
    selected = (sizeAr) - 1;
  }
  for (int i = 0; i < sizeAr; i++) {
    if (i == selected) {
      lcd.setCursor(2, 0);
      lcd.print(menuList[i] + "       ");

      if ((i + 1) >= sizeAr) {
        lcd.setCursor(2, 1);
        lcd.print("              ");
      }
      else if ((i + 1) < sizeAr) {
        lcd.setCursor(3, 1);
        lcd.print(menuList[i + 1] + "         ");
      }
    }
  }
  if (selected <= 0 || (changes & BUTTON_SELECT)) {
    selected = 0;
  }
}

void screenSelect(int selected, int changes) {
  //  for (int i = 0; i < (sizeof(screens) / sizeof(String)); i++) {
  //    if (selectedScreen == screens[i]) {
  //      selected = screens[i];
  //    }
  //  }
  if (selected == 0) {
    renderScreen(mainMenu, changes, sizeof(mainMenu) / sizeof(String));
    switch (mainMenuEnum) {
      case firstFloor:
        if (changes & BUTTON_SELECT) {
          selectedd = 1;
        }
        else if (changes & BUTTON_DOWN) {
          mainMenuEnum = groundFloor;
        }
        break;

      case groundFloor:
        if (changes & BUTTON_SELECT) {
          selectedd = 2;
          mainMenuEnum = kitchen;
        }

        else if (changes & BUTTON_UP) {
          mainMenuEnum = firstFloor;
        }
        else if (changes & BUTTON_DOWN) {
          mainMenuEnum = returnVal;
        }
        break;
      case returnVal:
        if (changes & BUTTON_UP) {
          mainMenuEnum = groundFloor;

        }
        else if (changes & BUTTON_SELECT) {
          returnAllVals();
          mainMenuEnum = firstFloor;
        }
        break;
    }
  }
  else if (selected == 1) {
    renderScreen(firstFloorAr, changes, sizeof(firstFloorAr) / sizeof(String));
    if (mainMenuEnum < 2) {
      mainMenuEnum = bedroom1;
    }
    switch (mainMenuEnum) {
      case bedroom1:
        if (changes & BUTTON_SELECT) {
          selectedd = 3;
          mainMenuEnum = lights;
          lampFrom = 0;
        }
        else if (changes & BUTTON_DOWN) {
          mainMenuEnum = bedroom2;
        }
        break;
      case bedroom2:
        if (changes & BUTTON_SELECT) {
          selectedd = 4;
          mainMenuEnum = lights;
          lampFrom = 1;
        }
        else if (changes & BUTTON_UP) {
          mainMenuEnum = bedroom1;
        }
        else if (changes & BUTTON_DOWN) {
          mainMenuEnum = bathroom;
        }
        break;
      case bathroom:
        if (changes & BUTTON_SELECT) {
          selectedd = 5;
          mainMenuEnum = lights;
          lampFrom = 2;
        }
        else if (changes & BUTTON_UP) {
          mainMenuEnum = bedroom2;
        }
        else if (changes & BUTTON_DOWN) {
          mainMenuEnum = exitSel;
        }
        break;
      case exitSel:
        if (changes & BUTTON_SELECT) {
          selectedd = 0;
          mainMenuEnum = firstFloor;
        }
        else if (changes & BUTTON_UP) {
          mainMenuEnum = bathroom;
        }
        break;
    }
  }
  else if (selected == 3) {
    renderScreen(bedroom, changes, sizeof(bedroom) / sizeof(String));
    //    switch (mainMenuEnum) {
    //      case lamp:
    //        if (changes & BUTTON_DOWN) {
    //          mainMenuEnum = heat;
    //        }
    //        else if (changes & BUTTON_SELECT) {
    //          selectedd = 9;
    //          mainMenuEnum = brightness;
    //        }
    //        break;
    //      case heat:
    //        if (changes & BUTTON_DOWN) {
    //          mainMenuEnum = exitRoom;
    //        }
    //        else if (changes & BUTTON_UP) {
    //          mainMenuEnum = lamp;
    //        }
    //        else if (changes & BUTTON_SELECT) {
    //          mainMenuEnum = temperature;
    //          selectedd = 10;
    //        }
    //        break;
    //      case exitRoom:
    //        if (changes & BUTTON_SELECT) {
    //          switch (lampFrom) {
    //            case 0:
    //            case 1:
    //            case 2:
    //              selectedd = 1;
    //              mainMenuEnum = bedroom1;
    //              break;
    //            case 3:
    //            case 4:
    //            case 5:
    //              selectedd = 2;
    //              mainMenuEnum = kitchen;
    //              break;
    //          }
    //
    //        }
    //        else if (changes & BUTTON_UP) {
    //          mainMenuEnum = heat;
    //        }
    //        break;
    //    }
    drawRoomOps(changes);
  }
  else if (selected == 4) {
    renderScreen(bedroom, changes, sizeof(bedroom) / sizeof(String));
    //    switch (mainMenuEnum) {
    //      case lamp:
    //        if (changes & BUTTON_DOWN) {
    //          mainMenuEnum = heat;
    //        }
    //        else if (changes & BUTTON_SELECT) {
    //          selectedd = 9;
    //          mainMenuEnum = brightness;
    //        }
    //        break;
    //      case heat:
    //        if (changes & BUTTON_DOWN) {
    //          mainMenuEnum = exitRoom;
    //        }
    //        else if (changes & BUTTON_UP) {
    //          mainMenuEnum = lamp;
    //        }
    //        break;
    //      case exitRoom:
    //        if (changes & BUTTON_SELECT) {
    //          selectedd = 1;
    //          mainMenuEnum = bedroom1;
    //        }
    //        else if (changes & BUTTON_UP) {
    //          mainMenuEnum = heat;
    //        }
    //        break;
    //    }
    drawRoomOps(changes);
  }
  else if (selected == 5) {
    renderScreen(bedroom, changes, sizeof(bedroom) / sizeof(String));
    //    switch (mainMenuEnum) {
    //      case lamp:
    //        if (changes & BUTTON_DOWN) {
    //          mainMenuEnum = heat;
    //        }
    //        else if (changes & BUTTON_SELECT) {
    //          selectedd = 9;
    //          mainMenuEnum = brightness;
    //        }
    //        break;
    //      case heat:
    //        if (changes & BUTTON_DOWN) {
    //          mainMenuEnum = exitRoom;
    //        }
    //        else if (changes & BUTTON_UP) {
    //          mainMenuEnum = lamp;
    //        }
    //        break;
    //      case exitRoom:
    //        if (changes & BUTTON_SELECT) {
    //          selectedd = 1;
    //          mainMenuEnum = firstFloor;
    //        }
    //        else if (changes & BUTTON_UP) {
    //          mainMenuEnum = heat;
    //        }
    //        break;
    //    }
    drawRoomOps(changes);
  }
  else if (selected == 9) {

    renderScreen(lampS, changes, sizeof(lampS) / sizeof(String));
    switch (mainMenuEnum) {
      case brightness:
        if (changes & BUTTON_DOWN) {
          mainMenuEnum = onOff;
        }
        else if (changes & BUTTON_SELECT) {
          selectedd = 11;
        }
        break;
      case onOff:
        if (changes & BUTTON_DOWN) {
          mainMenuEnum = exitLamp;
        }
        else if (changes & BUTTON_UP) {
          mainMenuEnum = brightness;
        }
        else if (changes & BUTTON_SELECT) {
          selectedd = 12;
        }
        break;
      case exitLamp:
        if (changes & BUTTON_SELECT) {
          selectedd = 3;
          mainMenuEnum = lights;

        }
        else if (changes & BUTTON_UP) {
          mainMenuEnum = onOff;
        }
        break;
    }
  }
  else if (selected == 11) {
    switch (bright) {
      case level:
        if (changes & BUTTON_DOWN) {
          bright = exitBrit;
        }
        else if (lightDominant) {
          if (lcd.readButtons() & BUTTON_RIGHT && (lightss[lampFrom].brightness < 100)) {
            lightss[lampFrom].brightness += 1;
          }
          else if (lcd.readButtons() & BUTTON_LEFT && (lightss[lampFrom].brightness > 0)) {
            lightss[lampFrom].brightness -= 1;
          }
          lcd.setCursor(2, 0);
          lcd.print("level: " + String(lightss[lampFrom].brightness) + "  ");
          lcd.setCursor(2, 1);
          lcd.print("exit       ");
        }
        else {
          if (lcd.readButtons() & BUTTON_RIGHT && (lamps[lampFrom].brightness < 100)) {
            lamps[lampFrom].brightness += 1;
          }
          else if (lcd.readButtons() & BUTTON_LEFT && (lamps[lampFrom].brightness > 0)) {
            lamps[lampFrom].brightness -= 1;
          }
          lcd.setCursor(2, 0);
          lcd.print("level: " + String(lamps[lampFrom].brightness) + "  ");
          lcd.setCursor(2, 1);
          lcd.print("exit       ");
        }
        break;
      case exitBrit:
        if (changes & BUTTON_SELECT) {
          selectedd = 9;
          mainMenuEnum = brightness;
          bright = level;
        }
        else if (changes & BUTTON_UP) {
          bright = level;
        }
        lcd.setCursor(2, 0);
        lcd.print("exit      ");
        lcd.setCursor(2, 1);
        lcd.print("     ");
        break;
    }
  }
  else if (selected == 12) {
    switch (onOffTime) {
      case on:
        if (changes & BUTTON_RIGHT) {
          onOffTime = off;
        }
        else if (changes & BUTTON_SELECT) {
          lcd.setCursor(1, 0);
          lcd.write(1);
          lcd.setCursor(13, 0);
          lcd.print(" ");
          lcd.setCursor(2, 0);
          lcd.print("On       ");
          lamps[lampFrom].On = "on";
          Serial.println(lamps[lampFrom].On);

        }
        else if (changes & BUTTON_DOWN) {
          onOffTime = timeState;
        }
        lcd.setCursor(2, 0);
        lcd.print("On      ");
        lcd.setCursor(10, 0);
        lcd.print("Off");
        lcd.setCursor(0, 0);
        lcd.write(0);
        lcd.setCursor(8, 0);
        lcd.print(" ");
        lcd.setCursor(2, 1);
        lcd.print("Time: ");
        if (lightDominant) {
          renderTime(2, 1);
        }
        else {
          renderTime(1, 1);
        }
        break;
      case off:
        if (changes & BUTTON_LEFT) {
          onOffTime = on;
        }
        else if (changes & BUTTON_SELECT) {
          lcd.setCursor(1, 0);
          lcd.print(" ");
          lcd.setCursor(13, 0);
          lcd.write(1);
          lamps[lampFrom].On = "Off";
          Serial.println(lamps[lampFrom].On);
        }
        else if (changes & BUTTON_DOWN) {
          onOffTime = timeState;
        }
        lcd.setCursor(2, 0);
        lcd.print("On    ");
        lcd.setCursor(10, 0);
        lcd.print("Off");
        lcd.setCursor(8, 0);
        lcd.write(0);
        lcd.setCursor(0, 0);
        lcd.print(" ");
        lcd.setCursor(2, 1);
        lcd.print("Time: ");
        if (lightDominant) {
          renderTime(2, 1);
        }
        else {
          renderTime(1, 1);
        }
        break;
      case timeState:
        if (changes & BUTTON_UP && !edit) {
          onOffTime = on;
        }
        else if (changes & BUTTON_SELECT && (edit == false)) {
          edit = true;
        }
        else if (lightDominant && edit) {
          if (lcd.readButtons() & BUTTON_RIGHT && lightss[lampFrom].onOffTimeMin < 59) {
            lightss[lampFrom].onOffTimeMin += 1;
          }
          else if (lcd.readButtons() & BUTTON_LEFT && lightss[lampFrom].onOffTimeMin > 0) {
            lightss[lampFrom].onOffTimeMin -= 1;
          }
          else if (lcd.readButtons() & BUTTON_UP &&  lightss[lampFrom].onOffTimeHour < 23) {
            lightss[lampFrom].onOffTimeHour += 1;
          }
          else if (lcd.readButtons() & BUTTON_DOWN &&  lightss[lampFrom].onOffTimeHour > 0) {
            lightss[lampFrom].onOffTimeHour -= 1;
          }
          if (changes & BUTTON_SELECT) {
            edit = false;
          }
        }
        else if (!lightDominant && edit) {
          if (lcd.readButtons() & BUTTON_RIGHT && lamps[lampFrom].onOffTimeMin < 59) {
            lamps[lampFrom].onOffTimeMin += 1;
          }
          else if (lcd.readButtons() & BUTTON_LEFT && lamps[lampFrom].onOffTimeMin > 0) {
            lamps[lampFrom].onOffTimeMin -= 1;
          }
          else if (lcd.readButtons() & BUTTON_UP &&  lamps[lampFrom].onOffTimeHour < 23) {
            lamps[lampFrom].onOffTimeHour += 1;
          }
          else if (lcd.readButtons() & BUTTON_DOWN &&  lamps[lampFrom].onOffTimeHour > 0) {
            lamps[lampFrom].onOffTimeHour -= 1;
          }
          if (changes & BUTTON_SELECT) {
            edit = false;
          }
        }
        else if (changes & BUTTON_DOWN && !edit) {
          onOffTime = exitOnOff;
        }
        if (lightDominant) {
          renderTime(2, 0);
        }
        else {
          renderTime(1, 0);
        }
        if (edit) {
          lcd.setCursor(0, 0);
          lcd.write(2);
        }
        else {
          lcd.setCursor(0, 0);
          lcd.write(0);
        }

        lcd.setCursor(13, 0);
        lcd.print(" ");
        lcd.setCursor(2, 1);
        lcd.print("exit       ");
        break;
      case exitOnOff:
        if (changes & BUTTON_SELECT) {
          selectedd = 9;
          mainMenuEnum = brightness;
          onOffTime = on;
        }
        else if (changes & BUTTON_UP) {
          onOffTime = timeState;
        }
        lcd.setCursor(2, 0);
        lcd.print("exit       ");
        lcd.setCursor(2, 1);
        lcd.print("      ");
        break;
    }
  }
  else if (selected == 10) {
    renderScreen(warmth, changes, sizeof(warmth) / sizeof(String));
    switch (mainMenuEnum) {
      case temperature:
        if (changes & BUTTON_DOWN) {
          mainMenuEnum = onOffHeat;
        }
        else if (changes & BUTTON_SELECT) {
          selectedd = 13;
          bright = level;
          Serial.println("clicked");
          Serial.println(selectedd);
        }
        break;
      case onOffHeat:
        if (changes & BUTTON_DOWN) {
          mainMenuEnum = exitHeat;
        }
        else if (changes & BUTTON_UP) {
          mainMenuEnum = temperature;
        }
        else if (changes & BUTTON_SELECT) {
          selectedd = 14;
          onOffTime = on;
        }
        break;
      case exitHeat:
        if (changes & BUTTON_UP) {
          mainMenuEnum = onOffHeat;
        }
        else if (changes & BUTTON_SELECT) {
          mainMenuEnum = lights;
          selectedd = 3;
        }
        break;
    }

  }
  else if (selected == 13) {
    switch (bright) {
      case level:
        if (changes & BUTTON_DOWN) {
          bright = exitBrit;
        }
        else if (lcd.readButtons() & BUTTON_RIGHT && (heaters[lampFrom].temperature < 100)) {
          heaters[lampFrom].temperature += 1;
        }
        else if (lcd.readButtons() & BUTTON_LEFT && (heaters[lampFrom].temperature > 0)) {
          heaters[lampFrom].temperature -= 1;
        }
        lcd.setCursor(2, 0);
        lcd.print("level: " + String(heaters[lampFrom].temperature) + "   ");
        lcd.setCursor(2, 1);
        lcd.print("exit       ");
        break;
      case exitBrit:
        if (changes & BUTTON_SELECT) {
          selectedd = 10;
          mainMenuEnum = temperature;
          bright = level;
          Serial.print(heaters[lampFrom].floorLevel);
          Serial.print("/");
          Serial.print(heaters[lampFrom].location);
          Serial.print("/heat/main/Level");
          Serial.print(":");
          Serial.println(heaters[lampFrom].temperature);
        }
        else if (changes & BUTTON_UP) {
          bright = level;
        }
        lcd.setCursor(2, 0);
        lcd.print("exit      ");
        lcd.setCursor(2, 1);
        lcd.print("     ");
        break;
    }
  }
  else if (selected == 14) {
    switch (onOffTime) {
      case on:
        if (changes & BUTTON_RIGHT) {
          onOffTime = off;
        }
        else if (changes & BUTTON_SELECT) {
          lcd.setCursor(1, 0);
          lcd.write(1);
          lcd.setCursor(13, 0);
          lcd.print(" ");
          lcd.setCursor(2, 0);
          lcd.print("On       ");
          heaters[lampFrom].On = "on";
          Serial.println(heaters[lampFrom].On);

        }
        else if (changes & BUTTON_DOWN) {
          onOffTime = timeState;
        }
        lcd.setCursor(2, 0);
        lcd.print("On      ");
        lcd.setCursor(10, 0);
        lcd.print("Off");
        lcd.setCursor(0, 0);
        lcd.write(0);
        lcd.setCursor(8, 0);
        lcd.print(" ");
        renderTime(0, 1);
        break;
      case off:
        if (changes & BUTTON_LEFT) {
          onOffTime = on;
        }
        else if (changes & BUTTON_SELECT) {
          lcd.setCursor(1, 0);
          lcd.print(" ");
          lcd.setCursor(13, 0);
          lcd.write(1);
          heaters[lampFrom].On = "Off";
          Serial.println(heaters[lampFrom].On);
        }
        else if (changes & BUTTON_DOWN) {
          onOffTime = timeState;
        }
        lcd.setCursor(2, 0);
        lcd.print("On    ");
        lcd.setCursor(10, 0);
        lcd.print("Off");
        lcd.setCursor(8, 0);
        lcd.write(0);
        lcd.setCursor(0, 0);
        lcd.print(" ");
        renderTime(0, 1);
        break;
      case timeState:
        if (changes & BUTTON_UP && !edit) {
          onOffTime = on;
        }
        else if (changes & BUTTON_SELECT && (edit == false)) {
          edit = true;
        }

        else if (edit) {
          if (lcd.readButtons() & BUTTON_RIGHT && heaters[lampFrom].onOffTimeMin < 59) {
            heaters[lampFrom].onOffTimeMin += 1;
          }
          else if (lcd.readButtons() & BUTTON_LEFT && heaters[lampFrom].onOffTimeMin > 0) {
            heaters[lampFrom].onOffTimeMin -= 1;
          }
          else if (lcd.readButtons() & BUTTON_UP &&  heaters[lampFrom].onOffTimeHour < 23) {
            heaters[lampFrom].onOffTimeHour += 1;
          }
          else if (lcd.readButtons() & BUTTON_DOWN &&  heaters[lampFrom].onOffTimeHour > 0) {
            heaters[lampFrom].onOffTimeHour -= 1;
          }
          if (changes & BUTTON_SELECT) {
            edit = false;
          }
        }
        else if (changes & BUTTON_DOWN && !edit) {
          onOffTime = exitOnOff;
        }
        renderTime(0, 0);
        if (edit) {
          lcd.setCursor(0, 0);
          lcd.write(2);
        }
        else {
          lcd.setCursor(0, 0);
          lcd.write(0);
        }

        lcd.setCursor(13, 0);
        lcd.print(" ");
        lcd.setCursor(2, 1);
        lcd.print("exit        ");
        break;
      case exitOnOff:
        if (changes & BUTTON_SELECT) {
          selectedd = 10;
          mainMenuEnum = temperature;
          onOffTime = on;
        }
        else if (changes & BUTTON_UP) {
          onOffTime = timeState;
        }
        lcd.setCursor(2, 0);
        lcd.print("exit       ");
        lcd.setCursor(2, 1);
        lcd.print("      ");
        break;
    }
  }
  else if (selected == 2) {
    renderScreen(groundFlo, changes, sizeof(groundFlo) / sizeof(String));
    switch (mainMenuEnum) {
      case kitchen:
        if (changes & BUTTON_DOWN) {
          mainMenuEnum = hall;
        }
        else if (changes & BUTTON_SELECT) {
          mainMenuEnum = lights;
          selectedd = 3;
          lampFrom = 3;
        }
        break;
      case hall:
        if (changes & BUTTON_DOWN) {
          mainMenuEnum = livingRoom;
        }
        else if (changes & BUTTON_UP) {
          mainMenuEnum = kitchen;
        }
        else if (changes & BUTTON_SELECT) {
          mainMenuEnum = lights;
          selectedd = 3;
          lampFrom = 4;
        }
        break;
      case livingRoom:
        if (changes & BUTTON_DOWN) {
          mainMenuEnum = exitGround;
        }
        else if (changes & BUTTON_UP) {
          mainMenuEnum = hall;
        }
        else if (changes & BUTTON_SELECT) {
          mainMenuEnum = lights;
          selectedd = 3;
          lampFrom = 5;
        }
        break;
      case exitGround:
        if (changes & BUTTON_SELECT) {
          selectedd = 0;
          mainMenuEnum = firstFloor;
        }
        else if (changes & BUTTON_UP) {
          mainMenuEnum = livingRoom;
        }
        break;
    }
  }
  //  else if (selected == 15) {
  //    renderScreen(lampS, changes, sizeof(lampS) / sizeof(String));
  //    switch (mainMenuEnum) {
  //      case brightness:
  //        if (changes & BUTTON_DOWN) {
  //          mainMenuEnum = onOff;
  //        }
  //        else if (changes & BUTTON_SELECT) {
  //          selectedd = 11;
  //        }
  //        break;
  //      case onOff:
  //        if (changes & BUTTON_DOWN) {
  //          mainMenuEnum = exitLamp;
  //        }
  //        else if (changes & BUTTON_UP) {
  //          mainMenuEnum = brightness;
  //        }
  //        else if (changes & BUTTON_SELECT) {
  //          selectedd = 12;
  //        }
  //        break;
  //      case exitLamp:
  //        if (changes & BUTTON_SELECT) {
  //          selectedd = 3;
  //          mainMenuEnum = lights;
  //
  //        }
  //        else if (changes & BUTTON_UP) {
  //          mainMenuEnum = onOff;
  //        }
  //        break;
  //    }
  //
  //  }
}

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  delay(2000);
  lcd.begin(16, 2);
  lcd.createChar(0, arrowRight);
  lcd.createChar(1, dot);
  lcd.createChar(2, editt);
  for (byte i = 0; i < 6; ++i) {
    lightss[i].onOffTimeHour = 0;
    lightss[i].onOffTimeMin = 0;
    lightss[i].brightness = 0;
    lamps[i].onOffTimeHour = 0;
    lamps[i].onOffTimeMin = 0;
    lamps[i].brightness = 0;
    heaters[i].onOffTimeHour = 0;
    heaters[i].onOffTimeMin = 0;
    heaters[i].temperature = 0;
    lightss[i].On = "off";
    lamps[i].On = "off";
    heaters[i].On = "off";

    if ( i < 3) {
      lightss[i].floorLevel = "First";
      lamps[i].floorLevel = "First";
      heaters[i].floorLevel = "First";
    }
    else {
      lightss[i].floorLevel = "Ground";
      lamps[i].floorLevel = "Ground";
      heaters[i].floorLevel = "Ground";
    }
    switch (i) {
      case 0:
        lightss[i].location = "bedroom1";
        heaters[i].location = "bedroom1";
        lamps[i].location = "bedroom1";
        lamps[i].lampName = "Table";
        break;
      case 1:
        lightss[i].location = "bedroom2";
        heaters[i].location = "bedroom2";
        lamps[i].location = "bedroom2";
        lamps[i].lampName = "bed";
        break;
      case 2:
        lightss[i].location = "bathroom";
        heaters[i].location = "bathroom";
        lamps[i].location = "bathroom";
        lamps[i].lampName = "Window";
        break;
      case 3:
        lightss[i].location = "kitchen";
        heaters[i].location = "kitchen";
        lamps[i].location = "kitchen";
        lamps[i].lampName = "Table";
        break;
      case 4:
        lightss[i].location = "hall";
        heaters[i].location = "hall";
        lamps[i].location = "hall";
        lamps[i].lampName = "Torch";
        break;
      case 5:
        lightss[i].location = "livingRoom";
        heaters[i].location = "livingRoom";
        lamps[i].location = "livingRoom";
        lamps[i].lampName = "Table";
        break;
    }
  }
  Serial.println("ENHANCED:");
}

void loop() {
  //  Serial.println(freeMemory());
  // put your main code here, to run repeatedly:
  static int oldBut = lcd.readButtons();
  int but = lcd.readButtons();
  int changes = oldBut & ~but;

  screenSelect(selectedd, changes);
  if (Serial.available() > 0) {
    String query = Serial.readString();
    if (query == "Q\n") {
      returnAllVals();
    }
    else if (query == "M\n") {
      Serial.println(freeMemory());
    }
  }
  oldBut = but;
}
