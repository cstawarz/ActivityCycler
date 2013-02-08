const byte ACTIVITY_CONTROL_PIN = 13;
const unsigned long SERIAL_SPEED = 9600;
const byte SERIAL_CONFIG = SERIAL_8N1;

boolean isRunning = false;
boolean isActive = false;

const int MAX_NUM_INTERVALS = 100;
const unsigned long MAX_INTERVAL = 10800;  // 3 hours
const unsigned long SECONDS_TO_MILLIS = 1000;
unsigned long intervals[MAX_NUM_INTERVALS];
int numIntervals = 0;
int currentInterval = 0;
unsigned long currentIntervalStartTime = 0;

String commandBuffer;


void setup() {
    pinMode(ACTIVITY_CONTROL_PIN, OUTPUT);
    Serial.begin(SERIAL_SPEED, SERIAL_CONFIG);
}


void loop() {
    if (!isRunning)
        return;
    
    const unsigned long currentTime = millis();
    
    if ((currentTime - currentIntervalStartTime) <
        (intervals[currentInterval] * SECONDS_TO_MILLIS)) {
        return;
    }

    boolean state = isActive;

    do {
        currentInterval++;
        if (currentInterval == numIntervals) {
            setActive(false);
            isRunning = false;
            Serial.println("DONE");
            return;
        }
        state = !state;
    } while (intervals[currentInterval] == 0);

    setActive(state);
    currentIntervalStartTime = currentTime;
}


void setActive(boolean state) {
    if (isActive != state) {
        digitalWrite(ACTIVITY_CONTROL_PIN, (state ? HIGH : LOW));
        isActive = state;
    }
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
    if (isRunning) {
        Serial.println("ERROR already running");
        return;
    }
    
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

    if  (numIntervals > 0) {  // Sanity check -- should always be true
        currentInterval = 0;
        currentIntervalStartTime = millis();
        isRunning = true;
    }

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
    if (isRunning) {
        setActive(false);
        isRunning = false;
    }
    Serial.println("OK STOP");
}
