class Led {
public:
    static const byte STATE_OFF = 0;
    static const byte STATE_ON = 1;
    static const byte STATE_BLINK = 2;

private:
    byte pin;
    byte state = STATE_OFF;
    bool isOn = false;
    unsigned int blinkMillis = 0;
    unsigned long previousMillis = 0;

public:
    Led(byte pin) : pin(pin) {
        pinMode(pin, OUTPUT);
        turnOff();
    }

    void setState(byte newState) {
        state = newState;
        if (state == STATE_OFF) turnOff();
        else if (state == STATE_ON) turnOn();
        else if (state == STATE_BLINK) {
            if (blinkMillis == 0) setState(STATE_OFF);
            turnOff();
            previousMillis = millis();
        }
    }

    void setBlinksPerSecond(byte blinksPerSecond) {
        blinkMillis = 1000 / blinksPerSecond;
    }

    void touch() {
        unsigned long currentMillis = millis();
        if (state == STATE_BLINK && currentMillis - previousMillis >= blinkMillis) {
            previousMillis = currentMillis;
            isOn ? turnOff() : turnOn();
        }
    }

private:
    void turnOn() {
        digitalWrite(pin, HIGH);
        isOn = true;
    }

    void turnOff() {
        digitalWrite(pin, LOW);
        isOn = false;
    }
};

class ultrasonicsens {
private:
    int echoPin, triggerPin;

public:
    ultrasonicsens(int echoPin, int triggerPin) : echoPin(echoPin), triggerPin(triggerPin) {}

    long readUltrasonicDistance() {
        pinMode(triggerPin, OUTPUT);
        digitalWrite(triggerPin, LOW);
        delayMicroseconds(2);
        digitalWrite(triggerPin, HIGH);
        delayMicroseconds(10);
        digitalWrite(triggerPin, LOW);
        pinMode(echoPin, INPUT);
        return pulseIn(echoPin, HIGH);
    }

    long getDistance() {
        return 0.01723 * readUltrasonicDistance();  // Conversión a cm
    }
};


class traficlight {
private:
    ultrasonicsens* distancesensor;
    Led RedLed, YellowLed, BlueLed;

public:
    traficlight(int echo, int trig, int red, int yellow, int Blue)
        : RedLed(red), YellowLed(yellow), BlueLed(green) {
        distancesensor = new ultrasonicsens(echo, trig);
    }

    ~traficlight() {
        delete distancesensor;
    }

    void begin() {
      RedLed.setState(Led::STATE_OFF);
      YellowLed.setState(Led::STATE_OFF);
      BlueLed.setState(Led::STATE_OFF);
    }

    void updateLights() {
        long distance = distancesensor->getDistance();
        Serial.print("Distance: ");
        Serial.println(distance);

        if (distance < 50) {
            RedLed.setState(Led::STATE_ON);
            YellowLed.setState(Led::STATE_OFF);
            BlueLed.setState(Led::STATE_OFF);
        } else if (distance < 100) {
            RedLed.setState(Led::STATE_OFF);
            YellowLed.setState(Led::STATE_ON);
            BlueLed.setState(Led::STATE_OFF);
        } else {
            RedLed.setState(Led::STATE_OFF);
            YellowLed.setState(Led::STATE_OFF);
            BlueLed.setState(Led::STATE_ON);
        }

        RedLed.touch();
        YellowLed.touch();
        GreenLed.touch();
    }
};

traficlight semaphore(22, 23, 18, 19, 21);

void setup() {
    Serial.begin(9600);
    semaphore.begin();
}

void loop() {
    semaphore.updateLights();
    delay(100);
}
