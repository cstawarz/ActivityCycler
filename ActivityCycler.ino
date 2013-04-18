const byte ACTIVITY_CONTROL_PIN = 12;   // Arbitrary
const byte RUNNING_INDICATOR_PIN = 13;  // Built-in LED

boolean isRunning = false;
boolean isActive = false;
boolean isVerbose = false;

const int MAX_NUM_INTERVALS = 100;
const unsigned long MAX_INTERVAL = 10800;  // 3 hours
const unsigned long SECONDS_TO_MILLIS = 1000;
unsigned long intervals[MAX_NUM_INTERVALS];
int numIntervals = 0;
int currentInterval = 0;
unsigned long currentIntervalStartTime = 0;

const unsigned long SERIAL_SPEED = 9600;
const byte SERIAL_CONFIG = SERIAL_8N1;
String commandBuffer;


void setup() {
    // Configure outputs
    pinMode(ACTIVITY_CONTROL_PIN, OUTPUT);
    pinMode(RUNNING_INDICATOR_PIN, OUTPUT);

    // Set up communciation
    commandBuffer.reserve(200);
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
            setRunning(false);
            if (isVerbose) {
                Serial.println("DONE");
            }
            return;
        }
        state = !state;
    } while (intervals[currentInterval] == 0);

    setActive(state);
    currentIntervalStartTime = currentTime;
}


void setRunning(boolean state) {
    if (isRunning != state) {
        if (!state) {
            setActive(false);
        }
        digitalWrite(RUNNING_INDICATOR_PIN, (state ? HIGH : LOW));
        isRunning = state;
    }
}


void setActive(boolean state) {
    if (isActive != state) {
        digitalWrite(ACTIVITY_CONTROL_PIN, (state ? HIGH : LOW));
        isActive = state;
        if (isVerbose) {
            Serial.println(state ? "ACTIVE" : "INACTIVE");
        }
    }
}


void serialEvent() {
    while (Serial.available()) {
        char nextByte = Serial.read();
        commandBuffer += nextByte;
        if (nextByte == '\n') {
            commandBuffer.trim();
            commandBuffer.toUpperCase();
            handleCommand(commandBuffer);
            commandBuffer = "";
        }
    }
}


void handleCommand(const String &command) {
    if (command.startsWith("RUN ")) {
        handleRunCommand(command.substring(4), false);
    } else if (command.startsWith("DEBUG ")) {
        handleRunCommand(command.substring(6), true);
    } else if (command == "STOP") {
        handleStopCommand();
    } else if (command == "STATUS") {
        handleStatusCommand();
    } else {
        Serial.println("ERROR invalid command");
    }
}


void handleRunCommand(const String &args, boolean verbose) {
    if (isRunning) {
        Serial.println("ERROR already running");
        return;
    }

    isVerbose = verbose;
    
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

    if (numIntervals > 0) {  // Sanity check -- should always be true
        currentInterval = 0;
        currentIntervalStartTime = millis();
        setRunning(true);
    }

    Serial.print("OK RUN");
    for (int i = 0; i < numIntervals; i++) {
        Serial.print(' ');
        Serial.print(intervals[i]);
        if (i < numIntervals - 1)
            Serial.print(',');
    }
    Serial.println("");
    
    if (isVerbose) {
        Serial.println("INACTIVE");
    }
}


void handleStopCommand() {
    setRunning(false);
    Serial.println("OK STOP");
}


void handleStatusCommand() {
    Serial.print("OK STATUS ");
    Serial.println(isRunning ? "RUNNING" : "STOPPED");
}
