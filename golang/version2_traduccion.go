// http://localhost:9091/?n=10
// go run version2_traduccion.go

package main

import (
	"encoding/json"
	"fmt"
	"io"
	"net/http"
	"net/url"
	"strings"
)

func main() {
	http.HandleFunc("/", func(w http.ResponseWriter, r *http.Request) {
		n := r.URL.Query().Get("n")
		if n == "" {
			fmt.Fprint(w, "Proporciona un número: http://localhost:9091/?n=10")
			return
		}

		// Llamada SOAP
		soap := `<?xml version="1.0" encoding="utf-8"?>
<soap:Envelope xmlns:soap="http://schemas.xmlsoap.org/soap/envelope/">
  <soap:Body>
    <NumberToWords xmlns="http://www.dataaccess.com/webservicesserver/">
      <ubiNum>` + n + `</ubiNum>
    </NumberToWords>
  </soap:Body>
</soap:Envelope>`

		resp, err := http.Post(
			"https://www.dataaccess.com/webservicesserver/NumberConversion.wso",
			"text/xml; charset=utf-8",
			strings.NewReader(soap),
		)
		if err != nil {
			http.Error(w, err.Error(), 500)
			return
		}
		defer resp.Body.Close()

		body, _ := io.ReadAll(resp.Body)
		bodyStr := string(body)
		enIngles := strings.TrimSpace(
			strings.Split(strings.Split(bodyStr, "NumberToWordsResult>")[1], "<")[0],
		)

		// Traducción con MyMemory API
		apiURL := "https://api.mymemory.translated.net/get?q=" +
			url.QueryEscape(enIngles) + "&langpair=en%7Ces"

		tradResp, err := http.Get(apiURL)
		if err != nil {
			http.Error(w, err.Error(), 500)
			return
		}
		defer tradResp.Body.Close()

		tradBody, _ := io.ReadAll(tradResp.Body)

		var resultado map[string]interface{}
		json.Unmarshal(tradBody, &resultado)

		responseData := resultado["responseData"].(map[string]interface{})
		traducido := responseData["translatedText"].(string)

		w.Header().Set("Content-Type", "text/plain; charset=utf-8")
		fmt.Fprint(w, traducido)
	})

	fmt.Println("Servidor en http://localhost:9091/?n=10")
	http.ListenAndServe(":9091", nil)
}
