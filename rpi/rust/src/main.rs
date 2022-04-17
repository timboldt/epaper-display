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

use air_quality::get_air_quality;

use crate::weather::get_current_weather;

mod air_quality;
mod weather;

fn main() -> Result<(), Box<dyn std::error::Error>> {
    let lat = match std::env::var("OPEN_WEATHER_LAT") {
        Ok(key) => key,
        Err(_r) => {
            eprintln!("error: missing environment variable OPEN_WEATHER_LAT");
            "".to_string()
        }
    };
    let lon = match std::env::var("OPEN_WEATHER_LON") {
        Ok(key) => key,
        Err(_r) => {
            eprintln!("error: missing environment variable OPEN_WEATHER_LON");
            "".to_string()
        }
    };
    let units = match std::env::var("OPEN_WEATHER_UNITS") {
        Ok(key) => key,
        Err(_r) => "imperial".to_string(),
    };
    let api_key = match std::env::var("OPEN_WEATHER_KEY") {
        Ok(key) => key,
        Err(_r) => {
            eprintln!("error: missing environment variable OPEN_WEATHER_KEY");
            "".to_string()
        }
    };
    let _ = get_current_weather(&api_key, &lat, &lon, &units)?;
    let _ = get_air_quality(&api_key, &lat, &lon);
    Ok(())
}
