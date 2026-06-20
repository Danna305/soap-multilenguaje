// http://localhost:5002/?n=10
// cd version2_traduccion && dotnet run

var builder = WebApplication.CreateBuilder(args);
builder.WebHost.UseUrls("http://localhost:5002");
var app = builder.Build();

app.MapGet("/", async (HttpContext ctx) =>
{
    var n = ctx.Request.Query["n"].ToString();
    if (string.IsNullOrEmpty(n))
    {
        await ctx.Response.WriteAsync("Proporciona un número: http://localhost:5002/?n=10");
        return;
    }

    var soap = $"""
        <?xml version="1.0" encoding="utf-8"?>
        <soap:Envelope xmlns:soap="http://schemas.xmlsoap.org/soap/envelope/">
          <soap:Body>
            <NumberToWords xmlns="http://www.dataaccess.com/webservicesserver/">
              <ubiNum>{n}</ubiNum>
            </NumberToWords>
          </soap:Body>
        </soap:Envelope>
        """;

    using var client = new HttpClient();

    // Llamada SOAP
    var soapContent = new StringContent(soap, System.Text.Encoding.UTF8, "text/xml");
    var soapResp = await client.PostAsync(
        "https://www.dataaccess.com/webservicesserver/NumberConversion.wso",
        soapContent
    );
    var soapBody = await soapResp.Content.ReadAsStringAsync();
    var enIngles = soapBody.Split("<NumberToWordsResult>")[1]
                           .Split("</NumberToWordsResult>")[0].Trim();

    // Traducción con MyMemory API
    var urlTrad = $"https://api.mymemory.translated.net/get?q={Uri.EscapeDataString(enIngles)}&langpair=en|es";
    var tradResp = await client.GetStringAsync(urlTrad);

    var traducido = tradResp.Split("\"translatedText\":\"")[1].Split("\"")[0];

    ctx.Response.ContentType = "text/plain; charset=utf-8";
    await ctx.Response.WriteAsync(traducido);
});

app.Run();