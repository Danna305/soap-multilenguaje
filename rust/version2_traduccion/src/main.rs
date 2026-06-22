// http://localhost:8091/?n=10
// cargo run

use warp::Filter;

#[tokio::main]
async fn main() {
    let route = warp::get()
        .and(warp::query::<std::collections::HashMap<String, String>>())
        .and_then(|params: std::collections::HashMap<String, String>| async move {
            let n = params.get("n").cloned().unwrap_or("0".to_string());

            let soap = format!(r#"<?xml version="1.0" encoding="utf-8"?>
<soap:Envelope xmlns:soap="http://schemas.xmlsoap.org/soap/envelope/">
  <soap:Body>
    <NumberToWords xmlns="http://www.dataaccess.com/webservicesserver/">
      <ubiNum>{}</ubiNum>
    </NumberToWords>
  </soap:Body>
</soap:Envelope>"#, n);

            let client = reqwest::Client::new();
            let soap_resp = client
                .post("https://www.dataaccess.com/webservicesserver/NumberConversion.wso")
                .header("Content-Type", "text/xml; charset=utf-8")
                .body(soap)
                .send()
                .await
                .unwrap()
                .text()
                .await
                .unwrap();

            let en_ingles = soap_resp
                .split("<NumberToWordsResult>")
                .nth(1).unwrap_or("")
                .split("</NumberToWordsResult>")
                .next().unwrap_or("")
                .trim()
                .to_string();

            let url = format!(
                "https://api.mymemory.translated.net/get?q={}&langpair=en|es",
                urlencoding::encode(&en_ingles)
            );

            let trad_resp = client.get(&url).send().await.unwrap().text().await.unwrap();
            let json: serde_json::Value = serde_json::from_str(&trad_resp).unwrap();
            let traducido = json["responseData"]["translatedText"]
                .as_str()
                .unwrap_or("")
                .to_string();

            Ok::<_, warp::Rejection>(warp::reply::with_header(
                traducido,
                "Content-Type",
                "text/plain; charset=utf-8",
            ))
        });

    println!("Servidor en http://localhost:8091/?n=10");
    warp::serve(route).run(([127, 0, 0, 1], 8091)).await;
}