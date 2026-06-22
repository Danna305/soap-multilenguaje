// http://localhost:8090/?n=10
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
            let resp = client
                .post("https://www.dataaccess.com/webservicesserver/NumberConversion.wso")
                .header("Content-Type", "text/xml; charset=utf-8")
                .body(soap)
                .send()
                .await
                .unwrap()
                .text()
                .await
                .unwrap();

            let result = resp
            .split("NumberToWordsResult>")
            .nth(1).unwrap_or("")
            .split("<")
            .next().unwrap_or("")
            .to_string();

            Ok::<_, warp::Rejection>(warp::reply::with_header(
                result,
                "Content-Type",
                "text/plain; charset=utf-8",
            ))
        });

    println!("Servidor en http://localhost:8090/?n=10");
    warp::serve(route).run(([127, 0, 0, 1], 8090)).await;
}