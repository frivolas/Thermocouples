import processing.serial.*;

// Serial variables
Serial myPort;
float[] serialInArray = new float[6];
int serialCount=0;
boolean firstContact = false;

// temp logging variables
float tempA;
float tempX;
float tempZ;
float tempAD;
float tempXD;
float tempT;
int refreshRate = 1000;     //ms
float midThreshold = 45;    //Centigrade, based on motor operating temp
float highThreshold = 85;   //Centigrade, based on motor max temp

// misc variables
float textXpos = 0;
float textYpos = 0;
float textWidth = 35;
float pad = 40;
float labelYpos = 0;
PFont font;

// bar plot variables
float barWidth=0;
float barHeight0;
float maxBarHeight = 0;
float maxTemp = 100; //100C

// File variables
String theHeader = "Time,Bend Temp,Feed Temp,Duck Temp,Bend Drv,Feed Drv,Box Temp";
String theFile;
PrintWriter output;

void setup()
{
  size(650,400);
  background(255);
  font = createFont("arial",20);
  printArray(Serial.list());
  myPort = new Serial(this, Serial.list()[0], 9600);
  
  theFile = thePath();
  output = createWriter(theFile);
  output.println(theHeader);
  
  textXpos = width/6;
  textYpos = height-50;
  labelYpos = height - height + 65; 
  
  barWidth = textWidth+5;
  maxBarHeight = height-150;
}


void draw()
{
  background(255);
  textSize(20);
  fill(0);
  text("DIWire Temp Logger", textXpos-80, 30);
  showtemp();
  drawBars(tempA, tempX, tempZ, tempAD, tempXD, tempT);
  showGrid();
  textSize(10);
  fill(0);
  text("Refresh rate: \n" + str(refreshRate) + " ms", width-100, labelYpos+20);
  text("Press ESC to quit and save the data to: \n" + theFile, textXpos-90, height-20);
  textSize(12);
  text(theTime(),width-80, height - 15);
  
}


void serialEvent(Serial myPort) {
  // read a byte from the serial port:
  int inByte = myPort.read();
  // if this is the first byte received, and it's an A,
  // clear the serial buffer and note that you've
  // had first contact from the microcontroller. 
  // Otherwise, add the incoming byte to the array:
  if (firstContact == false) {
    if (inByte == 'A') { 
      myPort.clear();          // clear the serial port buffer
      firstContact = true;     // you've had first contact from the microcontroller
      myPort.write('R');       // ask for more
    } 
  } else {
    //println("Byte!");
    // Add the latest byte from the serial port to array:
    serialInArray[serialCount] = inByte;
    serialCount++;

    // If we have 6 bytes:
    if (serialCount > 5 ) {
      tempA = serialInArray[4];
      tempX = serialInArray[5];
      tempZ = serialInArray[0];
      tempAD = serialInArray[1];
      tempXD = serialInArray[2];
      tempT = serialInArray[3];

      // print the values to the terminal and to the file
      println(theTime() + "\t" +tempA + "\t" + tempX + "\t" + tempZ + "\t" + tempAD + "\t" + tempXD + "\t" + tempT);
      output.println(theTime() + "," + tempA + "," + tempX + "," + tempZ + "," + tempAD + "," + tempXD + "," + tempT);
      output.flush();
      // Wait a sec and send a capital R to request new sensor readings:
      delay(refreshRate);
      myPort.write('R');
      // Reset serialCount:
      serialCount = 0;
    }
    }
  }



void showtemp()
{
fill(0,0,0);
textAlign(LEFT);
textFont(font, 16);
text(str((float)tempA) + " C", textXpos, textYpos);
text(str((float)tempX) + " C", textXpos + textWidth+pad, textYpos);
text(str((float)tempZ) + " C", textXpos + 2*textWidth + 2*pad, textYpos);
text(str((float)tempAD) + " C", textXpos + 3*textWidth + 3*pad, textYpos);
text(str((float)tempXD) + " C", textXpos + 4*textWidth + 4*pad, textYpos);
text(str((float)tempT) + " C", textXpos + 5*textWidth + 5*pad, textYpos);
}


void thermosError(){
  text("ERROR WITH THERMOCOUPLES", width/2, 100);
}



void drawBars(float a, float x, float z, float ad, float xd, float t){
  float[] heights = new float[6];
  color[] colors = new color[6];
  float[] temps = {a,x,z,ad,xd,t};
  
  heights[0] = map(a,0,maxTemp,0,maxBarHeight);
  heights[1] = map(x,0,maxTemp,0,maxBarHeight);
  heights[2] = map(z,0,maxTemp,0,maxBarHeight);
  heights[3] = map(ad,0,maxTemp,0,maxBarHeight);
  heights[4] = map(xd,0,maxTemp,0,maxBarHeight);
  heights[5] = map(t,0,maxTemp,0,maxBarHeight);
  
  for(int i=0; i<colors.length;i++){
   if(temps[i] < midThreshold) {colors[i] = color(0,255,0);}
   else if((midThreshold <= temps[i]) && (temps[i] < highThreshold)) {colors[i] = color(255,165,0);}
   else if(temps[i] > highThreshold) {colors[i] = color(255,0,0);}
  }
  
  //printArray(heights); 
   
  for(int i=0; i<heights.length;i++){
   noStroke();
   float barXo = textXpos + i*textWidth + i*pad;
   float barY0 = textYpos - heights[i] - 30;
   float barXf = barWidth;
   float barYf = heights[i];
   fill(colors[i]);
   rect(barXo,barY0,barXf,barYf);
  }
  
}


void showGrid(){
   for(int i=0; i<6;i++){
   float barXo = textXpos + i*textWidth + i*pad;
   float barY0 = textYpos - maxBarHeight - 30;
   float barXf = barWidth;
   float barYf = maxBarHeight;
   noFill();
   stroke(0);
   rect(barXo,barY0,barXf,barYf);
  }
  
  fill(0,0,0);
textAlign(LEFT);
textFont(font, 14);
text("Bend", textXpos, labelYpos);
text("Feed", textXpos + textWidth+pad, labelYpos);
text("Duck", textXpos + 2*textWidth + 2*pad, labelYpos);
text("A Drv", textXpos + 3*textWidth + 3*pad, labelYpos);
text("X Drv", textXpos + 4*textWidth + 4*pad, labelYpos);
text("Box", textXpos + 5*textWidth + 5*pad, labelYpos);
  
} 



String theDate(){
  String theMinute, theSec;
  int y = year();
  int mo = month();
  int d = day();
  int h = hour();
  int mi = minute();
  int s = second();

  if(mi<10) theMinute = "0" + mi;
  else theMinute = "" + mi;

  if(s<10) theSec = "0" + s;
  else theSec = "" + s;

  String dateString = y + "" + mo + "" + d + "-" + h + theMinute + theSec;

  return dateString;
}


String theTime(){
  String theMinute, theSec;
  int h=hour();
  int mi=minute();
  int sec = second();
  
  if(mi<10) theMinute = "0" + mi;
  else theMinute = "" + mi;

  if(sec<10) theSec = "0" + sec;
  else theSec = "" + sec;

  String timeString = "[" + h + ":" + theMinute + ":" + theSec + "] ";
  return timeString;
}

String thePath(){
 String fileName = dataPath("") + theDate() + ".csv";
 return fileName;
}

void keyPressed(){
  if(key == ESC){
    fill(255);
    stroke(0);
    rect(width/4,height/4,width/2,height/4);
    fill(0);
    textSize(25);
    text("Saving Log file, \ndon't close this window",width/4+25,height/4+40);
    println("SAVING FILE!");
    output.flush();
    output.close();

    exit();
  }
}