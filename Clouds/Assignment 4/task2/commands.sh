docker build -t my-golang-app .
docker run -it --rm --name my-running-app my-golang-app
docker cp my-running-app:/go/src/app/lines.txt ./lines.txt
docker stop my-running-app