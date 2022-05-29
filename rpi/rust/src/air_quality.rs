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
struct AirQualityResponse {
    list: Vec<AirQuality>,
}

#[derive(Deserialize, Debug)]
struct AirQuality {
    #[serde(rename = "dt")]
    utc_timestamp: i64,
    main: AQI,
    components: Pollutants,
}

#[derive(Deserialize, Debug)]
struct AQI {
    aqi: i32,
}

#[derive(Deserialize, Debug)]
struct Pollutants {
    co: Option<f32>,
    no: Option<f32>,
    no2: Option<f32>,
    o3: Option<f32>,
    so2: Option<f32>,
    pm2_5: Option<f32>,
    pm10: Option<f32>,
    nh3: Option<f32>,
}

pub async fn get_air_quality(
    api_key: &str,
    lat: &str,
    lon: &str,
) -> Result<(), Box<dyn std::error::Error>> {
    let url = format!(
        "https://api.openweathermap.org/data/2.5/air_pollution?lat={}&lon={}&appid={}",
        lat, lon, api_key
    );
    // println!("{}", url);
    // let resp = reqwest::blocking::get(url)?.text()?;
    // println!("{}", resp);
    let resp: AirQualityResponse = reqwest::get(url).await?.json()?;
    println!("{:?}", resp);
    let dt = Utc
        .timestamp(resp.list[0].utc_timestamp, 0)
        .with_timezone(&Local);
    println!("{}", dt);

    let url = format!(
        "https://api.openweathermap.org/data/2.5/air_pollution/history?lat={}&lon={}&start={}&end={}&appid={}",
        lat, lon, resp.list[0].utc_timestamp - 60 * 60 * 8, resp.list[0].utc_timestamp, api_key
    );
    println!("{}", url);
    let resp: AirQualityResponse = reqwest::get(url).await?.json();
    println!("{:?}", resp);

    // TODO:
    // 1) Convert this information to something useful to the caller, instead of printing it.
    // 2) Correctly gather useful historical data or cache it.
    // 3) Possibly daemonize this instead of making it a one-shot.

    Ok(())
}
