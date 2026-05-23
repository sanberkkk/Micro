int soundPin = A0;
int lampPin = 9;
int level = 1;
int brightness[] = {0, 26, 64, 128, 192, 255};
unsigned long firstClap = 0;
unsigned long lastDetected = 0;
int clapCount = 0;
bool lampOn = false;

void setup() {
  pinMode(lampPin, OUTPUT);
  analogWrite(lampPin, 0);
  Serial.begin(9600);
}

void loop() {
  int value = analogRead(soundPin);
  unsigned long now = millis();

  if (value > 45 && now - lastDetected > 200) {
    lastDetected = now;
    if (clapCount == 0) {
      firstClap = now;
      clapCount = 1;
    } else {
      clapCount = 2;
    }
  }

  if (clapCount > 0 && now - firstClap > 1000) {
    if (clapCount == 1) {
      lampOn = !lampOn;
      analogWrite(lampPin, lampOn ? brightness[level] : 0);
    } 
    else if (clapCount == 2 && lampOn) {
      level++;
      if (level > 5) level = 1;
      analogWrite(lampPin, brightness[level]);
    }
    clapCount = 0;
  }