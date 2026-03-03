//////////////////////////////////////////////////////////////////////////////
// Sketch: LYMOS                                                           //
// Author: Перевезенцева Мария                                              //
//                                                                          //
//                                                                          //
// Comments:                                                                //
//    Программа для Arduino Mega  на котором релизован проект LYMOS        //
//    Умное управление светом в доме                                        //
//                                                                          //
//    Выпускной проект 9 класс                                              //
//////////////////////////////////////////////////////////////////////////////

// Подключение библиотек

//#include "SPI.h"
//#include "Adafruit_GFX.h"
#include <SPFD5408_Adafruit_GFX.h>     // Core graphics library - Графическая библиотека
#include <SPFD5408_Adafruit_TFTLCD.h>  // Hardware-specific library - Библиотека работы с экраном
#include <SPFD5408_TouchScreen.h>      // Touch library
#include <GRGB.h>                      // Библиотека RGB

// Calibrates value
#define SENSIBILITY 300
#define MINPRESSURE 10
#define MAXPRESSURE 1000

// TouchScreen Pin Номера ножек по которым идет обмен данными с экраном
#define YP A3  //A3
#define XM A2  //A2
#define YM 9   //9
#define XP 8   //8

short TS_MINX = 96;
short TS_MINY = 117;
short TS_MAXX = 932;
short TS_MAXY = 950;

// Init TouchScreen: Настройка работы с TouchScreen - ИНИЦИАЛИЗАЦИЯ
TouchScreen ts = TouchScreen(XP, YP, XM, YM, SENSIBILITY);
// LCD Pin Номера ножек по которым идет обмен данными с экраном
#define LCD_CS A3
#define LCD_CD A2
#define LCD_WR A1
#define LCD_RD A0
#define LCD_RESET A4  // Optional : otherwise connect to Arduino's reset pin

// Assign human-readable names to some common 16-bit color values:
//  Задание констант цветов для работы с экраном (текст - линии - фон)
#define BLACK 0x0000
#define BLUE 0x001F
#define DarBLUE 0x104B
#define RED 0xF800
#define GREEN 0x07E0
#define CYAN 0x07FF
#define MAGENTA 0xF81F
#define YELLOW 0xFFE0
#define WHITE 0xFFFF
#define S_WHITE 0XDEFB
#define S_SILVER 0x077F
#define SILVER 0XE7FF
#define S_GOLD 0XD343
#define GOLD 0XFFF3
#define S_BEIGE 0XFD8C
#define BEIGE 0XFFFE
#define GREY 0X7BCF
#define LILAC 0XC0FF
#define ORANGE 0xFC60
#define C_MAGENTA 0xF8BA
#define C_ORANGE 0xF9E0
// Init LCD: Настройка работы с экраном - ИНИЦИАЛИЗАЦИЯ'
Adafruit_TFTLCD tft(LCD_CS, LCD_CD, LCD_WR, LCD_RD, LCD_RESET);

// Настройка работы со СВЕТОДИОДОМ RGB - ИНИЦИАЛИЗАЦИЯ
// указать тип драйвера COMMON_CATHODE/COMMON_ANODE
// и пины в порядке      R,  G,  B
GRGB led(COMMON_ANODE, 44, 45, 46);

// ============================================================
// Переменные для программы
uint16_t buttons_y = 0;
uint16_t width = 0;
uint16_t height = 0;
int16_t Color = 0xFFFF, Brig = 255;
unsigned int Warmth = 3224;
int TCx, TCy, TCz;      // Координаты нажатия тачскрина
boolean TCpus = false;  // Признак нажатия экрана
int i, k, Timed;
int analogZoom = 15;  // номер порта к которому подключен датчик звука
int analogLum = 14;  // номер порта к которому подключен датчик освещённости
int val = 0;         // переменная для хранения считываемого значения
float Zvuk,Lum   ;
boolean Lumos = false;  // Переменная показывает состояние света
boolean Clap = false;  // Датчик звука - хлопок
boolean SClap = false;  // 
boolean Dmove = false;  // Датчк движения 

int Regim = 0;  // Режим работы - выполняемая функция
/* 0 - экран заставки
   1 - Начальное меню (Главное)
   2 - Заставка планирование дня
   3 - Управление светом  по плану дня
   4 - Меню настройки света
   5 - Меню Функций управления
   6 - Датчик звука
   7 - Датчик движения
   8 - Датчик освещенности
*/
// -------------------------------------------------------------
//  Установление флага рисования экрана для заданной функции
boolean Flag1 = true;
boolean Flag2 = true;
boolean Flag3 = true;
boolean Flag4 = true;
boolean Flag5 = true;
boolean Flag6 = true;
boolean Flag7 = true;
boolean Flag8 = true;
//===============================================================

//-- Setup
// ========================================
// |                                      |
// |        Запуск программы              |
// |                                      |
// |         Инициализация                |
// ========================================
void setup(void) {

  // Serial por for debug, not works if shield is plugged in arduino
  Serial.begin(38400);
  pinMode (47, INPUT);  // Датич звука  47 pin

  // Inicialize the controller
  tft.reset();
  tft.begin(0x9341);
  tft.setRotation(0);  // Need for the Mega, please changed for your choice or rotation initial
  width = tft.width() - 1;
  height = tft.height() - 1;
  // Debug
  Serial.println(F("TFT LCD test"));
  Serial.print("TFT size is ");
  Serial.print(tft.width());
  Serial.print("x");
  Serial.println(tft.height());

  // ++++++++++++++++++++++++++++++++++++
  // задаем параметры для работы со светодиодом
  // установить предустановленный цвет (16 бит)
  led.setColor(GWhite);    // список цветов в GRGB.h --- БЕЛЫЙ
  led.setBrightness(255);  // установить ОБЩУЮ яркость -- МАКСИМУМ
  led.setCRT(true);        // включить гамма-коррекцию (по умолч. выключено)
  led.disable();           // выключаем светодиод
    // ------------------------------------------------------------------------

  // ========================================
  // |                                      |
  // |   рисование экрана заставки          |
  // |                                      |
  // ========================================
  drawBorder();  // Рисуем бордюр начальной заставки (синяя рамка - белый фон)

  led.setColor(GREEN);  //Включаем зеленый светодиод на время начальной заставки
                        // led.setRGB(255,250,000);
//========================================================
//   работа с русским текстом
//==============================================================
  tft.cp437(true);



  tft.setCursor(55, 50);
  tft.setTextSize(3);
  tft.setTextColor(GREEN);
  tft.println(utf8rus("Проект"));

  tft.setTextColor(GREEN);
  tft.setTextSize(4);
  tft.setCursor(45, 90);
  tft.println("LYMOS");

  tft.setCursor(20, 150);
  tft.setTextSize(2);
  tft.setTextColor(BLACK);
  tft.println(utf8rus("Автор:"));

  tft.setCursor(55, 170);
  tft.setTextSize(2);
  tft.setTextColor(BLUE);
  tft.println(utf8rus("Мария"));

  tft.setCursor(55, 190);
  tft.setTextSize(2);
  tft.setTextColor(BLUE);
  tft.println(utf8rus("Перевезенцева"));

  tft.setCursor(30, 250);
  tft.setTextSize(1);
  tft.setTextColor(BLACK);
  tft.println(utf8rus("Для продолжения коснись экрана"));
  //===================================
  Regim = 0;  // Устанавливаем режим 0
  Serial.print("--- 0000 ----- ");
  Serial.println(Regim);

  //===================================================================
  //#############################################################
  //##   Regim =0   ##      Выполняется функция 0              ##
  //##---------------------------------------------------------##
  //##                                                         ##
  //##           НАЧАЛЬНАЯ  ЗАСТАВКА ЭКРАН                     ##
  //##                                                         ##
  //#############################################################
  //      Ожидаем нажатие на экран
  do {
    ReadTocScrin();  // опрос тачскрина
  } while (TCpus == false);
  //===================================================================
  // Произошло нажатие на экран
  //===================================================================
  Regim = 1;  // Устанавливаем режим 1   !!!!! для отладки режим 4
  Serial.print("--- Regim = ");
  Serial.println(Regim);
  led.setColor(Color);      // список цветов в GRGB.h --- БЕЛЫЙ
  led.setBrightness(Brig);  // установить ОБЩУЮ яркость -- МАКСИМУМ
  led.disable();            // выключаем светодиод
  //###################################################################
}

// ###########################################
// ##                                       ##
// ##                   Loop                ##
// ##      Основной цикл программы          ##
// ##                                       ##
// ###########################################
void loop() {
  // =====================================================
  ReadTocScrin();  // опрос тачскрина
  // =====================================================
  // Для отладки Вывод координат сенсора TSPoint p;
  // ======================================================
  /*
    if (TCpus)      {  // Проверка Нажатие тачскина
     // *** подбор координат ***
      Serial.print("X = "); Serial.print(TCx);
      Serial.print("\t    Y = "); Serial.println(TCy);
      //Serial.print("\t Pres= "); Serial.println(TCz);
    }
    */
  //========================================================

  switch (Regim) {
    case 1:
      //#############################################################
      //##   Regim =1   ##      Выполняется функция 1              ##
      //##---------------------------------------------------------##
      //##                                                         ##
      //##                 ГЛАВНОЕ     МЕНЮ                        ##
      //##                                                         ##
      //#############################################################
      if (Flag1) {  //==========================================================
        Flag2 = true;
        Flag3 = true;
        Flag4 = true;
        Flag5 = true;
        Flag6 = true;
        Flag7 = true;
        Flag8 = true;
        // Рисуем экран главного меню
        Serial.print("  --- 11111 ----------- ");
        Serial.println(Regim);
        //  int x1,y1,x2,y2,Dl,Vi,ra;
        //  x1=1; y1=2; x2=20; y2=20; Dl=10; Vi=10; ra=33;
        tft.fillScreen(BLACK);   // Красим экран в черный цвет
        tft.setTextColor(GOLD);  // Пишем название проекта
        tft.setTextSize(5);
        tft.setCursor(35, 35);
        tft.println("LYMOS");
        // ----------- клавиша ВКЛ --------------------
        tft.fillCircle(55, 140, 40, GREEN);  // Круглая кнопка с заливкой
        tft.drawCircle(55, 140, 40, WHITE);  // Круг - рамка
        tft.drawCircle(55, 140, 39, WHITE);  // Круг - рамка
  
        tft.setTextColor(BLACK);  // Включение выключение
        tft.setTextSize(2);
        tft.setCursor(40, 133);
        tft.println(utf8rus("ВКЛ"));
        // ----------- клавиша ВЫКЛ --------------------
        tft.fillCircle(185, 140, 40, RED);    // Круглая кнопка с заливкой
        tft.drawCircle(185, 140, 40, WHITE);  // Круг - рамка
        tft.drawCircle(185, 140, 39, WHITE);  // Круг - рамка
        //tft.fillRoundRect(155,120,60,40,8,RED); //  Кнопка с заливкой
        //tft.drawRoundRect(155,120,60,40,8,WHITE); //  Кнопка рамка
        tft.setTextColor(WHITE);
        tft.setTextSize(2);
        tft.setCursor(162, 133);
        tft.println(utf8rus("ВЫКЛ"));
        // ----------- меню Настройка света --------------------
        tft.fillRoundRect(20, 205, 200, 40, 8, WHITE);   //  Кнопка с заливкой
        tft.drawRoundRect(20, 205, 200, 40, 8, YELLOW);  //  Кнопка рамка
        tft.setTextColor(BLACK);                         //Настройка светадиода
        tft.setTextSize(2);
        tft.setCursor(30, 218);
        tft.println(utf8rus("Настройка света"));
        // ----------- меню Функции управления --------------------
        tft.fillRoundRect(5, 265, 230, 40, 8, WHITE);   //  Кнопка с заливкой
        tft.drawRoundRect(5, 265, 230, 40, 8, YELLOW);  //  Кнопка рамка
        tft.setTextColor(BLACK);                        //Настройка светадиода
        tft.setTextSize(2);
        tft.setCursor(10, 280);
        tft.println(utf8rus("Функции управления"));
        delay(500);
        Flag1 = false;  // Устанавливаем флаг - что экран нарисован (больше его не рисуем)
      }
      // -----------------------------------------------------------------------------

      if (TCpus) {  // Проверка Нажатие тачскина
        // --------- Проверка кнопки ВЫКЛ --------------------------------------------------------------------
        if (((TCx > 148) && (TCx < 227)) && ((TCy > 103) && (TCy < 184))) {  // Условие проверки нажатия кнопки ВЫКЛ
          tft.fillRoundRect(100, 5, 40, 15, 3, BLACK);                       //  Кнопка с заливкой
          led.disable();
          Serial.println("== VD -- OFF");
        }  // -----------------------------------------------------------------------------------
        // --------- Проверка кнопки ВКЛ --------------------------------------------------------------------
        if (((TCx > 16) && (TCx < 97)) && ((TCy > 103) && (TCy < 184))) {  // Условие проверки нажатия кнопки ВКЛ
          tft.fillRoundRect(100, 5, 40, 15, 3, WHITE);                     //  Кнопка с заливкой
          Serial.println(F("== VD ON --"));
          led.enable();
        }  // -----------------------------------------------------------------------------------
        // --------- Проверка кнопки Настройки Света --------------------------------------------------------------------
        if (((TCx > 20) && (TCx < 227)) && ((TCy > 205) && (TCy < 245))) {  // Условие проверки нажатия кнопки ВКЛ
          Serial.println(F("== VD ON --"));
          led.enable();
          Regim = 4;
          Flag1 = true;
        }  // -----------------------------------------------------------------------------------
        // --------- Проверка кнопки Функции управления --------------------------------------------------------------------
        if (((TCx > 5) && (TCx < 235)) && ((TCy > 265) && (TCy < 305))) {  // Условие проверки нажатия кнопки ВКЛ
          led.disable();
          Serial.println("== VD -- OFF");
          Regim = 5;
          Flag1 = true;
        }  // -----------------------------------------------------------------------------------
      }
      //----------------------------------------------------------------------------------
      break;  //=======================================================

    case 2:
      //#############################################################
      //##   Regim =2   ##      Выполняется функция 2              ##
      //##---------------------------------------------------------##
      //##                                                         ##
      //##                 Планирование дня                        ##
      //##           настройка функции                             ##
      //##                                                         ##
      //#############################################################
      if (Flag2) {  //==========================================================
        // Рисуем экран
        Serial.print("   --- 2222222 -------------  ");
        Serial.println(Regim);
        tft.fillScreen(BLACK);  // Красим экран в черный цвет
        // ---------- кнопка назад --------------
        tft.fillRoundRect(5, 299, 70, 20, 5, GREY);   //  Кнопка с заливкой
        tft.drawRoundRect(5, 299, 70, 20, 5, WHITE);  //  Кнопка рамка
        tft.setTextColor(WHITE);
        tft.setTextSize(2);
        tft.setCursor(10, 300);
        tft.println(utf8rus("назад"));
        // ------------ Кнопка далее ----------------------------
        tft.fillRoundRect(165, 299, 70, 20, 5, GREY);   //  Кнопка с заливкой
        tft.drawRoundRect(165, 299, 70, 20, 5, WHITE);  //  Кнопка рамка
        tft.setTextColor(WHITE);
        tft.setTextSize(2);
        tft.setCursor(170, 300);
        tft.println(utf8rus("далее"));
        tft.setTextColor(WHITE);
        tft.setTextSize(2);
        tft.setCursor(30, 20);
        tft.println(utf8rus("Распорядок дня"));
        // ---- Рисуем распорядок режима дня --------------
        tft.fillRoundRect(20, 50, 57, 50, 5, GOLD);  //Время 6:00
        tft.setTextColor(BLACK);
        tft.setTextSize(2);
        tft.setCursor(22, 70);
        tft.println("06");
        tft.setCursor(43, 70);
        tft.println(":");
        tft.setCursor(50, 70);
        tft.println("00");
        tft.fillRoundRect(90, 50, 57, 50, 5, BEIGE);  //Время 8:00
        tft.setTextColor(BLACK);
        tft.setTextSize(2);
        tft.setCursor(92, 70);
        tft.println("08");
        tft.setCursor(113, 70);
        tft.println(":");
        tft.setCursor(120, 70);
        tft.println("00");
        tft.drawRoundRect(159, 50, 57, 50, 5, WHITE);  //Время 10:00
        tft.setTextColor(WHITE);
        tft.setTextSize(2);
        tft.setCursor(162, 70);
        tft.println("10");
        tft.setCursor(183, 70);
        tft.println(":");
        tft.setCursor(190, 70);
        tft.println("00");
        tft.fillRoundRect(20, 140, 57, 50, 5, SILVER);  //Время 15:00
        tft.setTextColor(BLACK);
        tft.setTextSize(2);
        tft.setCursor(22, 160);
        tft.println("15");
        tft.setCursor(43, 160);
        tft.println(":");
        tft.setCursor(50, 160);
        tft.println("00");
        tft.fillRoundRect(90, 140, 57, 50, 5, WHITE);  //Время 18:00
        tft.setTextColor(BLACK);
        tft.setTextSize(2);
        tft.setCursor(92, 160);
        tft.println("18");
        tft.setCursor(113, 160);
        tft.println(":");
        tft.setCursor(120, 160);
        tft.println("00");
        tft.fillRoundRect(160, 140, 57, 50, 5, GOLD);  //Время 20:00
        tft.setTextColor(BLACK);
        tft.setTextSize(2);
        tft.setCursor(162, 160);
        tft.println("20");
        tft.setCursor(183, 160);
        tft.println(":");
        tft.setCursor(190, 160);
        tft.println("00");
        tft.drawRoundRect(20, 220, 57, 50, 5, WHITE);  //Время 22:00
        tft.setTextColor(WHITE);
        tft.setTextSize(2);
        tft.setCursor(22, 240);
        tft.println("22");
        tft.setCursor(43, 240);
        tft.println(":");
        tft.setCursor(50, 240);
        tft.println("00");
        Flag2 = false;
      }
      //=============================================================
      if (TCpus) {  // Проверка Нажатие тачскина
        // --------- Проверка кнопки назад ----------------------------------
        if (((TCx > 5) && (TCx < 80)) && ((TCy > 299) && (TCy < 320))) {
          led.setColor(Color);      // список цветов в GRGB.h --- БЕЛЫЙ
          led.setBrightness(Brig);  // установить ОБЩУЮ яркость -- МАКСИМУМ
          led.disable();            // выключаем светодиод
          Regim = 1;
          Flag2 = true;
        }
        // --------- Проверка кнопк далее -----------------------------------
        if (((TCx > 165) && (TCx < 235)) && ((TCy > 299) && (TCy < 320))) {
          led.setColor(Color);      // список цветов в GRGB.h --- БЕЛЫЙ
          led.setBrightness(Brig);  // установить ОБЩУЮ яркость -- МАКСИМУМ
          led.disable();            // выключаем светодиод
          Regim = 3;
          Flag2 = true;
        }
      }
      break;  //=====================================================

    case 3:
      //#############################################################
      //##   Regim =3   ##      Выполняется функция 3              ##
      //##---------------------------------------------------------##
      //##                                                         ##
      //##                Выполнение функции                       ##
      //##   Автоматическое управление светом по плану дня         ##
      //##                                                         ##
      //#############################################################
      if (Flag3) {  //==========================================================
                    //                        Рисуем экран
        Serial.print("   --- 3333333333333 -------------  ");
        Serial.println(Regim);
        tft.fillScreen(BLACK);  // Красим экран в черный цвет
        tft.drawRect(20, 30, 90, 50, WHITE);
        tft.drawLine(20, 200, 216, 200, WHITE);
        tft.setTextColor(GOLD);
        tft.setTextSize(2);
        tft.setCursor(130, 45);
        tft.println("LYMOS");
        tft.setTextColor(WHITE);
        tft.setTextSize(2);
        tft.setCursor(51, 45);
        tft.println(utf8rus(":00"));
        Timed = 0;
        tft.setCursor(30, 45);
        tft.setTextSize(3);
        tft.fillRoundRect(5, 299, 70, 20, 5, GREY);   //  Кнопка с заливкой
        tft.drawRoundRect(5, 299, 70, 20, 5, WHITE);  //  Кнопка рамка
        tft.setTextColor(WHITE);
        tft.setTextSize(2);
        tft.setCursor(10, 300);
        tft.println(utf8rus("назад"));
        Flag3 = false;
      }
      //=============================================================

      //tft.setCursor (30,45);
      //tft.setTextColor(BLACK);
      //tft.print(Timed/6);
      tft.fillRect(22, 32, 30, 40, BLACK);
      Timed = Timed + 1;
      //Serial.println(Timed);
      tft.setCursor(30, 45);
      tft.setTextColor(WHITE);
      if ((Timed / 6) < 10) tft.setCursor(38, 45);
      tft.print(Timed / 6);
      delay(100);

      if (Timed / 6 < 5) {  // Условие проверки Timed<6
        led.setColor(BLACK);
        tft.drawLine(20, 195, 20, 205, WHITE);
        tft.drawRect(20, 160, 20, 20, WHITE);
        tft.drawLine(20, 155, 40, 155, WHITE);
        tft.fillRect(20, 210, 5, 10, WHITE);
      }                                          // -----------------------------------
      if ((Timed / 6 > 5) && (Timed / 6 < 7)) {  // Условие проверки Timed<7 и Timed>5
        led.setColor(S_GOLD);
        tft.drawLine(46, 195, 46, 205, WHITE);
        tft.fillRect(46, 160, 20, 20, GOLD);
        tft.drawLine(20, 155, 40, 155, BLACK);
        tft.drawLine(46, 155, 66, 155, WHITE);
        tft.fillRect(20, 210, 5, 10, BLACK);
        tft.fillRect(46, 210, 5, 10, WHITE);
      }                                           // ------------------------------------
      if ((Timed / 6 > 7) && (Timed / 6 < 10)) {  // Условие проверки Timed<10 и Timed>7
        led.setColor(S_BEIGE);
        tft.drawLine(71, 195, 71, 205, WHITE);
        tft.fillRect(71, 160, 20, 20, BEIGE);
        tft.drawLine(46, 155, 66, 155, BLACK);
        tft.drawLine(71, 155, 91, 155, WHITE);
        tft.fillRect(46, 210, 5, 10, BLACK);
        tft.fillRect(71, 210, 5, 10, WHITE);
      }                                           // -------------------------------------
      if ((Timed / 6 > 9) && (Timed / 6 < 15)) {  // Условие проверки Timed<15 и Timed>9
        led.setColor(BLACK);
        tft.drawLine(96, 195, 96, 205, WHITE);
        tft.drawRect(96, 160, 20, 20, WHITE);
        tft.drawLine(71, 155, 91, 155, BLACK);
        tft.drawLine(96, 155, 116, 155, WHITE);
        tft.fillRect(71, 210, 5, 10, BLACK);
        tft.fillRect(96, 210, 5, 10, WHITE);
      }                                            // ---------------------------------------
      if ((Timed / 6 > 14) && (Timed / 6 < 18)) {  // Условие проверки Timed<18 и Timed>14
        led.setColor(S_SILVER);
        tft.drawLine(121, 195, 121, 205, WHITE);
        tft.fillRect(121, 160, 20, 20, SILVER);
        tft.drawLine(96, 155, 116, 155, BLACK);
        tft.drawLine(121, 155, 141, 155, WHITE);
        tft.fillRect(96, 210, 5, 10, BLACK);
        tft.fillRect(121, 210, 5, 10, WHITE);
      }                                            // ------------------------------------------
      if ((Timed / 6 > 17) && (Timed / 6 < 20)) {  // Условие проверки Timed<20 и Timed>17
        led.setColor(WHITE);
        led.setKelvin(3224);
        tft.drawLine(146, 195, 146, 205, WHITE);
        tft.fillRect(146, 160, 20, 20, WHITE);
        tft.drawLine(121, 155, 141, 155, BLACK);
        tft.drawLine(146, 155, 166, 155, WHITE);
        tft.fillRect(121, 210, 5, 10, BLACK);
        tft.fillRect(146, 210, 5, 10, WHITE);
      }                                            // --------------------------------------
      if ((Timed / 6 > 19) && (Timed / 6 < 22)) {  // Условие проверки Timed<22 и Timed>19
        led.setColor(S_BEIGE);
        tft.drawLine(171, 195, 171, 205, WHITE);
        tft.fillRect(171, 160, 20, 20, BEIGE);
        tft.drawLine(146, 155, 166, 155, BLACK);
        tft.drawLine(171, 155, 191, 155, WHITE);
        tft.fillRect(146, 210, 5, 10, BLACK);
        tft.fillRect(171, 210, 5, 10, WHITE);
      }                                            // ---------------------------------------
      if ((Timed / 6 > 21) && (Timed / 6 < 24)) {  // Условие проверки Timed<22 и Timed>19
        led.setColor(BLACK);
        tft.drawLine(196, 195, 196, 205, WHITE);
        tft.drawRect(196, 160, 20, 20, WHITE);
        tft.drawLine(171, 155, 191, 155, BLACK);
        tft.drawLine(196, 155, 216, 155, WHITE);
        tft.fillRect(171, 210, 5, 10, BLACK);
        tft.fillRect(196, 210, 5, 10, WHITE);
      }                      // ---------------------------------------------------
      if (Timed / 6 > 23) {  //повтор дня
        tft.setCursor(30, 45);
        tft.setTextColor(BLACK);
        tft.print(Timed / 6);
        Timed = 0;
        tft.fillRect(20, 160, 196, 20, BLACK);
        tft.drawLine(196, 155, 216, 155, BLACK);
        tft.fillRect(196, 210, 5, 10, BLACK);
      }  // -------------------------------------------------------------------------
         //==========================================================================
      ReadTocScrin();
      if (TCpus) {  // Проверка Нажатие тачскина
        // --------- Проверка кнопки назад ------------------------------------------
        if (((TCx > 5) && (TCx < 80)) && ((TCy > 299) && (TCy < 320))) {
          led.setColor(Color);      // список цветов в GRGB.h --- БЕЛЫЙ
          led.setBrightness(Brig);  // установить ОБЩУЮ яркость -- МАКСИМУМ
          led.disable();            // выключаем светодиод
          Regim = 1;
          Flag3 = true;
        }
      }
      // -----------------------------------------------------------------------------------
      break;

    case 4:
      //#############################################################
      //##   Regim =4   ##      Выполняется функция 4              ##
      //##---------------------------------------------------------##
      //##                                                         ##
      //##                 Функция                                 ##
      //##               настройка света                           ##
      //##                                                         ##
      //#############################################################
      if (Flag4) {                //==========================================================
                                  //                 Включаем светодиод
        led.setColor(Color);      // список цветов в GRGB.h --- БЕЛЫЙ
        led.setBrightness(Brig);  // установить ОБЩУЮ яркость -- МАКСИМУМ
        led.setKelvin(Warmth);
        led.enable();  // выключаем светодиод
                       //==========================================================
        //                        Рисуем экран
        Serial.print("   --- 444444444444444444 -------------  ");
        Serial.println(Regim);
        tft.fillScreen(BLACK);  // Красим экран в черный цвет
                                // ----- рисуем заголовок экрана -----------------------
        tft.drawRect(1, 1, 238, 50, GOLD);
        tft.drawRect(2, 2, 237, 49, GOLD);
        tft.fillRect(4, 30, 233, 19, WHITE);
        tft.setTextColor(GOLD);  // Пишем название проекта
        tft.setTextSize(3);
        tft.setCursor(75, 5);
        tft.println("LYMOS");
        tft.setTextColor(BLACK);  //Настройка светодиода
        tft.setTextSize(2);
        tft.setCursor(30, 32);
        tft.println(utf8rus("Настройка света"));
        //--------------- Кнопка возврата главное меню --------------------
        tft.fillRoundRect(5, 299, 70, 20, 5, GREY);   //  Кнопка с заливкой
        tft.drawRoundRect(5, 299, 70, 20, 5, WHITE);  //  Кнопка рамка
        tft.setTextColor(WHITE);                      //Настройка светодиода
        tft.setTextSize(2);
        tft.setCursor(10, 300);
        tft.println(utf8rus("назад"));
        //--------------- Меню настройка цвета --------------------
        tft.fillRoundRect(20, 60, 40, 40, 5, WHITE);
        tft.drawRoundRect(18, 58, 44, 44, 5, CYAN);
        tft.fillRoundRect(70, 60, 40, 40, 5, YELLOW);
        tft.fillRoundRect(120, 60, 40, 40, 5, GREEN);
        tft.fillRoundRect(170, 60, 40, 40, 5, BLUE);
        tft.fillRoundRect(20, 120, 40, 40, 5, LILAC);
        tft.fillRoundRect(70, 120, 40, 40, 5, MAGENTA);
        tft.fillRoundRect(120, 120, 40, 40, 5, RED);
        tft.fillRoundRect(170, 120, 40, 40, 5, ORANGE);
        // ---------------- Меню установки яркости -----------------
        tft.drawRect(10, 170, 220, 50, WHITE);
        tft.setTextColor(WHITE);
        tft.setTextSize(2);
        tft.setCursor(10, 175);
        tft.println(utf8rus(" -    Яркость   +"));
        tft.drawLine(15, 205, 225, 205, YELLOW);
        // -- рисуем бегунок текущей яркости --------------
        // (226-12)\255=0,84
        k = (Brig * 0.84) + 12;
        //Serial.print("K = "); Serial.print(k);
        // Serial.print("\t    Bri "); Serial.println(Brig);
        tft.fillRect(k, 200, 5, 10, WHITE);
        // ---------------- Меню установки теплоты цвета -----------------
        tft.drawRect(10, 230, 220, 50, WHITE);
        tft.setTextColor(WHITE);
        tft.setTextSize(2);
        tft.setCursor(10, 235);
        tft.println(utf8rus(" -    Теплота   +"));
        tft.drawLine(15, 265, 225, 265, YELLOW);
        // -- рисуем бегунок текущей теплоты --------------
        // (226-12)\255=0,84  Warmth  ((Warmth-1000)/191,176)
        k = 12 + ((Warmth) / 46.7289);
        //Serial.print("K = "); Serial.print(k);
        // Serial.print("\t    War "); Serial.println(Brig);
        tft.fillRect(k, 260, 5, 10, WHITE);
        Flag4 = false;
      }
      //=============================================================
      if (TCpus) {  // Проверка Нажатие тачскина
        // --------- Проверка кнопки назад ------------------------------------------
        if (((TCx > 5) && (TCx < 80)) && ((TCy > 299) && (TCy < 320))) {
          Regim = 1;
          Flag4 = true;
        }
        // ---- изменение  теплоты  светодиода ------------------
        if (((TCx > 12) && (TCx < 226)) && ((TCy > 230) && (TCy < 280))) {
          Warmth = ((TCx - 12) * 46.7289);
          tft.fillRect(12, 260, 217, 10, BLACK);
          k = 12 + ((Warmth) / 46.7289);
          tft.fillRect(k, 260, 5, 10, WHITE);
          tft.drawLine(15, 265, 225, 265, YELLOW);
          //Serial.print(TCx);Serial.print("    k ");
          //Serial.print(k);Serial.print("    Warmth ");
          //Serial.println(Warmth);
          led.setKelvin(Warmth);
        }
        // ---- изменение яркости светодиода ------------------
        if (((TCx > 12) && (TCx < 226)) && ((TCy > 170) && (TCy < 220))) {
          Brig = (TCx - 12) / 0.84;
          tft.fillRect(12, 200, 217, 10, BLACK);
          k = (Brig * 0.84) + 12;
          tft.fillRect(k, 200, 5, 10, WHITE);
          tft.drawLine(15, 205, 225, 205, YELLOW);
          led.setBrightness(Brig);
        }
        // --------- Проверка кнопки белый --------------------------------------------------------------------
        if (((TCx > 20) && (TCx < 60)) && ((TCy > 60) && (TCy < 100))) {  // Условие проверки нажатия белого квадрата
          led.setColor(WHITE);
          led.setKelvin(Warmth);
          tft.drawRoundRect(18, 58, 44, 44, 5, CYAN);
          tft.drawRoundRect(68, 58, 44, 44, 5, BLACK);
          tft.drawRoundRect(118, 58, 44, 44, 5, BLACK);
          tft.drawRoundRect(168, 58, 44, 44, 5, BLACK);
          tft.drawRoundRect(18, 118, 44, 44, 5, BLACK);
          tft.drawRoundRect(68, 118, 44, 44, 5, BLACK);
          tft.drawRoundRect(118, 118, 44, 44, 5, BLACK);
          tft.drawRoundRect(168, 118, 44, 44, 5, BLACK);
          // ---------------- Меню установки теплоты цвета -----------------
          tft.drawRect(10, 230, 220, 50, WHITE);
          tft.setTextColor(WHITE);
          tft.setTextSize(2);
          tft.setCursor(10, 235);
          tft.println(utf8rus(" -    Теплота   +"));
          tft.drawLine(15, 265, 225, 265, YELLOW);
          // -- рисуем бегунок текущей теплоты --------------
          // (226-12)\255=0,84  Warmth  ((Warmth)/46.7289)
          k = 12 + ((Warmth) / 46.7289);
          tft.fillRect(k, 260, 5, 10, WHITE);
        }
        // --------- Проверка кнопки жёлтый --------------------------------------------------------------------
        if (((TCx > 70) && (TCx < 110)) && ((TCy > 60) && (TCy < 100))) {  // Условие проверки нажатия жёлтого квадрата
          led.setColor(S_GOLD);
          tft.drawRoundRect(18, 58, 44, 44, 5, BLACK);
          tft.drawRoundRect(68, 58, 44, 44, 5, CYAN);
          tft.drawRoundRect(118, 58, 44, 44, 5, BLACK);
          tft.drawRoundRect(168, 58, 44, 44, 5, BLACK);
          tft.drawRoundRect(18, 118, 44, 44, 5, BLACK);
          tft.drawRoundRect(68, 118, 44, 44, 5, BLACK);
          tft.drawRoundRect(118, 118, 44, 44, 5, BLACK);
          tft.drawRoundRect(168, 118, 44, 44, 5, BLACK);
          tft.fillRect(10, 230, 220, 50, BLACK);
        }
        // --------- Проверка кнопки зелёный --------------------------------------------------------------------
        if (((TCx > 120) && (TCx < 160)) && ((TCy > 60) && (TCy < 100))) {  // Условие проверки нажатия зелёного квадрата
          led.setColor(GREEN);
          tft.drawRoundRect(18, 58, 44, 44, 5, BLACK);
          tft.drawRoundRect(68, 58, 44, 44, 5, BLACK);
          tft.drawRoundRect(118, 58, 44, 44, 5, CYAN);
          tft.drawRoundRect(168, 58, 44, 44, 5, BLACK);
          tft.drawRoundRect(18, 118, 44, 44, 5, BLACK);
          tft.drawRoundRect(68, 118, 44, 44, 5, BLACK);
          tft.drawRoundRect(118, 118, 44, 44, 5, BLACK);
          tft.drawRoundRect(168, 118, 44, 44, 5, BLACK);
          tft.fillRect(10, 230, 220, 50, BLACK);
        }
        // --------- Проверка кнопки синий --------------------------------------------------------------------
        if (((TCx > 170) && (TCx < 210)) && ((TCy > 60) && (TCy < 100))) {  // Условие проверки нажатия синего квадрата
          led.setColor(BLUE);
          tft.drawRoundRect(18, 58, 44, 44, 5, BLACK);
          tft.drawRoundRect(68, 58, 44, 44, 5, BLACK);
          tft.drawRoundRect(118, 58, 44, 44, 5, BLACK);
          tft.drawRoundRect(168, 58, 44, 44, 5, CYAN);
          tft.drawRoundRect(18, 118, 44, 44, 5, BLACK);
          tft.drawRoundRect(68, 118, 44, 44, 5, BLACK);
          tft.drawRoundRect(118, 118, 44, 44, 5, BLACK);
          tft.drawRoundRect(168, 118, 44, 44, 5, BLACK);
          tft.fillRect(10, 230, 220, 50, BLACK);
        }
        // --------- Проверка кнопки фиолетовый --------------------------------------------------------------------
        if (((TCx > 20) && (TCx < 60)) && ((TCy > 120) && (TCy < 160))) {  // Условие проверки нажатия фиолетового квадрата
          led.setColor(LILAC);
          tft.drawRoundRect(18, 58, 44, 44, 5, BLACK);
          tft.drawRoundRect(68, 58, 44, 44, 5, BLACK);
          tft.drawRoundRect(118, 58, 44, 44, 5, BLACK);
          tft.drawRoundRect(168, 58, 44, 44, 5, BLACK);
          tft.drawRoundRect(18, 118, 44, 44, 5, CYAN);
          tft.drawRoundRect(68, 118, 44, 44, 5, BLACK);
          tft.drawRoundRect(118, 118, 44, 44, 5, BLACK);
          tft.drawRoundRect(118, 118, 44, 44, 5, BLACK);
          tft.fillRect(10, 230, 220, 50, BLACK);
        }
        // --------- Проверка кнопки розовый --------------------------------------------------------------------
        if (((TCx > 70) && (TCx < 110)) && ((TCy > 120) && (TCy < 160))) {  // Условие проверки нажатия розового квадрата
          led.setColor(C_MAGENTA);
          tft.drawRoundRect(18, 58, 44, 44, 5, BLACK);
          tft.drawRoundRect(68, 58, 44, 44, 5, BLACK);
          tft.drawRoundRect(118, 58, 44, 44, 5, BLACK);
          tft.drawRoundRect(168, 58, 44, 44, 5, BLACK);
          tft.drawRoundRect(18, 118, 44, 44, 5, BLACK);
          tft.drawRoundRect(68, 118, 44, 44, 5, CYAN);
          tft.drawRoundRect(118, 118, 44, 44, 5, BLACK);
          tft.drawRoundRect(168, 118, 44, 44, 5, BLACK);
          tft.fillRect(10, 230, 220, 50, BLACK);
        }
        // --------- Проверка кнопки красный --------------------------------------------------------------------
        if (((TCx > 120) && (TCx < 160)) && ((TCy > 120) && (TCy < 160))) {  // Условие проверки нажатия красного квадрата
          led.setColor(RED);
          tft.drawRoundRect(18, 58, 44, 44, 5, BLACK);
          tft.drawRoundRect(68, 58, 44, 44, 5, BLACK);
          tft.drawRoundRect(118, 58, 44, 44, 5, BLACK);
          tft.drawRoundRect(168, 58, 44, 44, 5, BLACK);
          tft.drawRoundRect(18, 118, 44, 44, 5, BLACK);
          tft.drawRoundRect(68, 118, 44, 44, 5, BLACK);
          tft.drawRoundRect(118, 118, 44, 44, 5, CYAN);
          tft.drawRoundRect(168, 118, 44, 44, 5, BLACK);
          tft.fillRect(10, 230, 220, 50, BLACK);
        }
        // --------- Проверка кнопки ораньжевый --------------------------------------------------------------------
        if (((TCx > 170) && (TCx < 210)) && ((TCy > 120) && (TCy < 160))) {  // Условие проверки нажатия ораньжевого квадрата
          led.setColor(C_ORANGE);
          tft.drawRoundRect(18, 58, 44, 44, 5, BLACK);
          tft.drawRoundRect(68, 58, 44, 44, 5, BLACK);
          tft.drawRoundRect(118, 58, 44, 44, 5, BLACK);
          tft.drawRoundRect(168, 58, 44, 44, 5, BLACK);
          tft.drawRoundRect(18, 118, 44, 44, 5, BLACK);
          tft.drawRoundRect(68, 118, 44, 44, 5, BLACK);
          tft.drawRoundRect(118, 118, 44, 44, 5, BLACK);
          tft.drawRoundRect(168, 118, 44, 44, 5, CYAN);
          tft.fillRect(10, 230, 220, 50, BLACK);
        }
      }
      break;  //=====================================================

    case 5:
      //#############################################################
      //##   Regim =5   ##      Выполняется функция 5              ##
      //##---------------------------------------------------------##
      //##                                                         ##
      //##                 Меню                                    ##
      //##              функций управления                         ##
      //##                                                         ##
      //#############################################################
      if (Flag5) {  //==========================================================
        //                        Рисуем экран
        Serial.print("   --- 5555555555555 -------------  ");
        Serial.println(Regim);
        tft.fillScreen(BLACK);  // Красим экран в черный цвет
                                // ----- рисуем заголовок экрана -----------------------
        tft.drawRect(1, 1, 238, 50, GOLD);
        tft.drawRect(2, 2, 237, 49, GOLD);
        tft.fillRect(4, 30, 233, 19, WHITE);
        tft.setTextColor(GOLD);  // Пишем название проекта
        tft.setTextSize(3);
        tft.setCursor(75, 5);
        tft.println("LYMOS");
        tft.setTextColor(BLACK);  //Настройка светодиода
        tft.setTextSize(2);
        tft.setCursor(18, 32);
        tft.println(utf8rus("Функции управления"));
        //--------------- Кнопка возврата главное меню --------------------
        tft.fillRoundRect(5, 299, 70, 20, 5, GREY);   //  Кнопка с заливкой
        tft.drawRoundRect(5, 299, 70, 20, 5, WHITE);  //  Кнопка рамка
        tft.setTextColor(WHITE);
        tft.setTextSize(2);
        tft.setCursor(10, 300);
        tft.println(utf8rus("назад"));
        //--------------- Кнопка функции Планинга дня -------------------
        tft.fillRoundRect(10, 80, 220, 30, 8, DarBLUE);  //  Кнопка с заливкой
        tft.drawRoundRect(10, 80, 220, 30, 8, YELLOW);   //  Кнопка рамка
        tft.setTextColor(WHITE);                         //Настройка светадиода
        tft.setTextSize(2);
        tft.setCursor(55, 88);
        tft.println(utf8rus("ПЛАНИНГ ДНЯ"));
        //--------------- Кнопка функции Звук -------------------
        tft.fillRoundRect(10, 130, 220, 30, 8, DarBLUE);  //  Кнопка с заливкой
        tft.drawRoundRect(10, 130, 220, 30, 8, YELLOW);   //  Кнопка рамка
        tft.setTextColor(WHITE);                          //Настройка светадиода
        tft.setTextSize(2);
        tft.setCursor(55, 138);
        tft.println(utf8rus("ДАТЧИК ЗВУКА"));
        //--------------- Кнопка функции Освещённость -------------------
        tft.fillRoundRect(10, 180, 220, 30, 8, DarBLUE);  //  Кнопка с заливкой
        tft.drawRoundRect(10, 180, 220, 30, 8, YELLOW);   //  Кнопка рамка
        tft.setTextColor(WHITE);                          //Настройка светадиода
        tft.setTextSize(2);
        tft.setCursor(25, 188);
        tft.println(utf8rus("ДАТЧИК ЯРКОСТИ"));
        //--------------- Кнопка функции Датчик движения -------------------
        tft.fillRoundRect(10, 230, 220, 30, 8, DarBLUE);  //  Кнопка с заливкой
        tft.drawRoundRect(10, 230, 220, 30, 8, YELLOW);   //  Кнопка рамка
        tft.setTextColor(WHITE);                          //Настройка светадиода
        tft.setTextSize(2);
        tft.setCursor(25, 238);
        tft.println(utf8rus("ДАТЧИК ДВИЖЕНИЯ"));
        Flag5 = false;
      }
      //=============================================================
      if (TCpus) {  // Проверка Нажатие тачскина
        // --------- Проверка кнопки назад ----------------------------------
        if (((TCx > 5) && (TCx < 80)) && ((TCy > 299) && (TCy < 320))) {
          Regim = 1;
          Flag5 = true;
        }
        //=============================================================
        // --------- Проверка кнопки планинг дня --------------------------
        if (((TCx > 8) && (TCx < 220)) && ((TCy > 88) && (TCy < 110))) {
          Regim = 2;
          Flag5 = true;
        }
        //=============================================================
        // --------- Проверка кнопки датчик звука ----------------------------
        if (((TCx > 8) && (TCx < 220)) && ((TCy > 130) && (TCy < 160))) {
          Regim = 6;
          Flag5 = true;
        }
        //=============================================================
        // --------- Проверка кнопки датчик яркости --------------------------
        if (((TCx > 8) && (TCx < 220)) && ((TCy > 180) && (TCy < 210))) {
          Regim = 8;
          Flag5 = true;
        }
        //=============================================================
        // --------- Проверка кнопки датчик движения --------------------------
        if (((TCx > 8) && (TCx < 220)) && ((TCy > 230) && (TCy < 260))) {
          Regim = 7;
          Flag5 = true;
        }
      }
      break;  //=====================================================

    case 6:
      //#############################################################
      //##   Regim =6   ##      Выполняется функция               ##
      //##---------------------------------------------------------##
      //##                                                         ##
      //##                      ФУНКЦИЯ                            ##
      //##                    Датчик звука                         ##
      //##                                                         ##
      //#############################################################
      if (Flag6) {  //==========================================================
        //                        Рисуем экран
        Serial.print("   --- 666666666666666 -------------  ");
        Serial.println(Regim);
        led.disable();
        tft.fillScreen(BLACK);                        // Красим экран в черный цвет
        //--------------- Кнопка возврата главное меню --------------------
        tft.fillRoundRect(5, 299, 70, 20, 5, GREY);   //  Кнопка с заливкой
        tft.drawRoundRect(5, 299, 70, 20, 5, WHITE);  //  Кнопка рамка
        tft.setTextColor(WHITE);
        tft.setTextSize(2);
        tft.setCursor(10, 300);
        tft.println(utf8rus("назад"));
        // Заставка экрана вверху
        tft.drawRect(1, 1, 238, 50, GOLD);
        tft.drawRect(2, 2, 237, 49, GOLD);
        tft.fillRect(4, 30, 233, 19, WHITE);
       // Поле для индикатора датчика
        tft.drawRect(209,79, 22, 102, WHITE);
        tft.setTextColor(GOLD);  // Пишем название проекта
        tft.setTextSize(3);
        tft.setCursor(75, 5);
        tft.println("LYMOS");
        tft.setTextColor(BLACK);
        tft.setTextSize(2);
        tft.setCursor(40, 32);
        tft.println(utf8rus("Датчик звука"));
        // инструкция
        tft.setTextColor(WHITE);  // Громкий звук
        tft.setTextSize(3);
       tft.setCursor(40, 75);
        tft.println(utf8rus("Издайте"));
        tft.setCursor(40, 105);
        tft.println(utf8rus("Громкий "));
        tft.setCursor(75, 135);
        tft.println(utf8rus("звук"));
        Zvuk=555;
        i = 0;
        Flag6 = false;
      }
      //=============================================================
      if (TCpus) {  // Проверка Нажатие тачскина
        // --------- Проверка кнопки назад -------------------------------------
        if (((TCx > 5) && (TCx < 80)) && ((TCy > 299) && (TCy < 320))) {
          Regim = 1;
          Flag6 = true;
        }
      }
      // --------- Опрос датчика звука --------------------
      val = analogRead(analogZoom);  // считываем значение
      Zvuk = (Zvuk * 0.95) + ((float)val * 0.05);  // Средний уровень шума
      Clap= digitalRead (47);
      if ((Clap== HIGH) && (SClap== HIGH)){
          Clap=0;
       } else {  // не горит - включаем
          SClap=Clap;
        
      }
      tft.drawLine(211,179-i,229,179-i,BLACK);
      k= abs(Zvuk-val);
      if (abs(Zvuk-val)>50) Zvuk=val;
      i=k*5;
      k=0;
      if (i>98) i=98;
      tft.drawLine(211,135,229,135,RED);
      tft.drawLine(211,179-i,229,179-i,GOLD);
      //tft.fillRect(210, 179-k, 20, 100,BLACK);
      //tft.fillRect(210, 80, 20, k, WHITE);
      if ( (k>8) || (Clap== HIGH) ) {  // Сработал датчик звука
        if (Lumos) {  // горит - выключаем
          Lumos = false;
          led.disable();
        } else {  // не горит - включаем
          Lumos = true;
          led.enable();
        }
        //i = k;
        tft.fillRect(210, 182, 20, 20, GREEN);
        delay(200);
      } else {
       tft.fillRect(210, 182, 20, 20, BLACK);
      }
      //Serial.print(val);  // выводим полученное значение
      //Serial.print("   mid = ");
      //Serial.print( Zvuk);
      Serial.print( Clap); Serial.print(" -- ");
      Serial.print( 8);
      Serial.print("   max = ");
      Serial.println( k);
      break;  //=====================================================

    case 7:
      //#############################################################
      //##   Regim =7   ##      Выполняется функция                ##
      //##---------------------------------------------------------##
      //##                                                         ##
      //##                      ФУНКЦИЯ                            ##
      //##                    Датчик   движения                    ##
      //##                                                         ##
      //#############################################################
      if (Flag7) {  //==========================================================
                    //  Рисуем экран
        Serial.print("   --- 77777 -------------  ");
        Serial.println(Regim);
        led.disable();
        tft.fillScreen(BLACK);                        // Красим экран в черный цвет
        //--------------- Кнопка возврата главное меню --------------------
        tft.fillRoundRect(5, 299, 70, 20, 5, GREY);   //  Кнопка с заливкой
        tft.drawRoundRect(5, 299, 70, 20, 5, WHITE);  //  Кнопка рамка
        tft.setTextColor(WHITE);
        tft.setTextSize(2);
        tft.setCursor(10, 300);
        tft.println(utf8rus("назад"));
        // Заставка экрана вверху
        tft.drawRect(1, 1, 238, 50, GOLD);
        tft.drawRect(2, 2, 237, 49, GOLD);
        tft.fillRect(4, 30, 233, 19, WHITE);
        tft.setTextColor(GOLD);  // Пишем название проекта
        tft.setTextSize(3);
        tft.setCursor(75, 5);
        tft.println("LYMOS");
        tft.setTextColor(BLACK);
        tft.setTextSize(2);
        tft.setCursor(40, 32);
        tft.println(utf8rus("Датчик движения"));
        tft.setTextColor(WHITE);// инструкция
        tft.setTextSize(3);
        tft.setCursor(60, 100);
        tft.println(utf8rus("Сделайте "));
        tft.setCursor(40,140);
        tft.println(utf8rus(" движение"));
        Flag7 = false;
      }
      //=============================================================
      if (TCpus) {  // Проверка Нажатие тачскина
        // --------- Проверка кнопки назад -----------------------------------
        if (((TCx > 5) && (TCx < 80)) && ((TCy > 299) && (TCy < 320))) {
          Regim = 1;
          Flag7 = true;
          led.disable();
        }
      }
     //=============================================================
     // обработка датчка движеня
     Dmove= digitalRead (48);
      if  (Dmove==HIGH){
       led.enable();    //включение света когда сработал датчик движеня
      }
     else{
       led.disable();    //включение света когда движеня нет
     }

      break;  //=====================================================

    case 8:
      //#############################################################
      //##   Regim =8   ##      Выполняется функция                ##
      //##---------------------------------------------------------##
      //##                                                         ##
      //##                      ФУНКЦИЯ                            ##
      //##                    Датчик    яркости                    ##
      //##                                                         ##
      //#############################################################
      if (Flag8) {  //==========================================================
                    //  Рисуем экран
        Serial.print("   --- 888888 -------------  ");
        Serial.println(Regim);
        led.disable();
        tft.fillScreen(BLACK);                        // Красим экран в черный цвет
        //--------------- Кнопка возврата главное меню --------------------
        tft.fillRoundRect(5, 299, 70, 20, 5, GREY);   //  Кнопка с заливкой
        tft.drawRoundRect(5, 299, 70, 20, 5, WHITE);  //  Кнопка рамка
        tft.setTextColor(WHITE);
        tft.setTextSize(2);
        tft.setCursor(10, 300);
        tft.println(utf8rus("назад"));
        // Заставка экрана вверху
        tft.drawRect(1, 1, 238, 50, GOLD);
        tft.drawRect(2, 2, 237, 49, GOLD);
        tft.fillRect(4, 30, 233, 19, WHITE);
        tft.setTextColor(GOLD);  // Пишем название проекта
        tft.setTextSize(3);
        tft.setCursor(75, 5);
        tft.println("LYMOS");
        tft.setTextColor(BLACK);
        tft.setTextSize(2);
        tft.setCursor(40, 32);
        tft.println(utf8rus("Датчик яркости"));
        // инструкция
        // Поле для индикатора датчика
        tft.drawRect(209,79, 22, 102, WHITE);
        i=0;
        Flag8 = false;
      }
      //=============================================================
      if (TCpus) {  // Проверка Нажатие тачскина
        // --------- Проверка кнопки назад ------------------------------------------
        if (((TCx > 5) && (TCx < 80)) && ((TCy > 299) && (TCy < 320))) {
          Regim = 1;
          Flag8 = true;
        }
      }
     // --------- Опрос датчика освещённости --------------------
      val = analogRead(analogLum);  // считываем значение
     Lum = (Lum * 0.9) + ((float)val * 0.1);  // Фильтр датчика яркости
      if (Lum<100) {
        led.enable();    //включение света когда темно
        tft.fillRoundRect(10, 245, 125, 30, 7, GREEN);   //  Кнопка с заливкой
        tft.drawRoundRect(10, 245, 125, 30, 7, WHITE);  //  Кнопка рамка
        tft.setTextColor(DarBLUE);
        tft.setTextSize(2);
        tft.setCursor(20, 252);
        tft.println(utf8rus("включение"));
               // ----------- светодиод ВКЛ --------------------
        tft.fillCircle(60, 160, 40, WHITE);    // Круглая кнопка с заливкой
        tft.drawCircle(60, 160, 40, GREEN);  // Круг - рамка
        tft.drawCircle(60, 160, 39, GREEN);  // Круг - рамка
        tft.setTextColor(DarBLUE);
        tft.setTextSize(2);
        tft.setCursor(40, 153);
        tft.println(utf8rus("ВКЛ"));
      } 
      else {
         tft.fillRoundRect(10, 245, 125, 30, 7, BLACK);   //  Кнопка с заливкой
      }

      if (Lum>800) {
        led.disable();  //выключение света когда светло 
      tft.fillRoundRect(10, 245, 125, 30, 7, BLACK);   //  Кнопка с заливкой
       tft.fillCircle(60, 160, 40, BLACK);    // Круглая кнопка с заливкой

      }  
     // выводим полученное значение
       Serial.print(val); Serial.print("   "); Serial.println(int(Lum)); 



      tft.drawLine(211,179-i,229,179-i,BLACK);
     
      i=(Lum/10.2);
      if (i>98) i=98;
      tft.drawLine(211,170,229,170,GREEN);
      tft.drawLine(211,100,229,100,RED);      
      tft.drawLine(211,179-i,229,179-i,GOLD);

      break;  //=====================================================

    // +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
    default:
      //#############################################################
      //##   Regim =    ##      Выполняется функция                ##
      //##---------------------------------------------------------##
      //##     выполняется, если не выбрана ни одна альтернатива   ##
      //#############################################################
      Regim = 1;
      Flag1 = true;
      break;  //=====================================================
  }

  // ##############################################################################
  // ######  КОНЕЦ  ОСНОВНОГО  ЦИКЛА ##############################################
  // ##############################################################################
}

// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//  Подпрограмма рисования начального бордюра
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
void drawBorder() {
  uint16_t width = tft.width() - 1;
  uint16_t height = tft.height() - 1;
  uint8_t border = 10;
  tft.fillScreen(BLUE);
  tft.fillRect(border, border, (width - border * 2), (height - border * 2), WHITE);
}
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
