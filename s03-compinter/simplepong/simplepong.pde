import processing.serial.*;
import g4p_controls.*;

boolean gameStart = false;

float x = 150;
float y = 150;
float speedX = random(3, 5);
float speedY = random(3, 5);
int leftColor = 128;
int rightColor = 128;
int diam;
int rectSize = 300;
float diamHit;
int posY = 0;

float roll1  = 0.0F; // y
float pitch1 = 0.0F; // z
float yaw1   = 0.0F; // x
float elbow  = 0.0F;
float temp  = 0.0F;
float alt   = 0.0F;

float elbowMult = 480.0/125;

// Serial port state.
Serial       port;
final String serialConfigFile = "serialconfig.txt";
boolean      printSerial = false;

// UI controls.
GPanel    configPanel;
GDropList serialList;
GLabel    serialLabel;
GLabel    calLabel;
GCheckbox printSerialCheckbox;

void setup() {
  size(500, 500);
  noStroke();
  smooth();
  ellipseMode(CENTER);
  
    // Serial port setup.
  // Grab list of serial ports and choose one that was persisted earlier or default to the first port.
  int selectedPort = 0;
  String[] availablePorts = Serial.list();
  if (availablePorts == null) {
    println("ERROR: No serial ports available!");
    exit();
  }
  String[] serialConfig = loadStrings(serialConfigFile);
  if (serialConfig != null && serialConfig.length > 0) {
    String savedPort = serialConfig[0];
    // Check if saved port is in available ports.
    for (int i = 0; i < availablePorts.length; ++i) {
      if (availablePorts[i].equals(savedPort)) {
        selectedPort = i;
      } 
    }
  }
  // Build serial config UI.
  configPanel = new GPanel(this, 10, 10, width-20, 90, "Configuration (click to hide/show)");
  serialLabel = new GLabel(this,  0, 20, 80, 25, "Serial port:");
  configPanel.addControl(serialLabel);
  serialList = new GDropList(this, 90, 20, 200, 200, 6);
  serialList.setItems(availablePorts, selectedPort);
  configPanel.addControl(serialList);
  calLabel = new GLabel(this, 300, 20, 350, 25, "Calibration: Sys=? Gyro=? Accel=? Mag=?");
  configPanel.addControl(calLabel); 
  printSerialCheckbox = new GCheckbox(this, 5, 50, 200, 20, "Print serial data");
  printSerialCheckbox.setSelected(printSerial);
  configPanel.addControl(printSerialCheckbox);
  // Set serial port.
  //setSerialPort(serialList.getSelectedText());
}

void draw() { 
  background(255);

  fill(128,128,128);
  diam = 20;
  ellipse(x, y, diam, diam);

  fill(leftColor);
  rect(0, 0, 20, height);
  fill(rightColor);
  posY = (int)(elbow*elbowMult);
  //rect(width-30, mouseY-rectSize/2, 10, rectSize);
  rect(width-30, posY-rectSize/2, 10, rectSize);

  if (gameStart) {

    x = x + speedX;
    y = y + speedY;

    // if ball hits movable bar, invert X direction and apply effects
    if ( x > width-30 && x < width -20 && y > posY-rectSize/2 && y < posY+rectSize/2 ) {
      speedX = speedX * -1;
      x = x + speedX;
      rightColor = 0;
      fill(random(0,128),random(0,128),random(0,128));
      diamHit = random(75,150);
      ellipse(x,y,diamHit,diamHit);
      rectSize = rectSize-10;
      rectSize = constrain(rectSize, 10,150);      
    } 

    // if ball hits wall, change direction of X
    else if (x < 25) {
      speedX = speedX * -1.1;
      x = x + speedX;
      leftColor = 0;
    }

    else {     
      leftColor = 128;
      rightColor = 128;
    }
    // resets things if you lose
    if (x > width) { 
      gameStart = false;
      x = 150;
      y = 150; 
      speedX = random(3, 5);
      speedY = random(3, 5);
      rectSize = 150;
    }


    // if ball hits up or down, change direction of Y   
    if ( y > height || y < 0 ) {
      speedY = speedY * -1;
      y = y + speedY;
    }
  }
}
void mousePressed() {
  gameStart = !gameStart;
}
void serialEvent(Serial p) 
{
  String incoming = p.readString();
  if (printSerial) {
    println(incoming);
  }
  
  if ((incoming.length() > 8))
  {
    String[] list = split(incoming, " ");
    if ( (list.length > 0) && (list[0].equals("Orientation:")) ) 
    {
      roll1  = float(list[3]); // Roll = Z
      pitch1 = float(list[2]); // Pitch = Y 
      yaw1   = float(list[1]); // Yaw/Heading = X
      elbow  = float(list[4]);
    }
    if ( (list.length > 0) && (list[0].equals("Alt:")) ) 
    {
      alt  = float(list[1]);
    }
    if ( (list.length > 0) && (list[0].equals("Temp:")) ) 
    {
      temp  = float(list[1]);
    }
    if ( (list.length > 0) && (list[0].equals("Calibration:")) )
    {
      int sysCal   = int(list[1]);
      int gyroCal  = int(list[2]);
      int accelCal = int(list[3]);
      int magCal   = int(trim(list[4]));
      calLabel.setText("Calibration: Sys=" + sysCal + " Gyro=" + gyroCal + " Accel=" + accelCal + " Mag=" + magCal);
    }
  }
}

// Set serial port to desired value.
void setSerialPort(String portName) {
  // Close the port if it's currently open.
  if (port != null) {
    port.stop();
  }
  try {
    // Open port.
    port = new Serial(this, portName, 115200);
    port.bufferUntil('\n');
    // Persist port in configuration.
    saveStrings(serialConfigFile, new String[] { portName });
  }
  catch (RuntimeException ex) {
    // Swallow error if port can't be opened, keep port closed.
    port = null; 
  }
}

// UI event handlers

void handlePanelEvents(GPanel panel, GEvent event) {
  // Panel events, do nothing.
}

void handleDropListEvents(GDropList list, GEvent event) { 
  // Drop list events, check if new serial port is selected.
  if (list == serialList) {
    setSerialPort(serialList.getSelectedText()); 
  }
}

void handleToggleControlEvents(GToggleControl checkbox, GEvent event) { 
  // Checkbox toggle events, check if print events is toggled.
  if (checkbox == printSerialCheckbox) {
    printSerial = printSerialCheckbox.isSelected(); 
  }
}
