const int ledPin = 13;

const char* keyChars[] = {"abc", "def", "ghi", "jkl", "mno", "pqrs", "tuv", "wxyz", " .,"};
int lastKeyStates[] = {HIGH, HIGH, HIGH, HIGH, HIGH, HIGH, HIGH, HIGH, HIGH, HIGH};
int oldKeyStates[] = {HIGH, HIGH, HIGH, HIGH, HIGH, HIGH, HIGH, HIGH, HIGH, HIGH};
int newKeyStates[] = {HIGH, HIGH, HIGH, HIGH, HIGH, HIGH, HIGH, HIGH, HIGH, HIGH};
unsigned long lastDebounceTimes[] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0};

// abc corresponds to button 2
const int buttonOffset = 2;
const int numButtons = 10;
const unsigned long timeout = 1000;
const unsigned long debounceDelay = 50;
const bool preview = true;

bool capsOn = false;

int lastPressedKey = -1;
int lastPressedCharInd = -1;
unsigned long lastPressedTime = millis();

char waitingChar = '\0';

void setup() {
  Serial.begin(9600);
  pinMode(ledPin, OUTPUT);
  int i;
  for (i = 0; i < numButtons; i++)
  {
    pinMode(i+buttonOffset, INPUT);
    digitalWrite(i+buttonOffset, HIGH);
  }
}

int getPressedKey()
{
  int toReturn = -1;
  for (int i = 0; i < numButtons; ++i)
  {
    int currReading = digitalRead(i+buttonOffset);
    if (currReading != lastKeyStates[i])
    {
      lastDebounceTimes[i] = millis();
    }
    if (millis() - lastDebounceTimes[i] > debounceDelay)
    {
      if (currReading != newKeyStates[i])
      {
        oldKeyStates[i] = newKeyStates[i];
        newKeyStates[i] = currReading;
      }
    }
    
    if (newKeyStates[i] == LOW && oldKeyStates[i] == HIGH)
    {
      toReturn = i;
      oldKeyStates[i] = LOW;
    }
    lastKeyStates[i] = currReading;
  }
  return toReturn;
}


char getKeyChar(int button, int ind)
{
  if (button == 8 && capsOn)
  {
    return('<');
  }
  const char* possibleChars = keyChars[button];
  int realInd = ind % strlen(possibleChars);
  if (capsOn)
  {
    return toupper(possibleChars[realInd]);
  }
  else
  {
    return possibleChars[realInd];
  }
}

void loop() {
  digitalWrite(ledPin, capsOn);
  int pressedKey = getPressedKey();
  if (millis() > lastPressedTime + timeout)
  {
    lastPressedKey = -1;
    if (!preview && waitingChar != '\0')
    {
      Serial.print(waitingChar);
      waitingChar = '\0';
    }
  }
  if (pressedKey == 9)
  {
    capsOn = !capsOn;
  }
  else if (pressedKey != -1)
  {
    if (pressedKey == lastPressedKey)
    {
      ++lastPressedCharInd;
      if (preview)
      {
        char c = getKeyChar(pressedKey, lastPressedCharInd);
        if (c != '<')
        {
          Serial.print('<');
        }
        Serial.print(getKeyChar(pressedKey, lastPressedCharInd));
      }
      else
      {
        waitingChar = getKeyChar(pressedKey, lastPressedCharInd);
      }
      //show next letter on same key
    }
    else
    {
      lastPressedCharInd = 0;
      if (preview)
      {
        Serial.print(getKeyChar(pressedKey, 0));
      }
      else if (!preview && waitingChar != '\0')
      {
        Serial.print(waitingChar);
        waitingChar = getKeyChar(pressedKey, 0);
      }
    }
    lastPressedTime = millis();
    lastPressedKey = pressedKey;
  }
}
