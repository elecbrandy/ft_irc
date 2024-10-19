# ft_irc

## Server 기본 구현
- [x] client - server 기본적 연결
- [x] 닉네임 밑 사용자명 설정 가능
- [ ] 수신받은 패킷 이어 붙이기 (`com^Dman^Dd`)

## Channel 기본 구현
- [ ] 클라이언트가 채널에 참여 가능
- [ ] 클라이언트가 채널에 보낸 모든 메세지는 같은 채널에 속한 다른 클라이언트에게 전달
- [ ] 클라이언트는 운영자(`operators`) 일반사용자 (`regular users`)로 구분

## Channel 내 `operators` 전용 명령어
- [ ] KICK : 채널에서 클라 강제퇴장
- [ ] INVITE : 특정 클라를 채널로 초대
- [ ] TOPIC : 채널 주제 변경 또는 조회
- [ ] MODE : 채널모드 설정/변경
	- `i` : 초대 전용 채널 설정/해제
	- `t` : 운영자만 채널 주제 변경 가능하도록 제한/해제
	- `k` : 채널 비밀번호 설정/해제
	- `o` : 운영자 권한 부여/박탈
	- `l` : 채널 인원 제한 설정/해제

## Ping-pong 구현
- [ ] PING
- [ ] PONG

## Bonus 구현
- [ ] Handle file transfer
- [ ] A bot