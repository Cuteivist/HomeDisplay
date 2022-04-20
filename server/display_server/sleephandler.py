import datetime

class SleepHandler():
    def __init__(self):
        pass

    def add_sleep_info(self, data_json):
        current_hour = datetime.datetime.now().hour
        sleep_timeout_minutes = 30
        if current_hour < 5 or current_hour > 23: # Between 23 and 5
            sleep_timeout_minutes = 60
        elif current_hour > 16  and current_hour < 20: # Between 16 and 20
            sleep_timeout_minutes = 20
        # Timeout is stored in seconds
        data_json["sleep_timeout"] = sleep_timeout_minutes * 60