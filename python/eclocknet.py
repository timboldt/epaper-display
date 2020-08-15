import board
import busio
from digitalio import DigitalInOut

from adafruit_esp32spi import adafruit_esp32spi
import adafruit_esp32spi.adafruit_esp32spi_socket as socket
import adafruit_requests as requests

# Get wifi details and more from a secrets.py file
try:
    import secrets
except ImportError:
    print("WiFi secrets are kept in secrets.py, please add them there!")
    raise


def net_test():
    pass
    # esp32_cs = DigitalInOut(board.D13)
    # esp32_ready = DigitalInOut(board.D11)
    # esp32_reset = DigitalInOut(board.D12)

    # esp = adafruit_esp32spi.ESP_SPIcontrol(
    #     board.SPI(), esp32_cs, esp32_ready, esp32_reset)

    # requests.set_socket(socket, esp)

    # if esp.status == adafruit_esp32spi.WL_IDLE_STATUS:
    #     print("ESP32 found and in idle mode")
    # print("Firmware vers.", esp.firmware_version)
    # print("MAC addr:", [hex(i) for i in esp.MAC_address])

    # for ap in esp.scan_networks():
    #     print("\t%s\t\tRSSI: %d" % (str(ap['ssid'], 'utf-8'), ap['rssi']))

    # print("Connecting to AP...")
    # while not esp.is_connected:
    #     try:
    #         esp.connect_AP(secrets.WIFI_SSID, secrets.WIFI_PASSWORD)
    #     except RuntimeError as e:
    #         print("could not connect to AP, retrying: ", e)
    #         continue
    # print("Connected to", str(esp.ssid, "utf-8"), "\tRSSI:", esp.rssi)
    # print("My IP address is", esp.pretty_ip(esp.ip_address))
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

    # print("Done!")
