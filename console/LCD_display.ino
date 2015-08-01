
void DisplayModeTitle(char* title){
  /* Writes a centered line of text out to tthe LCD display.
  
  this version does not use the string library.
  next version will not build intermediate char array - just write directly to display
  
  */
  
   // erase the line by padding out the title with spaces
  byte titlelen = strlen(title);
  int leftpad = (16-titlelen)/2;
  //char* outStr = "                ";
  char* outStr = "----------------";
  byte i, cursor_start ;
  for (i=0; i <16; i++) {
      outStr[i] = ' ';
    }
  
  if ( titlelen == 15 ) {
     strcpy(outStr, title);
  } else if (leftpad>0) {  //center
    for (i=0; i <titlelen; i++) {
      outStr[i+leftpad] = title[i];
    }
  } else { // too wide
  leftpad = -leftpad;
  for (i=0; i <16; i++) {
      outStr[i] = title[i+leftpad];
    }
  }

  lcd.setCursor(0, 0);
  lcd.print(outStr);

}

void ClearSubMode(){
   lcd.setCursor(0, 1);
  lcd.print("                ");
}



void DisplaySubMode(char* title){
  /* Writes a centered line of text out to tthe LCD display.
  
  this version does not use the string library.
  next version will not build intermediate char array - just write directly to display
  
  */
  
   // erase the line by padding out the title with spaces
  byte titlelen = strlen(title);
  int leftpad = (16-titlelen)/2;
  //char* outStr = "                ";
  char* outStr = "----------------";
  byte i, cursor_start ;
  for (i=0; i <16; i++) {
      outStr[i] = ' ';
    }
  
  if ( titlelen == 15 ) {
     strcpy(outStr, title);
  } else if (leftpad>0) {  //center
    for (i=0; i <titlelen; i++) {
      outStr[i+leftpad] = title[i];
    }
  } else { // too wide
  leftpad = -leftpad;
  for (i=0; i <16; i++) {
      outStr[i] = title[i+leftpad];
    }
  }

  lcd.setCursor(0, 1);
  lcd.print(outStr);

}
