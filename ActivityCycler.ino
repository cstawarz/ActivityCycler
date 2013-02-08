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


void setState(byte newState) {
    digitalWrite(ACTIVITY_CONTROL_PIN, newState);
    state = newState;
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
    if (command.startsWith("RUN ")) {
        processRunCommand(command.substring(4));
    } else if (command == "STOP") {
        processStopCommand();
    } else {
        Serial.println("ERROR invalid command");
    }
}


void processRunCommand(const String &args) {
    int fromIndex = 0;
    numIntervals = 0;

    while (fromIndex < args.length() &&
           numIntervals < MAX_NUM_INTERVALS) {
        int toIndex = args.indexOf(',', fromIndex);
        if (toIndex == -1)
            toIndex = args.length();

        int value = args.substring(fromIndex, toIndex).toInt();
        intervals[numIntervals] = constrain(value, 0, MAX_INTERVAL);

        fromIndex = toIndex + 1;
        numIntervals++;
    }

    currentInterval = 0;
    running = true;

    Serial.print("OK RUN");
    for (int i = 0; i < numIntervals; i++) {
        Serial.print(' ');
        Serial.print(intervals[i]);
        if (i < numIntervals - 1)
            Serial.print(',');
    }
    Serial.println("");
}


void processStopCommand() {
    setState(INACTIVE);
    running = false;
    Serial.println("OK STOP");
}
