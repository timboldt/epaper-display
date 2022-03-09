import board
import busio
import json
import microcontroller
import time

from adafruit_esp32spi import adafruit_esp32spi
import adafruit_esp32spi.adafruit_esp32spi_socket as socket
import adafruit_requests as requests

# Get wifi details and more from a secrets.py file
try:
    import secrets
except ImportError:
    print("WiFi secrets are kept in secrets.py, please add them there!")
    raise


class NetCache():
    def __init__(self, cs, ready, reset):
        self._cs = cs
        self._ready = ready
        self._reset = reset
        self._conn = None
        self._load_cache()

    def seconds_since_last_refresh(self, rtc):
        if rtc.lost_power:
            print("RTC lost power")
            return 1e6
        s = time.mktime(rtc.datetime) - self._cache.get("last_refresh", 0)
        print("Cache age:", s)
        return s

    def refresh(self, rtc):
        self._connect()
        if not self._conn.is_connected:
            return
        self._get_net_time(rtc)
        self._get_btc_price()
        self._get_weather()
        self._disconnect()
        self._update_cache()

    def __getitem__(self, key):
        return self._cache.get(key, 0)

    def _load_cache(self):
        try:
            sz = int(microcontroller.nvm[0]) + \
                int(microcontroller.nvm[1]) * 256
            print("Cache size:", sz)
            s = str(microcontroller.nvm[2:sz+2], "utf-8")
            print("Cache contents:", s)
            self._cache = json.loads(s)
        except RuntimeError as e:
            print("Cache load failed: ", e)
            self._cache = {"last_refresh": 0}

    def _update_cache(self):
        b = bytearray(json.dumps(self._cache))
        sz = len(b)
        microcontroller.nvm[0:sz+2] = bytearray([sz & 0xff, sz >> 8]) + b

    def _connect(self):
        try:
            self._conn = adafruit_esp32spi.ESP_SPIcontrol(
                board.SPI(), self._cs, self._ready, self._reset)
            requests.set_socket(socket, self._conn)
            print("Airlift Firmware:", str(
                self._conn.firmware_version, "utf-8")[:-1])
            print("MAC Address:", "-".join([hex(i)[-2:]
                                            for i in self._conn.MAC_address]))

            print("Connecting to WiFi...")
            if not self._conn.is_connected:
                self._conn.connect_AP(secrets.WIFI_SSID, secrets.WIFI_PASSWORD)
            print("Connected to", str(self._conn.ssid,
                                      "utf-8"), "RSSI:", self._conn.rssi)
        except RuntimeError as e:
            print("WiFi connection failed: ", e)
            # TODO: Close dangling connection better?
            self._conn = None

    def _disconnect(self):
        # TODO
        pass

    def _get_net_time(self, rtc):
        try:
            print("Fetching current time from network...")
            r = requests.get("http://worldtimeapi.org/api/ip")
            trtc = time.mktime(rtc.datetime)
            j = r.json()
            tnet = j["unixtime"] + j["raw_offset"] + j["dst_offset"]
            rtc.datetime = time.localtime(tnet)
            print("Adjusted DS3231 clock by", trtc - tnet, "seconds.")
            self._cache["last_refresh"] = tnet
            r.close()
        except RuntimeError as e:
            print("HTTP request failed: ", e)

    def _get_btc_price(self):
        try:
            print("Fetching Bitcoin price from network...")
            r = requests.get(
                "http://api.coindesk.com/v1/bpi/currentprice/USD.json")
            j = r.json()
            btc = float(j["bpi"]["USD"]["rate_float"])
            print("Bitcoin price:", btc)
            self._cache["btc_usd"] = btc
            r.close()
        except RuntimeError as e:
            print("HTTP request failed: ", e)

    def _get_weather(self):
        try:
            print("Fetching weather data from network...")
            r = requests.get(
                "http://api.openweathermap.org/data/2.5/weather?id=5383777&units=metric&appid=" + secrets.OPEN_WEATHER_KEY)
            j = r.json()
            temperature = float(j["main"]["temp"])
            pressure = float(j["main"]["pressure"])
            humidity = float(j["main"]["humidity"])
            print("Weather:", temperature, pressure, humidity)
            self._cache["temperature"] = temperature
            self._cache["pressure"] = pressure
            self._cache["humidity"] = humidity
            r.close()
        except RuntimeError as e:
            print("HTTP request failed: ", e)
