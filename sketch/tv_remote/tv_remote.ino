const char FIRST_LED_PIN= 3;

const char FIRST_BUTTON = 8;
const int COUNT = 5;
const int WAIT_INPUT = 50;
#define VOLUME_PIN A0

int channel = 0;
int volume = 50;
int command = 0;
int power = 1;

void setup() {
  
  // initialize serial communication:
  Serial.begin(9600);

  for (int i = 0; i < COUNT; ++i)
  {
    pinMode(FIRST_BUTTON + i, INPUT);
    pinMode(FIRST_LED_PIN + i            , OUTPUT);
  }
  pinMode(VOLUME_PIN, INPUT);

}

bool readButton(int button)
{
  int buttonPin = button + FIRST_BUTTON;
  bool shouldSwitch = false;
  return (digitalRead(buttonPin) == HIGH);
}

void sendChannel()
{
    Serial.write('C');
    Serial.write(channel);
}

void sendVolume()
{
    Serial.write('V');
    Serial.write(volume);
}

void powerOff()
{
  if (power == 1)
  {
    digitalWrite(FIRST_LED_PIN + channel, LOW);
  }
  power = 0;
}

void talkToPc()
{
  int val = Serial.read();
  if (val == 'C') {
    sendChannel();
  }
  else if (val == 'V') {
    sendVolume();
  }
  else if (val == 'P') {
    powerOff();
  } else if (val == 'A')
  {
    sendChannel();
    sendVolume();
  }
}

void loop() {
  // Check channel buttons
  for (int i = 0; i < COUNT; ++i)
  {
    if (readButton(i))
    {
      power = 1;
      if (channel != i)
      {
        digitalWrite(FIRST_LED_PIN + channel, LOW);
        channel = i;
      }
      digitalWrite(FIRST_LED_PIN + channel, HIGH);
      sendChannel();
    }
  }

  // Check volume
  int rotation = analogRead(VOLUME_PIN);
  int newVolume = rotation / 10;
  if (newVolume > 100) newVolume = 100;
  if (newVolume != volume)
  {
    volume = newVolume;
    sendVolume();
  }

  // Check serial port command
  if (Serial.available() > 0)
  {
    talkToPc();  
  }

  // Wait for next cycle
  delay(100);
}
