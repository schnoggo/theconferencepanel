
void Panel(){
  // light panel
dprintln("Panel() ... ");
  int startingmode = displaymode;
  colorWipe(Color(255, 240, 240), 0);

   while (displaymode == startingmode) {
         ServiceBackground();
   }
   // Serial.println("Penale Cleanup()");
   colorWipe(Color(0, 0, 0), 0);
}


void LavaTest(){
  // Serial.print("TestSpokes() ... ");
  int startingmode = displaymode;
  int generation, i, j;
int nx, ny, neighbor;
  uint8_t f[MAX_DISPLAY_ANGLE+1][MAX_DISPLAY_RADIUS+1][2]; // o=age, 1=color
  uint8_t last_gen[MAX_DISPLAY_ANGLE+1][MAX_DISPLAY_RADIUS+1][2];
  uint8_t neighbor_list[8][2];
  uint8_t neighbor_count = 0;
  int drift, c;
  uint32_t color_list[16];

  for (i=1; i <16; i++) {
    color_list[0]=0;
      color_list[i]=Color(random(256), random(256), random(256));
  }
  generation=0;
  // Initialize board with random ages
     for (j=0; j <=MAX_DISPLAY_ANGLE; j++) {
        for (i=0; i <=MAX_DISPLAY_RADIUS; i++) {
          f[j][i][1]=0;
          f[j][i][0]=random(256);
          setPolarPixelColor(j, i, color_list[f[j][i][1]]);
        }
     }
  strip.show();

  while (displaymode == startingmode) {
        //ServiceBackground();
        BackGroundDelay(40);
        // copy last gen:
       for (j=0; j <=MAX_DISPLAY_ANGLE; j++) {
        for (i=0; i <=MAX_DISPLAY_RADIUS; i++) {
          last_gen[j][i][0]=f[j][i][0];
          last_gen[j][i][1]=f[j][i][1];
        }
       }
       if (generation++ >30) {
         // drop a seed crystal
        generation=0;
        nx=random(MAX_DISPLAY_ANGLE+1);
        ny=random(MAX_DISPLAY_RADIUS+1);
        last_gen[nx][ny][1]=color_list[random(16)];
        last_gen[nx][ny][0]=255;
       }

      for (j=0; j <=MAX_DISPLAY_ANGLE; j++) {
        for (i=0; i <=MAX_DISPLAY_RADIUS; i++) {
          // find neighbors:
          neighbor_count = 0;
          // (-1,-1) - NW
          if (i>0 && j>0) {
            neighbor_list[neighbor_count][0]=j-1;
            neighbor_list[neighbor_count][1]=i-1;
            neighbor_count++;
          }

          // (-1,0) W
          if (j>0 ) {
            neighbor_list[neighbor_count][0]=j-1;
            neighbor_list[neighbor_count][1]=i;
            neighbor_count++;
          }

          // (-1,+1) SW
          if (j>0 && i<MAX_DISPLAY_RADIUS) {
            neighbor_list[neighbor_count][0]=j-1;
            neighbor_list[neighbor_count][1]=i+1;
            neighbor_count++;
          }

          // (0,-1) N
          if ( i>0) {
            neighbor_list[neighbor_count][0]=j;
            neighbor_list[neighbor_count][1]=i-1;
            neighbor_count++;
          }

           // (0,-1) S
          if ( j>0) {
            neighbor_list[neighbor_count][0]=j-1;
            neighbor_list[neighbor_count][1]=i;
            neighbor_count++;
          }

           // (+1,+1) SE
          if ( j<MAX_DISPLAY_ANGLE && i<MAX_DISPLAY_RADIUS) {
            neighbor_list[neighbor_count][0]=j+1;
            neighbor_list[neighbor_count][1]=i+1;
            neighbor_count++;
          }

           // (+1,0) E
          if ( j<MAX_DISPLAY_ANGLE ) {
            neighbor_list[neighbor_count][0]=j+1;
            neighbor_list[neighbor_count][1]=i;
            neighbor_count++;
          }

           // (+1,-1) NE
          if ( j<MAX_DISPLAY_ANGLE && i>0) {
            neighbor_list[neighbor_count][0]=j+1;
            neighbor_list[neighbor_count][1]=i-1;
            neighbor_count++;
          }

        // pick a random neighbor:
        neighbor=random(neighbor_count);
        // perform generation update:
        nx=neighbor_list[neighbor][0];
        ny= neighbor_list[neighbor][1];
        if (last_gen[nx][ny][1] != last_gen[j][i][1]){ //different colors?
        // make sure they live a little while:
        if (last_gen[nx][ny][0]<230){
          if ( random(last_gen[j][i][0]) > random(last_gen[nx][ny][0])){
            // infect neighboring cell
            f[nx][ny][0]=255;
            f[nx][ny][1]=last_gen[j][i][1];
          }
        }
        }

        }
     }
   // update generation and display
            for (j=0; j <=MAX_DISPLAY_ANGLE; j++) {
        for (i=0; i <=MAX_DISPLAY_RADIUS; i++) {
                 setPolarPixelColor(j, i, color_list[f[j][i][1]]);
                 if (f[j][i][0]>0){
                   f[j][i][0]=f[j][i][0]-1;
                 }

        }
       }


     strip.show();
    BackGroundDelay(10);
  }
  // Serial.println("TestSpokes Cleanup");
}


void TestSpokes(){
  // Serial.print("TestSpokes() ... ");
  int startingmode = displaymode;
  int i,j;
  uint32_t clr;
  while (displaymode == startingmode) {

     clr=Color(random(256), random(256), random(256));
    for (i=0; i <=MAX_DISPLAY_ANGLE; i++) { // light up left ot right
      LightSpoke(i,clr);
     BackGroundDelay(60);
    }
     // clr=Color(random(256), random(256), random(256));
     clr=0;
     for (i=MAX_DISPLAY_RADIUS; i >= 0; i--) { // erase right to left
      LightSpoke(i,clr);
     BackGroundDelay(60);
    }
     BackGroundDelay(100);
  }
  //Serial.println("TestSpokes cleanup");
}


void VUMeter(){
  // Serial.print("TestSpokes() ... ");
  unsigned int display_spokes = MAX_DISPLAY_ANGLE; // number of "spokes" in display (0 based)
  unsigned int startingmode = displaymode;
  int i,j;
  unsigned int analog_reading = 0;
  int last_display_value, new_display_value = 0;
  float slice_width;
  uint32_t clr;


  // Clear the display:
     for (i=0; i < display_spokes; i++) { // light up left ot right
      LightSpoke(i,0); // turn it off
    }
    clr=Color(256, 256, 100);
    // draw the first bar:
    last_display_value=0;
    LightSpoke(0,clr);
  while (displaymode == startingmode) {

  // analog_reading = analogRead(AUDIO_IN_PIN); // eventually change this to an averaged sample
   analog_reading = avg_audio_sample;
   if (analog_reading>1022){analog_reading=1022;} //little sanity check and cap prevention
   slice_width=1024/(MAX_DISPLAY_ANGLE+2); // extra slice for "0," extra range to prevent hitting cap
  new_display_value = analog_reading/slice_width; // round it back into the unsigned int
  // new_display_value is now 0-10
  new_display_value--; // -1 to 9

/*
   Serial.print("analog ");
   Serial.print(analog_reading);
   Serial.print(",  scaled: ");
   Serial.print(new_display_value);
   Serial.print(",  last: ");
   Serial.println(last_display_value);
  */


/*
     if (new_display_value < 0) {
       for (i=last_display_value; i= 0; i--)
       LightSpoke(i,0);
       last_display_value = new_display_value;
     } else {
   if (new_display_value > last_display_value) {
    for (i=last_display_value+1; i <= new_display_value; i++) { // light up left to right
      LightSpoke(i,clr);
      last_display_value = new_display_value;
    }
   } else if (new_display_value < last_display_value) {
     for (i=last_display_value; i > new_display_value; i--) { // erase right to left
      LightSpoke(i,0);
     last_display_value = new_display_value;
    }
   }
     }
   // and do nothing if the values are equal.
*/

     for (i=0; i <= display_spokes; i++) { // light up left ot right
     if (i<5){
       clr=Color(20, 255, 20);
     } else {
       if (i<8){
         clr=Color(255, 255, 00);
       } else {
         clr=Color(255, 00, 00);
       }
     }
     if (i <= new_display_value){
      LightSpoke(i,clr);
     } else {
       LightSpoke(i,0); // turn it off
     }
    }

     BackGroundDelay(30);
  }
  //Serial.println("TestSpokes cleanup");
}

void Spinner(){
dprintln("Spinner() ... ");
    int startingmode = displaymode;
    int wheelSlotCount=MAX_DISPLAY_ANGLE+MAX_DISPLAY_RADIUS+1; //display is only half-circle
    int i, j, x, wheelRot;
    int spoke;
    int WheelPos;
    int velocity = 1;
    int acceleration = 4; // signed step size
    int accel_step = 0; //actual accel =  1/ acceleration
    int friction = 2;
    int impetus_duration = 10;

  wheelRot=0;
  int r,g,b;
 int  rTot, gTot, bTot=0;
   uint32_t spinnercolors[wheelSlotCount];
   uint32_t spinnerrgb[wheelSlotCount][4];
   spinnercolors[0]=0; // make sure there's a black line

   // Fill in the colors of the wheel:
  for (i=1; i < wheelSlotCount; i++){
   // spinnercolors[i]=Color((random(16)*16)+15, (random(16)*16)+15, (random(16)*16)+15);
   //spinnercolors[i]=Wheel(i*(256/wheelSlotCount));
   WheelPos=i*(256/wheelSlotCount);


   if (WheelPos < 85) {
     r=WheelPos * 3;
     g=255-r;
     b=0;

  } else if (WheelPos < 170) {
   WheelPos -= 85;
     b=WheelPos * 3;
     r=255-b;
     g=0;
  } else {
   WheelPos -= 170;
    g=WheelPos * 3;
    b=255-g;
    r=0;
  }

   spinnercolors[i]=Color(r,g,b);
   spinnerrgb[i][0]=r;
   spinnerrgb[i][1]=g;
   spinnerrgb[i][2]=b;

}
  for (wheelRot=0; wheelRot <wheelSlotCount; wheelRot++){

  for (i=0; i <wheelSlotCount; i++){
     x=i-wheelRot;
       if (x<0){
       x=x+(wheelSlotCount-1);
     }
/*
    int acc[3];
    int k;
    for(j=0; j<3; j++){
     spinnerrgb
  }
*/
  }
  }
   while (displaymode == startingmode) {
  //  ServiceBackground();
   // draw each spoke with updated color


     for (spoke=0; spoke <= MAX_DISPLAY_ANGLE; spoke++){
       x = spoke - wheelRot;
       if (x<0){
       x=x+(wheelSlotCount-1);
     }

       for (j=1; j <= MAX_DISPLAY_RADIUS; j++){ // dont need to update the center
        setPolarPixelColor(spoke, j, spinnercolors[x]);
       }
     }
   // set the center
  strip.show();
    // BackGroundDelay(50);
     BackGroundDelay(100-abs(velocity));
     // apply acceleration:
     if (impetus_duration > 0){
       impetus_duration--;
       if (accel_step++ > acceleration){
         velocity = velocity + 1;
       }
     }
      velocity = velocity - friction;
     // bound velocity:
     velocity = min(max(velocity, 0), 80);


     wheelRot++;
     if (wheelRot>=wheelSlotCount){
       wheelRot=0;
    }
  }
  // Serial.println("Spinner cleanup");
}

void TestRings(){
  // Serial.print("TestRings() ... ");
  int startingmode = displaymode;
  int i,j;
  uint32_t clr;

   while (displaymode == startingmode) {
     clr=Color(random(256), random(256), random(256));
    for (i=0; i <= MAX_DISPLAY_RADIUS; i++) {
      LightRing(i,clr);

     BackGroundDelay(60);
    }

   /*
   clr=0;
     for (i=4; i >= 1; i--) {
      LightRing(i,clr);
     BackGroundDelay(60);
    }

    */
  }
    // Serial.println("TestRings cleanup");
}

void AnimatedRings(){
  dprintln("AnimatedRings() ... ");
  int startingmode = displaymode; // global display mode
  int rmax = 0;
  int newRing, delta;
  uint32_t clr, oldColor;

  LightRing(rmax,clr); // first pixel
  int redCol=(random(16)*8);
  int grnCol=(random(16)*8);
  int bluCol=(random(16)*8);

  clr=Color(redCol+120, grnCol+120,bluCol+120);
 oldColor=Color( (136-redCol)/4, (136-grnCol)/4, (136-bluCol)/4 );

   FillRadial(4, oldColor);
  while (displaymode == startingmode) { // if an interupt changed the mode, exit
    delta=random(3)-1;

    newRing=rmax + delta;
    if ( (newRing<0) or (newRing>3) ){
      newRing=rmax;
    }

    if (newRing > rmax){
      LightRing(rmax,clr);
    } else if (newRing < rmax){
      LightRing(rmax,oldColor);
    }
      rmax = newRing;
      BackGroundDelay(30);

  }
  // Serial.println("AnimatedRings cleanup");

}
