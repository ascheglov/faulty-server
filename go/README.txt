# Manual setup

Docker login (for 12h)

    $ sudo docker login --username iam --password `yc iam create-token` cr.yandex

Build & push

    $ sudo docker build -t cr.yandex/crpp5u2jfob5u0afkcf5/f_server:v1 .
    $ sudo docker push cr.yandex/crpp5u2jfob5u0afkcf5/f_server:v1

Run

    $ sudo docker run -d --restart always -p 8080:8080 --name f_server cr.yandex/crpp5u2jfob5u0afkcf5/f_server:v1
