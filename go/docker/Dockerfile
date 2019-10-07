FROM golang as builder

RUN go get github.com/ascheglov/faulty-server/go/cmd/faulty-server \
 && CGO_ENABLED=0 GOOS=linux go build -a -installsuffix cgo -o /bin/faulty-server github.com/ascheglov/faulty-server/go/cmd/faulty-server

FROM scratch

COPY --from=builder /bin/faulty-server /bin/faulty-server

CMD ["/bin/faulty-server"]

EXPOSE 8080
