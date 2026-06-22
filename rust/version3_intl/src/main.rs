// http://localhost:8092/?n=10
// cargo run

use warp::Filter;

#[tokio::main]
async fn main() {
    let route = warp::get()
        .and(warp::query::<std::collections::HashMap<String, String>>())
        .map(|params: std::collections::HashMap<String, String>| {
            let n: i64 = params.get("n")
                .and_then(|v| v.parse().ok())
                .unwrap_or(0);

            let resultado = convertir_espanol(n);
            warp::reply::with_header(resultado, "Content-Type", "text/plain; charset=utf-8")
        });

    println!("Servidor en http://localhost:8092/?n=10");
    warp::serve(route).run(([127, 0, 0, 1], 8092)).await;
}

fn convertir_espanol(n: i64) -> String {
    if n == 0 { return "cero".to_string(); }
    if n < 0 { return format!("menos {}", convertir_espanol(-n)); }

    let unidades = ["","uno","dos","tres","cuatro","cinco","seis","siete",
        "ocho","nueve","diez","once","doce","trece","catorce","quince",
        "dieciséis","diecisiete","dieciocho","diecinueve","veinte"];
    let decenas = ["","","veinte","treinta","cuarenta","cincuenta",
        "sesenta","setenta","ochenta","noventa"];
    let centenas = ["","ciento","doscientos","trescientos","cuatrocientos",
        "quinientos","seiscientos","setecientos","ochocientos","novecientos"];

    match n {
        1..=20 => unidades[n as usize].to_string(),
        21..=29 => format!("veinti{}", unidades[(n-20) as usize]),
        30..=99 => {
            let res = decenas[(n/10) as usize].to_string();
            if n % 10 != 0 { format!("{} y {}", res, unidades[(n%10) as usize]) }
            else { res }
        },
        100 => "cien".to_string(),
        101..=999 => {
            let res = centenas[(n/100) as usize].to_string();
            if n % 100 != 0 { format!("{} {}", res, convertir_espanol(n%100)) }
            else { res }
        },
        1000..=1999 => {
            let res = "mil".to_string();
            if n % 1000 != 0 { format!("{} {}", res, convertir_espanol(n%1000)) }
            else { res }
        },
        2000..=999999 => {
            let res = format!("{} mil", convertir_espanol(n/1000));
            if n % 1000 != 0 { format!("{} {}", res, convertir_espanol(n%1000)) }
            else { res }
        },
        _ => n.to_string()
    }
}