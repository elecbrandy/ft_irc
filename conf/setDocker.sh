#!/bin/bash

# Docker 설치 여부 확인 및 설치
if ! command -v docker &> /dev/null; then
    curl -fsSL https://get.docker.com -o get-docker.sh
    sh get-docker.sh
    rm get-docker.sh
fi

# Docker 이미지로 Ubuntu 컨테이너 생성 및 실행
docker pull ubuntu:latest
docker run -d --name irc_container -it ubuntu:latest

# Ubuntu 컨테이너에 필요한 패키지 설치
docker exec -it irc_container bash -c "apt update && apt install -y inspircd irssi"

# /var/run/inspircd 디렉토리 생성 및 권한 설정
docker exec -it irc_container bash -c "mkdir -p /var/run/inspircd && chown -R 39:39 /var/run/inspircd"

# inspircd.conf 파일에서 서버 패스워드 및 바인딩 설정
docker exec -it irc_container bash -c "sed -i '/<connect allow=\"\*\"/a password=\"1234\"' /etc/inspircd/inspircd.conf"
docker exec -it irc_container bash -c "sed -i 's/<bind address=\"127.0.0.1\"/<bind address=\"0.0.0.0\"/' /etc/inspircd/inspircd.conf"

# IRC 서버 및 클라이언트 실행 명령어 안내
RED='\033[0;31m'
GREEN='\033[0;32m'
NC='\033[0m'

echo -e "${RED}docker exec -it irc_container inspircd --runasroot --nofork --debug${NC}" # IRC 서버 실행
echo -e "${GREEN}docker exec -it irc_container irssi -c 127.0.0.1 -p 6667 -w 1234 -n nickname${NC}" # IRC 클라이언트 접속
