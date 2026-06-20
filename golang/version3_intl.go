// http://localhost:9092/?n=10
// go run version3_intl.go

package main

import (
	"fmt"
	"net/http"
	"strconv"
)

func main() {
	http.HandleFunc("/", func(w http.ResponseWriter, r *http.Request) {
		n := r.URL.Query().Get("n")
		if n == "" {
			fmt.Fprint(w, "Proporciona un número: http://localhost:9092/?n=10")
			return
		}

		numero, err := strconv.ParseInt(n, 10, 64)
		if err != nil {
			http.Error(w, "Número inválido", 400)
			return
		}

		w.Header().Set("Content-Type", "text/plain; charset=utf-8")
		fmt.Fprint(w, convertirEspanol(numero))
	})

	fmt.Println("Servidor en http://localhost:9092/?n=10")
	http.ListenAndServe(":9092", nil)
}

func convertirEspanol(n int64) string {
	if n == 0 {
		return "cero"
	}
	if n < 0 {
		return "menos " + convertirEspanol(-n)
	}

	unidades := []string{"", "uno", "dos", "tres", "cuatro", "cinco",
		"seis", "siete", "ocho", "nueve", "diez", "once", "doce",
		"trece", "catorce", "quince", "dieciséis", "diecisiete",
		"dieciocho", "diecinueve", "veinte"}
	decenas := []string{"", "", "veinte", "treinta", "cuarenta",
		"cincuenta", "sesenta", "setenta", "ochenta", "noventa"}
	centenas := []string{"", "ciento", "doscientos", "trescientos",
		"cuatrocientos", "quinientos", "seiscientos", "setecientos",
		"ochocientos", "novecientos"}

	switch {
	case n <= 20:
		return unidades[n]
	case n < 30:
		return "veinti" + unidades[n-20]
	case n < 100:
		res := decenas[n/10]
		if n%10 != 0 {
			res += " y " + unidades[n%10]
		}
		return res
	case n == 100:
		return "cien"
	case n < 1000:
		res := centenas[n/100]
		if n%100 != 0 {
			res += " " + convertirEspanol(n%100)
		}
		return res
	case n < 2000:
		res := "mil"
		if n%1000 != 0 {
			res += " " + convertirEspanol(n%1000)
		}
		return res
	case n < 1000000:
		res := convertirEspanol(n/1000) + " mil"
		if n%1000 != 0 {
			res += " " + convertirEspanol(n%1000)
		}
		return res
	default:
		return strconv.FormatInt(n, 10)
	}
}