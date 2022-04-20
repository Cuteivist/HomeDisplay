import utility
import datetime

class DataHandler:
    def __init__(self, plot_time):
        self.plot_data_time = plot_time

    def plot_location(self):
        raise NotImplementedError

    def plot_type(self):
        raise NotImplementedError

    def _update_min_y(self, min_y, max_y):
        return min_y

    def _calculate_plot_labels(self, plot_date):
        raise NotImplementedError
        
    def create_plot_obj(self, title, x_series, y_series):
        plot_obj = {
            "title": title,
            "location": self.plot_location(),
            "type": self.plot_type(),
            "xMin": 0,
            "xMax": 1,
            "x": x_series,
            "y": y_series
        }
        min_y = 9999.0
        max_y = -9999.0
        # Find min and max of all series
        for series in y_series:
            if type(series) == list:
                for val in series:
                    casted_val = float(val)
                    min_y = min(min_y, casted_val)
                    max_y = max(max_y, casted_val)
            else:
                casted_val = float(series)
                min_y = min(min_y, casted_val)
                max_y = max(max_y, casted_val)

        min_y = self._update_min_y(min_y, max_y)

        (nice_min_y, nice_max_y, nice_tick_y) = utility.nice_bounds(min_y, max_y, 5)

        plot_obj["yMin"] = nice_min_y
        plot_obj["yMax"] = nice_max_y
        # Add 1 to include starting point
        y_label_count = int((nice_max_y - nice_min_y) / nice_tick_y) + 1
        plot_obj["yLabels"] = [str(utility.remove_exponent(p * nice_tick_y + nice_min_y)) for p in range(0, y_label_count)]
        plot_obj["xLabels"], plot_obj["xLabelsPos"] = self._calculate_plot_labels(self.plot_data_time)
        return plot_obj
