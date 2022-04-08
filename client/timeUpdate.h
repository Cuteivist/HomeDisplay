bool setupTime() {
    // configTime(gmtOffset_sec, daylightOffset_sec, ntpServer, "time.nist.gov"); //(gmtOffset_sec, daylightOffset_sec, ntpServer)
    // setenv("TZ", Timezone, 1);                                                 // setenv()adds the "TZ" variable to the environment with a value TimeZone, only used if set to 1, 0 means no change
    // tzset();                                                                   // Set the TZ environment variable
    // delay(100);
    // bool TimeStatus = UpdateLocalTime();
    // return TimeStatus;
    return false;
}

bool updateLocalTime() {
    // struct tm timeinfo;
    // char time_output[30], day_output[30], update_time[30];
    // while (!getLocalTime(&timeinfo, 10000))
    // { // Wait for 10-sec for time to synchronise
    //     BoardController::debug("Failed to obtain time");
    //     return false;
    // }
    // CurrentHour = timeinfo.tm_hour;
    // CurrentMin = timeinfo.tm_min;
    // CurrentSec = timeinfo.tm_sec;
    //  //See http://www.cplusplus.com/reference/ctime/strftime/
    // BoardController::debug(&timeinfo, "%a %b %d %Y   %H:%M:%S"); 
    // Displays: Saturday, June 24 2017 14:05:49
    //  if (Units == "M") {
    //    if ((Language == "CZ") || (Language == "DE") || (Language == "PL") || (Language == "NL")){
    //      sprintf(day_output, "%s, %02u. %s %04u", weekday_D[timeinfo.tm_wday], timeinfo.tm_mday, month_M[timeinfo.tm_mon], (timeinfo.tm_year) + 1900); // day_output >> So., 23. Juni 2019 <<
    //    }
    //    else
    //    {
    //      sprintf(day_output, "%s %02u-%s-%04u", weekday_D[timeinfo.tm_wday], timeinfo.tm_mday, month_M[timeinfo.tm_mon], (timeinfo.tm_year) + 1900);
    //    }
    //    strftime(update_time, sizeof(update_time), "%H:%M:%S", &timeinfo);  // Creates: '14:05:49'
    //    sprintf(time_output, "%s %s", TXT_UPDATED, update_time);
    //  }
    //  else
    //  {
    //    strftime(day_output, sizeof(day_output), "%a %b-%d-%Y", &timeinfo); // Creates  'Sat May-31-2019'
    //    strftime(update_time, sizeof(update_time), "%r", &timeinfo);        // Creates: '02:05:49pm'
    //    sprintf(time_output, "%s %s", TXT_UPDATED, update_time);
    //  }
    //  Date_str = day_output;
    //  Time_str = time_output;
    return true;
}
