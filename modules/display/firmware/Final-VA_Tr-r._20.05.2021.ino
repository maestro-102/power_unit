               // Прибор умеет измерять ток до 999 Ампер, напряжение до 36(999 с делителем) Вольт и мощность до 999000 Ватт.//
               //  В строки, где после // стоит значек < можно вносить изменения по мере надобности. Остальное трогать нельзя.//
//перечень цветов: BLACK NAVY DARKGREEN DARKCYAN MAROON PURPLE OLIVE LIGHTGREY DARKGREY BLUE GREEN CYAN RED MAGENTA YELLOW WHITE ORANGE GREENYELLOW PINK 
#include "INA226.h"  // INA Library
#include "SPI.h"
#include "TFT_ST7735.h"
TFT_ST7735 tft = TFT_ST7735();
const uint32_t SHUNT_MICRO_OHM = 5000;  ///< Сопротивление шунта в микроомах, например 100000 - это 0,1 Ом, изменяя значение шунта в этой строчке,
//                                             калибруют амперметр, минимальное значение = 50.
const uint16_t MAXIMUM_AMPS    = 10;       ///< Максимальное  измеряемое значение тока, значения кратное 1 А - ограничено до 1022 А
//INA_Class      INA;
INA226_Class INA226;                      ///< INA class instantiation

//---Здесь хранятся все переменные
float  V,A, W , Ah , Wh ;
unsigned long  new_Millis ;

void setup() {
  Serial.begin(9600);
  INA226.begin( MAXIMUM_AMPS, SHUNT_MICRO_OHM);  // Set to the expected Amp maximum and shunt resistance
  INA226.setBusConversion(8244);             // Время конверсии в микросекундах (140,204,332,588,1100,2116,4156,8244)8244µs=8.244 ms
  INA226.setShuntConversion(8244);           // Время конверсии в микросекундах (140,204,332,588,1100,2116,4156,8244)8244µs=8.244 ms
  INA226.setAveraging(8);                   //< Среднее количество чтений n раз (1,4,8,16,32,64,128,256,512,)рекомендую 4(8) для ЛБП и 16 для сварочного аппарата
  INA226.setMode(INA_MODE_CONTINUOUS_BOTH);  // Шина / шунт измеряется постоянно
  tft.init();                 // Инициализация дисплея.
  tft.setRotation(3);         //< Переворачиваем дисплей.Возможны два варианта 1 и 3, ставим как удобней.
  tft.fillScreen(TFT_BLACK);  // Указываем цвет заливки дисплея
  
                       //устанавливаем цифровые pin как выход  (в этом проекте нам доступны вывода D2, D3, D4, D5, D6, D10, D12)
  pinMode(2, OUTPUT);         // устанавливаем pin 2 как выход 
  pinMode(3, OUTPUT);         // устанавливаем pin 3 как выход
  pinMode(4, OUTPUT);         // устанавливаем pin 4 как выход 
  pinMode(5, OUTPUT);         // устанавливаем pin 5 как выход
  pinMode(6, OUTPUT);         // устанавливаем pin 6 как выход 
  pinMode(10, OUTPUT);        // устанавливаем pin 10 как выход
  pinMode(12, OUTPUT);        // устанавливаем pin 12 как выход
  
              //--- Рисуем рамку.
  //tft.fillRect(-1, -2,160, 128, ST7735_LIGHTGREY);  //< Для визуализации рамки в начале этой и начале следующей строке удалить значки "слэш". Они выглядят так //
  //tft.fillRect(1, 0, 156, 124, ST7735_BLACK);     //< При этом придется опустиь вниз на несколько позиций первую строку и поднять третью на 5 позиций
  
  //---В этом месте все статические данные, которые будут отображаться на дисплее
  tft.setTextColor(ST7735_GREEN, ST7735_BLACK);        //<  цвет текста , цвет заливки текста "А"
  tft.drawRightString("A", 150, 92, 4);                //<  Текст буква "А" , положение по оси Х , положение по оси Y , размер шрифта
  tft.setTextColor(ST7735_WHITE, ST7735_BLACK  );      //<  цвет текста , цвет заливки текста "Watt"
  tft.drawRightString("W", 146, 54, 2);                //<  Текст буква "W" , положение по оси Х , положение по оси Y , размер шрифта
  tft.setTextColor(ST7735_ORANGE, ST7735_BLACK);       //<  цвет текста , цвет заливки текста "V"
  tft.drawRightString("V", 150, 26, 4);                //<  Текст буква "V" , положение по оси Х , положение по оси Y , размер шрифта
  tft.setTextColor(ST7735_PINK, ST7735_BLACK);         //<  цвет текста , цвет заливки текста "Ah"
  tft.drawRightString("Ah", 75, 54, 2);                //<  Текст буква "Ah" , положение по оси Х , положение по оси Y , размер шрифта
    new_Millis = millis();
}
  void loop() {                   //--- Расчет всех динамических данных.---
  V = INA226.getBusMilliVolts() / 10e2;
  A = INA226.getBusMicroAmps()  /10e3;
  W = INA226.getBusMicroWatts() / 10e5;
  if (V<0){V=0;}
   if (A<0){A=0;}
    if (W<0){W=0;}
    V = V * 1.0 ;             // Коєффициент кратности табло вольтметра (и калибровка). Значение может быть от 0,1 до 30
    W = A * V  ;                                     // Расчет мощности в W, 
  Ah += A * (millis() - new_Millis) / 3600000 * 1;   //расчет емкости  в Ач
  new_Millis = millis();
   
  //--- Отображение всех динамических данных.---
  char V_out[7]; dtostrf( V , 5, 2, V_out);
  char A_out[8]; dtostrf( A , 7, 2, A_out);
  char W_out[8]; dtostrf( W , 7, 2, W_out);
  char Ah_out[8];
    
  tft.setTextColor( ST7735_ORANGE, ST7735_BLACK);         //<  цвет текста , цвет заливки  "Вольтметр" 
  if (V < 9.99) {
    dtostrf(V , 5, 3, V_out);
    tft.drawRightString(V_out, 132, 9, 6);              //<  положение и размер шрифта табло  "Вольтметр" < 9.99
  }
  else if (V >= 10 && V < 100 )   {
    dtostrf( V , 5, 2, V_out);
    tft.drawRightString(V_out, 132, 9, 6);             //<  положение и размер шрифта табло  "Вольтметр" >= 10 < 100
  }
  else if (V >= 100 && V < 1000 )   {
    dtostrf( V , 5, 1, V_out);
    tft.drawRightString(V_out, 132, 9, 6);             //<  положение и размер шрифта табло  "Вольтметр" >= 100 < 1000
  }
  
    tft.setTextColor(ST7735_GREEN, ST7735_BLACK );       //<  цвет текста,  цвет заливки "Амперметр"
    if (A < 9.99) {
    dtostrf(A , 5, 3, A_out);
    tft.drawRightString(A_out, 132, 76, 6);              //<  положение и размер шрифта табло "Амперметр" < 9.99
  }
  else if (A >= 10  && A < 100 )   {
    dtostrf( A , 5, 2, A_out);
    tft.drawRightString(A_out, 132, 76, 6);             //<  положение и размер шрифта табло "Амперметр" >= 10 < 100
  } 
  else if (A >= 100  && A < 1000 )   {
    dtostrf( A , 5, 1, A_out);
    tft.drawRightString(A_out, 132, 76, 6);             //<  положение и размер шрифта табло "Амперметр" >= 100 
  } 
  
    tft.setTextColor(ST7735_WHITE, ST7735_BLACK );      //<  цвет текста,  цвет заливки табло "Ватметр"     
    if (W < 9.99) {
    dtostrf(W , 6, 4, W_out);
    tft.drawRightString(W_out, 131, 54, 2);             //<  положение и размер шрифта табло  "Ватметр" < 9.99
  }
  else if (W >= 10 && W < 100)   {
    dtostrf( W , 6, 3, W_out);
    tft.drawRightString(W_out, 131, 54, 2);             //<  положение и размер шрифта табло "Ватметр" >= 10  < 100
  }
  else if (W >= 100 && W < 1000 )   {
    dtostrf( W , 6, 2, W_out);
    tft.drawRightString(W_out, 131, 54, 2);             //<  положение и размер шрифта табло "Ватметр" >= 100  < 1000
  }
  else if (W >= 1000  && W < 10000 )   {
    dtostrf( W , 6, 1, W_out);
    tft.drawRightString(W_out, 131, 54, 2);             //<  положение и размер шрифта табло "Ватметр" >= 1000 < 10000
  }
  else if (W >= 10000  && W < 100000 )   {
    dtostrf( W , 6, 0, W_out);
    tft.drawRightString(W_out, 131, 54, 2);             //<  положение и размер шрифта табло "Ватметр" >= 10000 < 100000
  }
  else if (W >= 100000  && W < 1000000 )   {
    dtostrf( W , 6, 0, W_out);
    tft.drawRightString(W_out, 131, 54, 2);             //<  положение и размер шрифта табло "Ватметр" >= 100000 
  }
  
 tft.setTextColor(ST7735_PINK, ST7735_BLACK);        //<  цвет текста, цвет заливки табло "A\час"  
  if (Ah <  9.99) {
    dtostrf( Ah , 6, 3, Ah_out);
    tft.drawRightString(Ah_out, 57 , 54, 2);          //<  положение и размер шрифта табло "A\час"  < 9.99
  }
  else if (Ah >= 10 && Ah < 100) {
    dtostrf( Ah , 6, 2, Ah_out);
    tft.drawRightString(Ah_out, 57 , 54, 2);          //<  положение и размер шрифта табло "A\час" >= 10  < 100
  }
  else if (Ah >= 100 && Ah < 1000) {
    dtostrf( Ah , 6, 1, Ah_out);
    tft.drawRightString(Ah_out, 57 , 54, 2);          //<  положение и размер шрифта табло "A\час" >= 100  < 1000
  }
  else if (Ah >= 1000 && Ah < 10000 ) {              
    dtostrf( Ah , 6, 0, Ah_out);
    tft.drawRightString(Ah_out, 57 , 54, 2);           //<  положение и размер шрифта табло "A\час" >= 1000 
}

                 //< Программируем функции сигналов на выводах D2, D3, D4, D5, D6, D10, D12
                 
  if (V >= 5){                     //< Условие подачи высокого уровня на ногу 2
   digitalWrite(2,HIGH) ;  }
   if (V < 4.5){
  digitalWrite (2,LOW) ;  }
    
  if (V >= 8){                   //< Условие подачи высокого уровня на ногу 3
   digitalWrite(3,HIGH) ;  }
   if (V < 7.5){
  digitalWrite (3,LOW) ;  }
 
  if (V >= 11){                   //< Условие подачи высокого уровня на ногу 4
   digitalWrite(4,HIGH) ;  }
   if (V < 10.5){
  digitalWrite (4,LOW) ;
  }
  if (V >= 14){                   //< Условие подачи высокого уровня на ногу 5
 digitalWrite(5,HIGH) ;  }    
   if (V < 13.5){
  digitalWrite (5,LOW) ;
  }
   if (V >= 17){                   //< Условие подачи высокого уровня на ногу 6
  digitalWrite(6,HIGH) ;  }
  if (V < 16.5){
  digitalWrite (6,LOW) ;
  } 
   if (V >= 20){                   //< Условие подачи высокого уровня на ногу 10
  digitalWrite(10,HIGH) ;  }
  if (V < 19.5){
  digitalWrite (10,LOW) ;
  } 
  if (V >= 23){                   //< Условие подачи высокого уровня на ногу 12
  digitalWrite(12,HIGH) ;  }
  if (V < 22.5){
  digitalWrite (12,LOW) ;
  }  }
