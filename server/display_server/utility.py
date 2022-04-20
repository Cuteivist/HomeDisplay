from types import NoneType
import math
from decimal import Decimal

def normalize_datetime_array(array):
    array_len = len(array)
    if array_len == 0:
        return list()
    elif array_len == 1:
        return [0]
    # Start from 0 and go to 1
    result = [0]
    last_element = array[-1]
    first_element = array[0]
    diff = (array[-1] - array[0]).total_seconds()
    for d in array[1:]:
        result.append(f"{((d - first_element).total_seconds() / diff):.3f}")
    return result

def remove_duplicates(array1, array2):
    length = len(array1) - 1
    if length < 1 or len(array2) != len(array1):
        return

    # If sensor was unavaialble there will be null values
    for i in reversed(range(length)):
        val = array1[i]
        if type(val) is NoneType or val == "unknown" or val == "unavailable":
            del array1[i]
            del array2[i]

    length = len(array1) - 1
    # Only two duplicates are neede for drawing
    # In case values are [2, 2, 2, 3, 4]
    # Middle 2 isn't needed, but those 2 are 
    # required to draw flat line
    same_value_count = 1
    for i in reversed(range(length)):                
        if array1[i] == array1[i + 1]:
            if same_value_count > 1:
                del array1[i + 1]
                del array2[i + 1]
            same_value_count += 1
        else:
            same_value_count = 1
            last_value = array1[i]

    
def nice_number(value, round_=False):
    exponent = math.floor(math.log(value, 10))
    fraction = value / 10 ** exponent

    if round_:
        if fraction < 1.5:
            nice_fraction = 1.
        elif fraction < 3.:
            nice_fraction = 2.
        elif fraction < 7.:
            nice_fraction = 5.
        else:
            nice_fraction = 10.
    else:
        if fraction <= 1:
            nice_fraction = 1.
        elif fraction <= 2:
            nice_fraction = 2.
        elif fraction <= 5:
            nice_fraction = 5.
        else:
            nice_fraction = 10.

    return nice_fraction * 10 ** exponent

def nice_bounds(axis_start, axis_end, num_ticks=10):
    axis_width = axis_end - axis_start
    if axis_width == 0:
        nice_tick = 0
    else:
        nice_range = nice_number(axis_width)
        nice_tick = nice_number(nice_range / (num_ticks - 1), round_=True)
        axis_start = math.floor(axis_start / nice_tick) * nice_tick
        axis_end = math.ceil(axis_end / nice_tick) * nice_tick

    return axis_start, axis_end, nice_tick

def remove_exponent(num):
    decimal_num = Decimal(num)
    return decimal_num.quantize(Decimal(1)) if decimal_num == decimal_num.to_integral() else decimal_num.normalize()


def weekday_to_str(weekday):
    if weekday == 0:
        return "Pon"
    if weekday == 1:
        return "Wto"
    if weekday == 2:
        return "Sro"
    if weekday == 3:
        return "Czw"
    if weekday == 4:
        return "Pia"
    if weekday == 5:
        return "Sob"
    if weekday == 6:
        return "Nie"
    return ""