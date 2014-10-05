void showLadderValue(){
  unsigned int v;
  v=analogRead(TEAM1PIN);
  char outnum[10];
  sprintf(outnum, "%04d   ", v);
   lcd.setCursor(0, 0);
  //  lcd.print(v);
    lcd.print(outnum);
  //  delay(500);
  
}
