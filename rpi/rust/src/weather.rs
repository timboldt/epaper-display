//  Copyright 2022 Google LLC
//
//  Licensed under the Apache License, Version 2.0 (the "License");
//  you may not use this file except in compliance with the License.
//  You may obtain a copy of the License at
//
//      https://www.apache.org/licenses/LICENSE-2.0
//
//  Unless required by applicable law or agreed to in writing, software
//  distributed under the License is distributed on an "AS IS" BASIS,
//  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
//  See the License for the specific language governing permissions and
//  limitations under the License.

#![warn(clippy::all)]

use chrono::{offset::TimeZone, Local, Utc};
use serde::Deserialize;

#[derive(Deserialize, Debug)]
struct OneCallWeather {
    current: CurrentConditions,
}

#[derive(Deserialize, Debug)]
struct CurrentConditions {
    #[serde(rename = "dt")]
    utc_timestamp: i64,
    sunrise: i64,
    sunset: i64,
    // Temperature in degrees (C, F or K).
    #[serde(rename = "temp")]
    temperature: f32,
    // Percent humidity.
    humidity: i32,
    // Pressure in hPa.
    pressure: i32,
    // Wind speed (kmph or mph).
    wind_speed: f32,
    wind_deg: i16,
    weather: Vec<WeatherInfo>,
}

#[derive(Deserialize, Debug)]
struct WeatherInfo {
    id: i16,
    main: String,
    description: String,
    icon: String,
}

pub fn get_current_weather(
    api_key: &str,
    lat: &str,
    lon: &str,
    units: &str,
) -> Result<(), Box<dyn std::error::Error>> {
    let url = format!(
        "https://api.openweathermap.org/data/2.5/onecall?lat={}&lon={}&units={}&exclude=minutely,daily&appid={}",
        lat, lon, units, api_key
    );
    println!("{}", url);
    let resp: OneCallWeather = reqwest::blocking::get(url)?.json()?;
    println!("{:?}", resp);
    let dt = Utc
        .timestamp(resp.current.utc_timestamp, 0)
        .with_timezone(&Local);
    println!("{}", dt);

    let url = format!(
        "https://api.openweathermap.org/data/2.5/onecall/timemachine?dt={}&lat={}&lon={}&units={}&exclude=minutely,daily&appid={}",
        resp.current.utc_timestamp - 60 * 60 * 8, lat, lon, units, api_key
    );
    println!("{}", url);
    let resp: OneCallWeather = reqwest::blocking::get(url)?.json()?;
    println!("{:?}", resp);
    // let resp = reqwest::blocking::get(url)?.text()?;
    // println!("{}", resp);

    // TODO:
    // 1) Convert this information to something useful to the caller, instead of printing it.
    // 2) Correctly gather useful historical data or cache it.
    // 3) Possibly daemonize this instead of making it a one-shot.

    Ok(())
}
