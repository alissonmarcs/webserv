FROM nginx
RUN apt-get update && apt-get install -y procps fcgiwrap lynx
