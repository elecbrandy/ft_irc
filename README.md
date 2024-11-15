# ft_irc

## docker set

1. **docker desktop** 키기

2. `main` 브런치에 넣어둔 **setDocker.sh** 실행 (`./setDocker.sh`)

3. 터미널 창을 두개 띄우고, 각각의 창에 irc 서버, 클라이언트 열기
- 첫번째 터미널에 서버를 : `docker exec -it irc_container inspircd --runasroot --nofork --debug`
- 두번째 터미널에 클라를 :
	- `docker exec -it irc_container /bin/bash` 로 접속해서 직접 `irssi` 켜기 또는  
	- `docker exec -it irc_container irssi -c 127.0.0.1 -p 6667 -w 1234 -n nickname` 으로 한번에 접속 - 전자가 편한듯? 
4. kill, rm 알아서
5. 도커에서 클라를 열고 로컬 서버에 접속할때는 ip주소 `host.docker.internal`
	- `docker exec -it irc_container /bin/bash` -> `/connect host.docker.internal 6667 1234`

__________

## checklist
### Server 기본 구현
- [x] client - server 기본적 연결
- [x] 닉네임 밑 사용자명 설정 가능
- [ ] 수신받은 패킷 이어 붙이기 (`com^Dman^Dd`)

### Cmd
- [x] PASS
- [x] USER
- [x] PING
- [x] NICK

### Channel 기본 구현
- [x] 클라이언트가 채널에 참여 가능
- [x] 클라이언트가 채널에 보낸 모든 메세지는 같은 채널에 속한 다른 클라이언트에게 전달
- [x] 클라이언트는 운영자(`operators`) 일반사용자 (`regular users`)로 구분

### Channel 내 `operators` 전용 명령어
- [ ] KICK : 채널에서 클라 강제퇴장
- [ ] INVITE : 특정 클라를 채널로 초대
- [ ] TOPIC : 채널 주제 변경 또는 조회
- [ ] MODE : 채널모드 설정/변경
	- `i` : 초대 전용 채널 설정/해제
	- `t` : 운영자만 채널 주제 변경 가능하도록 제한/해제
	- `k` : 채널 비밀번호 설정/해제
	- `o` : 운영자 권한 부여/박탈
	- `l` : 채널 인원 제한 설정/해제

### Bonus 구현
- [ ] Handle file transfer
- [ ] A bot

### etc

#### `recv` 의 반환값
- 리눅스 man 페이지에 따르면, 논블로킹 소켓에서 recv 호출 시 읽을 데이터가 없으면 -1을 반환하고 errno를 EAGAIN 또는 EWOULDBLOCK으로 설정.
- https://www.man7.org/linux/man-pages/man2/recv.2.html

#### `EAGAIN` 과 `EWOULDBLOCK` 의 의미
- `EAGAIN`: 비동기 소켓에서 호출이 즉시 완료될 수 없을 때 반환되는 오류 코드
	- 읽기(recv)의 경우: 수신 버퍼에 데이터가 없을 때 recv가 -1을 반환하고 errno에 EAGAIN이 설정
	- 쓰기(send)의 경우: 송신 버퍼가 가득 찬 상태에서 send를 호출하면 -1을 반환하고 errno에 EAGAIN이 설정
- 예시: `EAGAIN`이 발생하는 상황
	- 읽기(recv) 작업 시
		- 소켓이 논블로킹 모드로 설정되어 있고, 수신 버퍼에 데이터가 없는 경우 recv는 -1을 반환하고 errno에 EAGAIN이 설정
		- 대처 방법: 이 상황에서는 데이터를 기다리는 대신 다음 이벤트를 기다리거나 다시 시도하는 방식으로 처리
	- 쓰기(send) 작업 시
		- 송신 버퍼가 꽉 차서 더 이상 데이터를 보내지 못하는 경우, send는 -1을 반환하고 errno에 EAGAIN이 설정됩
		- 대처 방법: 송신 버퍼가 비워질 때까지 기다렸다가, 다시 시도

#### POLLIN과 POLLOUT
- POLLIN 값: 0x0001 (2진수로 0001)
- POLLOUT 값: 0x0004 (2진수로 0100)
- 비트 연산을 통해 POLLIN | POLLOUT을 계산하면: 0001 | 0100 = 0101 (10진수로 5)
- `1010`을 감지한 **poll** 함수는  pollin과 pollout 둘다 인지


### asdf
``` bash

aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaacc

```