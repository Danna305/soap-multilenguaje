// http://localhost:5001/?n=10
// cd version1_soap && dotnet run

var builder = WebApplication.CreateBuilder(args);
builder.WebHost.UseUrls("http://localhost:5001");
var app = builder.Build();

app.MapGet("/", async (HttpContext ctx) =>
{
    var n = ctx.Request.Query["n"].ToString();
    if (string.IsNullOrEmpty(n))
    {
        ctx.Response.ContentType = "text/plain";
        await ctx.Response.WriteAsync("Proporciona un número: http://localhost:5001/?n=10");
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
    var content = new StringContent(soap, System.Text.Encoding.UTF8, "text/xml");
    var response = await client.PostAsync(
        "https://www.dataaccess.com/webservicesserver/NumberConversion.wso",
        content
    );

    var body = await response.Content.ReadAsStringAsync();
    var result = body.Split("NumberToWordsResult>")[1].Split("<")[0];

    ctx.Response.ContentType = "text/plain; charset=utf-8";
    await ctx.Response.WriteAsync(result);
});

app.Run();