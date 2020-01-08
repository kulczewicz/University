docker build -t my-golang-app .
docker run -p -d --name=my-running-app my-golang-app
docker cp my-running-app:/go/src/app/lines.txt ./lines.txt
docker stop my-running-app
docker container rm my-running-app