void InitPolarCoords(){
  // global: polar
  // For the polar coordinate mapping on the arc display:
        polar[0][0]=-1;
	polar[1][0]=-1;
	polar[2][0]=-1;
	polar[3][0]=-1;
	polar[4][0]=-1;
	polar[5][0]=-1;
	polar[6][0]=-1;
  polar[7][0]=-1;
  polar[8][0]=-1;
  polar[9][0]=-1;

polar[0][1]=4;
polar[1][1]=5;
polar[2][1]=14;
polar[3][1]=15;
polar[4][1]=24;
polar[5][1]=25;
polar[6][1]=34;
polar[7][1]=35;
polar[8][1]=44;
polar[9][1]=45;

polar[0][2]=3;
polar[1][2]=6;
polar[2][2]=13;
polar[3][2]=16;
polar[4][2]=23;
polar[5][2]=26;
polar[6][2]=33;
polar[7][2]=36;
polar[8][2]=43;
polar[9][2]=46;

polar[0][3]=2;
polar[1][3]=7;
polar[2][3]=12;
polar[3][3]=17;
polar[4][3]=22;
polar[5][3]=27;
polar[6][3]=32;
polar[7][3]=37;
polar[8][3]=42;
polar[9][3]=47;

polar[0][4]=1;
polar[1][4]=8;
polar[2][4]=11;
polar[3][4]=18;
polar[4][4]=21;
polar[5][4]=28;
polar[6][4]=31;
polar[7][4]=38;
polar[8][4]=41;
polar[9][4]=48;

polar[0][5]=0;
polar[1][5]=9;
polar[2][5]=10;
polar[3][5]=19;
polar[4][5]=20;
polar[5][5]=29;
polar[6][5]=30;
polar[7][5]=39;
polar[8][5]=40;
polar[9][5]=49;
}

int Polar2Pixel(int angle, int radius){

}

void setPolarPixelColor(int angle, int radius, uint32_t color){
  // inputs:
  //    angle. (0-9) for this version
  // radius (0-5) for this version
  //    color = color (32-bit packed)
  //
  // globals: strip
  if (radius > 0){
      strip.setPixelColor(polar[angle][radius], color);
    }
}
void LightSpoke(int a, uint32_t c){
  // inputs:
  //    a = angle. (0-6) for this version
  //    c = color (32-bit packed)

   int i;
    for (i=0; i <= MAX_DISPLAY_RADIUS; i++){
      setPolarPixelColor(a, i, c);
    }

  strip.show();
}

void LightRing(int r, uint32_t c){
  // inputs:
  //    r = radius. (0-3 for this version)
  //    c = color (32-bit packed)
 int i;
  for (i=0; i <= MAX_DISPLAY_ANGLE; i++){
    setPolarPixelColor(i, r, c);
  }
  strip.show();
}


void FillRadial(int r, uint32_t c){
 int i;
  for (i=0; i < r; i++){
    LightRing(i, c);

  }
}
