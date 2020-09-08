import json
import requests
import time

import storage

try:
    import secrets
except ImportError:
    print("Network secrets are kept in secrets.py, please add them there!")
    raise


def time_since_last_fetch():
    if "last_fetch" in storage.cache:
        return time.time() - float(storage.cache["last_fetch"])
    else:
        return time.time()


def set_last_fetch_time(t):
    storage.cache["last_fetch"] = time.time()


def get_btc_price():
    try:
        print("Fetching Bitcoin price from network...")
        r = requests.get(
            "http://api.coindesk.com/v1/bpi/currentprice/USD.json")
        j = r.json()
        btc = float(j["bpi"]["USD"]["rate_float"])
        print("Bitcoin price:", btc)
        storage.cache["btc_usd"] = btc
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
        storage.cache["temperature"] = temperature
        storage.cache["pressure"] = pressure
        storage.cache["humidity"] = humidity
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
        storage.cache["ozone"] = o3
        storage.cache["pm25"] = pm25
        r.close()
    except RuntimeError as e:
        print("HTTP request failed: ", e)


# def get_stock_intraday(sym):
#     try:
#         print("Fetching {} stock price from AlphaVantage...".format(sym))
#         r = requests.get(
#             "https://www.alphavantage.co/query?function=TIME_SERIES_INTRADAY&symbol=" +
#             sym +
#             "&interval=15min&apikey=" +
#             secrets.ALPHAVANTAGE_API_KEY)
#         j = r.json()
#         series = j["Time Series (15min)"]
#         vals = []
#         for t in series:
#             vals.append(float(j["Time Series (15min)"][t]["4. close"]))
#         vals.reverse()
#         storage.cache[sym + "_intraday"] = vals
#         r.close()
#     except RuntimeError as e:
#         print("HTTP request failed: ", e)

def get_stock_intraday(sym):
    try:
        print("Fetching {} stock price from Finhub...".format(sym))
        r = requests.get(
            "https://finnhub.io/api/v1/quote?symbol=" +
            sym +
            "&token=" +
            secrets.FINHUB_API_KEY)
        j = r.json()
        if sym + "_intraday" in storage.cache:
            vals = storage.cache[sym + "_intraday"]
            vals = vals[-100:]
        else:
            vals = []
        vals.append(float(j["c"]))
        storage.cache[sym + "_intraday"] = vals
        storage.cache[sym + "_previous"] = float(j["pc"])
        r.close()
    except RuntimeError as e:
        print("HTTP request failed: ", e)

