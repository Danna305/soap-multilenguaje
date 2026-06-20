// http://localhost:5003/?n=10
// cd version3_intl && dotnet run

var builder = WebApplication.CreateBuilder(args);
builder.WebHost.UseUrls("http://localhost:5003");
var app = builder.Build();

app.MapGet("/", (HttpContext ctx) =>
{
    var nStr = ctx.Request.Query["n"].ToString();
    if (string.IsNullOrEmpty(nStr))
    {
        ctx.Response.ContentType = "text/plain";
        return "Proporciona un número: http://localhost:5003/?n=10";
    }

    if (!long.TryParse(nStr, out long n))
        return "Número inválido";

    ctx.Response.ContentType = "text/plain; charset=utf-8";
    return ConvertirEspanol(n);
});

app.Run();

static string ConvertirEspanol(long n)
{
    if (n == 0) return "cero";
    if (n < 0) return "menos " + ConvertirEspanol(-n);

    string[] unidades = ["", "uno", "dos", "tres", "cuatro", "cinco",
        "seis", "siete", "ocho", "nueve", "diez", "once", "doce",
        "trece", "catorce", "quince", "dieciséis", "diecisiete",
        "dieciocho", "diecinueve", "veinte"];
    string[] decenas = ["", "", "veinte", "treinta", "cuarenta",
        "cincuenta", "sesenta", "setenta", "ochenta", "noventa"];
    string[] centenas = ["", "ciento", "doscientos", "trescientos",
        "cuatrocientos", "quinientos", "seiscientos", "setecientos",
        "ochocientos", "novecientos"];

    return n switch
    {
        <= 20 => unidades[n],
        < 30 => "veinti" + unidades[n - 20],
        < 100 => decenas[n / 10] + (n % 10 != 0 ? " y " + unidades[n % 10] : ""),
        100 => "cien",
        < 1000 => centenas[n / 100] + (n % 100 != 0 ? " " + ConvertirEspanol(n % 100) : ""),
        < 2000 => "mil" + (n % 1000 != 0 ? " " + ConvertirEspanol(n % 1000) : ""),
        < 1000000 => ConvertirEspanol(n / 1000) + " mil" + (n % 1000 != 0 ? " " + ConvertirEspanol(n % 1000) : ""),
        _ => n.ToString()
    };
}