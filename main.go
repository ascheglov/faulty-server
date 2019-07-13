package main

import (
	"context"
	"fmt"
	"log"
	"net/http"
	"os"
	"time"
)

func main() {
	srv := &http.Server{Addr: ":8080"}
	http.HandleFunc("/", func(w http.ResponseWriter, r *http.Request) {
		log.Printf("%s %s %s", r.Method, r.RequestURI, r.Proto)
		w.Header().Set("Content-Type", "text/html; charset=utf-8")
		fmt.Fprint(w, "<pre>\n")
		r.Write(w)
		fmt.Fprint(w, "</pre>\n")
		fmt.Fprintf(w, "<hr/>%s<hr/>\n", time.Now().Format(time.RFC1123))
		fmt.Fprint(w, `<a href="/kill">Kill server</a>&nbsp;|&nbsp;`)
		fmt.Fprint(w, `<a href="/shutdown">Shutdown server</a>`)
	})
	http.HandleFunc("/kill", func(w http.ResponseWriter, r *http.Request) {
		log.Print("kill!")
		os.Exit(1)
	})
	http.HandleFunc("/shutdown", func(w http.ResponseWriter, r *http.Request) {
		w.WriteHeader(http.StatusNoContent)
		go func() {
			log.Print("shutdown")
			time.Sleep(time.Second)
			srv.Shutdown(context.Background())
		}()
	})
	log.Print("starting")
	log.Print(srv.ListenAndServe())
}
