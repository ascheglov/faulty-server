FROM golang 

RUN go get github.com/ascheglov/faulty-server

ENTRYPOINT /go/bin/faulty-server

EXPOSE 8080
