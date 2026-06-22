// http://localhost:9090/?n=10
// go run version1_soap.go

package main

import (
	"fmt"
	"io"
	"net/http"
	"strings"
)

func main() {
	http.HandleFunc("/", func(w http.ResponseWriter, r *http.Request) {
		n := r.URL.Query().Get("n")
		if n == "" {
			fmt.Fprint(w, "Proporciona un número: http://localhost:9090/?n=10")
			return
		}

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
		result := strings.Split(strings.Split(bodyStr, "NumberToWordsResult>")[1], "<")[0]
		fmt.Fprint(w, result)
	})

	fmt.Println("Servidor en http://localhost:9090/?n=10")
	http.ListenAndServe(":9090", nil)
}
