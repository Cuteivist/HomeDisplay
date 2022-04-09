#ifndef LOGGER_H
#define LOGGER_H

extern int mStartTime;
// TODO debug should only happend in debug
#define DEBUG(str) Serial.println(String(" | ") + String(millis() - mStartTime) + " - " + String(str))

#endif // LOGGER_H