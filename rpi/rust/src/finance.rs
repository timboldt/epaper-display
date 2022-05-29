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

pub fn get_stock_intraday(
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
    let resp: AirQualityResponse = reqwest::blocking::get(url)?.json()?;
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
    let resp: AirQualityResponse = reqwest::blocking::get(url)?.json()?;
    println!("{:?}", resp);

    // TODO:
    // 1) Convert this information to something useful to the caller, instead of printing it.
    // 2) Correctly gather useful historical data or cache it.
    // 3) Possibly daemonize this instead of making it a one-shot.

    Ok(())
}




// def get_stock_intraday(sym):
// try:
//     print("Fetching {} stock price from Finhub...".format(sym))
//     r = requests.get(
//         "https://finnhub.io/api/v1/quote?symbol=" +
//         sym +
//         "&token=" +
//         secrets.FINHUB_API_KEY)
//     j = r.json()
//     if sym + "_intraday" in storage.cache:
//         vals = storage.cache[sym + "_intraday"]
//         vals = vals[-200:]
//     else:
//         vals = []
//     newval = float(j["c"])
//     if len(vals) < 1 or newval != vals[-1]:
//         vals.append(newval)
//     storage.cache[sym + "_intraday"] = vals
//     storage.cache[sym + "_previous"] = float(j["pc"])
//     r.close()
// except RuntimeError as e:
//     print("HTTP request failed: ", e)

//https://crates.io/crates/finnhub-rs

// use finnhub_rs::client::Client;
// fn main() {
//     // Create a new finnhub client.
//     let client = Client::new("MY FINNHUB API KEY".to_string());
//     // Get a list of supported stocks given the exchange.
//     let res = client.stock_symbol("US".to_string()).await.unwrap();
//     // Print out the results.
//     println!("{:#?}", res);
// }