// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//                  Подпрограмма опроса сенсорного экрана
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
void ReadTocScrin () {
  TSPoint p = ts.getPoint();
   pinMode(XM, OUTPUT); //Pins configures again for TFT control
   pinMode(YP, OUTPUT);
   TCx = map(p.x, TS_MAXX ,TS_MINX , 0, tft.width());
   TCy = map(p.y, TS_MINY, TS_MAXY, 0, tft.height());;
   //TCx = p.x; // Координаты нажатия тачскрина X
   //TCy = p.y; // Координаты нажатия тачскрина Y
   TCz = p.z; // Сила нажатия тачскрина
   TCpus= false; // Флаг нажатия экрана
    //Serial.print("X = "); Serial.print(TCx);
    //Serial.print("\t    Y = "); Serial.print(TCy);
    //Serial.print("\t Pres= "); Serial.println(TCz);
    if (p.z > MINPRESSURE && p.z < MAXPRESSURE) TCpus= true;
    //Serial.print("\tPres= "); Serial.println(TCpus);
}
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
