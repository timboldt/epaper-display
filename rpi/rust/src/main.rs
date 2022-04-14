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

extern crate reqwest;
extern crate serde;

use serde::Deserialize;

#[derive(Deserialize, Debug)]
struct OneCallWeather {
    current: CurrentConditions,
}

#[derive(Deserialize, Debug)]
struct CurrentConditions {
    // Temperature in degrees (C, F or K).
    #[serde(rename = "temp")]
    temperature: Option<f32>,
    // Percent humidity.
    humidity: Option<i32>,
    // Pressure in hPa.
    pressure: Option<i32>,
}

fn main() -> Result<(), Box<dyn std::error::Error>> {
    let lat = 37.6624;
    let lon = -121.8747;
    let units = "imperial";
    let api_key = match std::env::var("OPEN_WEATHER_KEY") {
        Ok(key) => key,
        Err(_r) => {
            eprintln!("error: missing environment variable OPEN_WEATHER_KEY");
            "".to_string()
        }
    };
    let url = format!(
        "https://api.openweathermap.org/data/2.5/onecall?lat={}&lon={}&units={}&exclude=hourly,daily&appid={}",
        lat, lon, units, api_key
    );
    println!("{}", url);

    let resp: OneCallWeather = reqwest::blocking::get(url)?.json()?;
    println!("{:?}", resp);
    //let resp = reqwest::blocking::get(url)?.text()?;
    //println!("{}", resp);
    Ok(())
}
