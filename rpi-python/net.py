import json
import requests
import time

try:
    import secrets
except ImportError:
    print("Network secrets are kept in secrets.py, please add them there!")
    raise

cache = {}


# def get_net_time():
#     try:
#         print("Fetching current time from network...")
#         r = requests.get("http://worldtimeapi.org/api/ip")
#         j = r.json()
#         tnet = j["unixtime"] + j["raw_offset"] + j["dst_offset"]
#         print("Current time:", tnet)
#         cache["last_refresh"] = tnet
#         r.close()
#     except RuntimeError as e:
#         print("HTTP request failed: ", e)


def get_btc_price():
    try:
        print("Fetching Bitcoin price from network...")
        r = requests.get(
            "http://api.coindesk.com/v1/bpi/currentprice/USD.json")
        j = r.json()
        btc = float(j["bpi"]["USD"]["rate_float"])
        print("Bitcoin price:", btc)
        cache["btc_usd"] = btc
        r.close()
    except RuntimeError as e:
        print("HTTP request failed: ", e)


def get_weather():
    try:
        print("Fetching weather data from network...")
        r = requests.get(
            "http://api.openweathermap.org/data/2.5/weather?id=5383777&units=metric&appid=" + secrets.OPEN_WEATHER_KEY)
        j = r.json()
        temperature = float(j["main"]["temp"])
        pressure = float(j["main"]["pressure"])
        humidity = float(j["main"]["humidity"])
        print("Weather:", temperature, pressure, humidity)
        cache["temperature"] = temperature
        cache["pressure"] = pressure
        cache["humidity"] = humidity
        r.close()
    except RuntimeError as e:
        print("HTTP request failed: ", e)


def get_air_quality():
    try:
        print("Fetching air quality from AirNow...")
        r = requests.get(
            "http://www.airnowapi.org/aq/observation/zipCode/current/?format=application/json&zipCode=94566&distance=25&API_KEY=" +
            secrets.AIRNOW_API_KEY)
        j = r.json()
        # TODO: Verify that doc order is okay. If not look at "parameter".
        o3 = float(j[0]["AQI"])
        pm25 = float(j[1]["AQI"])
        print("AQI:", o3, pm25)
        cache["ozone"] = o3
        cache["pm25"] = pm25
        r.close()
    except RuntimeError as e:
        print("HTTP request failed: ", e)


