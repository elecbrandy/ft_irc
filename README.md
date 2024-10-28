# ft_irc

dongwook 작업일지
- Cmd 디렉토리 만들어서, Cmd 클래스 관련 파일 정리
- Cmd 클래스가 명령어 추출, 명령어 핸들, 명령어 실행 함수 모두 관리함
- 실제 irssi 와 연결 시 순서
	- 처음 접속 시 클라가 `CAP LS 302` 보내면 서버에서 `CAP * LS :` 답장해야 함
	- 이후 `NICK`, `USER` 호출
	- 이때 `NICK`, `USER` 호출 이전에 `PASS` 강제 되도록 해야할듯. 우린 비번 있으니까

### Server 기본 구현
- [x] client - server 기본적 연결
- [x] 닉네임 밑 사용자명 설정 가능
- [ ] 수신받은 패킷 이어 붙이기 (`com^Dman^Dd`)

### Cmd
- [ ] PASS : 첫 접속 때 비번 강제하게 해야할듯? 클라클래스에 bool 타입으로 인증했는지 확인하는거 만들어서
- [x] USER
- [ ] PING : 클라에서 임의로 `/ping a` 입력 시 `PRIVMSG a :PING 1730057358 334518` 이런 식으로 입력됨. 이때 PRIVMSG 등 처리 해야할듯
- [x] NICK : 완성하긴했는데, MAX_LEN 어떻게 할지 정해야 함. 제대로 바뀌었는지 확인 하는 방법을 넣어야 하나? 중복 등 아직 테스트 안해봄

### Channel 기본 구현
- [x] 클라이언트가 채널에 참여 가능
- [ ] 클라이언트가 채널에 보낸 모든 메세지는 같은 채널에 속한 다른 클라이언트에게 전달
- [ ] 클라이언트는 운영자(`operators`) 일반사용자 (`regular users`)로 구분

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

### 변경 사항
- 굿

____

## homebrew 설치

``` bash
git clone https://github.com/Homebrew/brew ~/homebrew

echo 'export PATH="$HOME/homebrew/bin:$PATH"' >> ~/.zshrc
source ~/.zshrc

brew help

brew install irssi
```


