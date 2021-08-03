// Date and time functions using a PCF8523 RTC connected via I2C and Wire lib
#include "RTClib.h"

// The struct of the clock.
struct RTCClock {
  uint8_t  RTCminute;
  uint8_t  RTChour;
  uint8_t  RTCday;
  uint8_t  RTCmonth;
  uint16_t RTCyear;
};
// initialize one struct
RTCClock rtcclock;

RTC_PCF8523 rtc;

char daysOfTheWeek[7][12] = {"Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday"};


void RTCsetup () {
  if (! rtc.begin()) {
    Serial.println("Couldn't find RTC");
    Serial.flush();
    abort();
  }

//  rtc.adjust(DateTime(2021, 8, 3, 9, 26, 0));

  if (! rtc.initialized() || rtc.lostPower()) {
    Serial.println("RTC is NOT initialized, let's set the time!");
    // When time needs to be set on a new device, or after a power loss, the
    // following line sets the RTC to the date & time this sketch was compiled
    rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
    // This line sets the RTC with an explicit date & time, for example to set
    // January 21, 2014 at 3am you would call:
    // rtc.adjust(DateTime(2014, 1, 21, 3, 0, 0));
    //
    // Note: allow 2 seconds after inserting battery or applying external power
    // without battery before calling adjust(). This gives the PCF8523's
    // crystal oscillator time to stabilize. If you call adjust() very quickly
    // after the RTC is powered, lostPower() may still return true.
  }

  // When time needs to be re-set on a previously configured device, the
  // following line sets the RTC to the date & time this sketch was compiled
  // rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
  // This line sets the RTC with an explicit date & time, for example to set
  // January 21, 2014 at 3am you would call:
  // rtc.adjust(DateTime(2014, 1, 21, 3, 0, 0));

  // When the RTC was stopped and stays connected to the battery, it has
  // to be restarted by clearing the STOP bit. Let's do this to ensure
  // the RTC is running.
  rtc.start();
}

void RTCloop() {
    DateTime now = rtc.now();
    rtcclock.RTCyear = now.year();
    rtcclock.RTCmonth = now.month();
    rtcclock.RTCday = now.day();
    rtcclock.RTChour = now.hour();
    rtcclock.RTCminute = now.minute();
    Serial.println("rtcloop");
}