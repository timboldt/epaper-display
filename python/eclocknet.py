import board
import busio
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
        # TODO: load cache
        # print(len(microcontroller.nvm))
        self._cache = {"last_refresh": 0}
        self._conn = None

    def seconds_since_last_refresh(self, rtc):
        if rtc.lost_power:
            return 1e6

        return time.mktime(rtc.datetime) - self._cache["last_refresh"]

    def refresh(self, rtc):
        self._connect()
        if not self._conn.is_connected:
            return
        self._get_net_time(rtc)
        # get network stuff
        # disconnect
        # update cache
        # also keep track of time for RTC

    def __getitem__(self, key):
        return self._cache[key]

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
            print(rtc.datetime)
            r.close()
        except RuntimeError as e:
            print("WiFi connection failed: ", e)
            # TODO: Close dangling connection better?
            self._conn = None

    # print(
    #     "IP lookup adafruit.com: %s" % esp.pretty_ip(
    #         esp.get_host_by_name("adafruit.com"))
    # )
    # print("Ping google.com: %d ms" % esp.ping("google.com"))

    # print()
    # JSON_URL = "http://api.coindesk.com/v1/bpi/currentprice/USD.json"
    # print("Fetching json from", JSON_URL)
    # r = requests.get(JSON_URL)
    # print("-" * 40)
    # print(r.json())
    # print("-" * 40)
    # r.close()

    # print()
    # JSON_URL = "http://worldtimeapi.org/api/ip"
    # print("Fetching json from", JSON_URL)
    # r = requests.get(JSON_URL)
    # print("-" * 40)
    # print(r.json()["datetime"])
    # print("-" * 40)
    # r.close()
