
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