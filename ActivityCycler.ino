const byte ACTIVITY_CONTROL_PIN = 13;
const byte ACTIVE = HIGH;
const byte INACTIVE = LOW;

const unsigned long SERIAL_SPEED = 9600;
const byte SERIAL_CONFIG = SERIAL_8N1;

const unsigned long UPDATE_INTERVAL = 100;  // 100 ms

String commandBuffer;

boolean running = false;
byte state = INACTIVE;

const int MAX_NUM_INTERVALS = 100;
const unsigned long MAX_INTERVAL = 10800;  // 3 hours
unsigned long intervals[MAX_NUM_INTERVALS];
int numIntervals = 0;
int currentInterval = 0;


void setup() {
    Serial.begin(SERIAL_SPEED, SERIAL_CONFIG);
    pinMode(ACTIVITY_CONTROL_PIN, OUTPUT);
}


void loop() {
    if (running) {
        digitalWrite(ACTIVITY_CONTROL_PIN, ACTIVE); 
    } else {
        digitalWrite(ACTIVITY_CONTROL_PIN, INACTIVE);
    }

    delay(UPDATE_INTERVAL);
}


void serialEvent() {
    while (Serial.available()) {
        char nextByte = Serial.read();
        commandBuffer += nextByte;
        if (nextByte == '\n') {
            commandBuffer.trim();
            commandBuffer.toUpperCase();
            processCommand(commandBuffer);
            commandBuffer = "";
        }
    }
}


void processCommand(const String &command) {
    boolean ok = false;
    
    if (command.startsWith("RUN ")) {
        ok = processRunCommand(command);
    } else if (command == "STOP") {
        running = false;
        ok = true;
    } else {
        Serial.println("Invalid command");
    }

    if (ok) {
        Serial.println("OK");
    }
}


boolean processRunCommand(const String &command) {
    numIntervals = 0;
    unsigned int fromIndex = 4;

    while (fromIndex < command.length()) {
        if (numIntervals >= MAX_NUM_INTERVALS) {
            Serial.println("Too many intervals");
            return false;
        }

        int toIndex = command.indexOf(',', fromIndex);
        if (toIndex == -1)
            toIndex = command.length();

        int value = command.substring(fromIndex, toIndex).toInt();
        intervals[numIntervals] = constrain(value, 0, MAX_INTERVAL);

        numIntervals++;
        fromIndex = toIndex + 1;
    }

    for (int i = 0; i < numIntervals; i++) {
        Serial.print(intervals[i]);
        Serial.print(' ');
    }
    Serial.println("");

    currentInterval = 0;
    running = true;
    
    return true;
}
