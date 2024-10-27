
## RFC 2812

> https://datatracker.ietf.org/doc/html/rfc2812#section-2.3.1

### IRC 메시지 형식 정의 (확장된 BNF 형태)

IRC 프로토콜 메시지는 연속적인 옥텟(8비트 데이터) 스트림에서 추출됩니다. 각 메시지는 **CR(캐리지 리턴)**과 **LF(라인 피드)**를 통해 구분되며, 빈 메시지는 무시됩니다.

#### 1. 메시지 구성 요소
- **메시지 형식**: `message = [ ":" prefix SPACE ] command [ params ] crlf`
  - **prefix**: 발신자 정보로, 서버 이름(servername) 또는 닉네임(nickname) 형식을 가집니다.
  - **command**: 메시지의 명령 부분으로, 알파벳 문자로 구성되거나 3자리 숫자 형식입니다.
  - **params**: 최대 14개의 매개변수이며, 공백(SPACE)으로 구분됩니다. 마지막 매개변수는 `:`로 시작해 공백을 포함할 수 있습니다.

#### 2. 구성 요소 상세 정의
- **prefix**:
  - `prefix = servername / ( nickname [ [ "!" user ] "@" host ] )`
  - 예시: 닉네임과 사용자 이름, 호스트 정보가 포함될 수 있습니다.

- **command**:
  - `command = 1*letter / 3digit`
  - 알파벳으로 시작하거나 3자리 숫자로 표현됩니다.

- **params**:
  - `params = *14( SPACE middle ) [ SPACE ":" trailing ]`
  - 14개의 `middle` 파라미터와 선택적인 `trailing` 파라미터로 구성됩니다.
  - `middle`: 공백이나 `:`를 포함하지 않는 문자열입니다.
  - `trailing`: 공백, `:` 및 공백이 아닌 문자열을 포함할 수 있는 마지막 파라미터입니다.

#### 3. 문자 및 기호 정의
- **nospcrlfcl**: `nospcrlfcl = %x01-09 / %x0B-0C / %x0E-1F / %x21-39 / %x3B-FF`
  - NUL(%x00), CR, LF, 공백, `:`를 제외한 모든 문자입니다.
- **SPACE**: `%x20` (공백)
- **crlf**: `%x0D %x0A` (캐리지 리턴 + 라인 피드)

---

### 주요 추가 규칙

#### 닉네임 및 채널 관련 규칙
- **nickname**:
  - `nickname = ( letter / special ) *8( letter / digit / special / "-" )`
  - 영문 문자나 특수 문자로 시작하며 최대 9자로 구성됩니다.
  - **허용 특수 문자**: `[`, `]`, `\`, "`", `_`, "^", `{`, "|", "}"
  
- **channel**:
  - `channel = ( "#" / "+" / ( "!" channelid ) / "&" ) chanstring [ ":" chanstring ]`
  - 채널 이름은 `#`, `+`, `!`, `&` 중 하나로 시작하며, 특수 문자열을 포함할 수 있습니다.

#### 기타 필드 정의
- **user**: `user = 1*( %x01-09 / %x0B-0C / %x0E-1F / %x21-3F / %x41-FF )`
  - NUL, CR, LF, 공백, `@`를 제외한 모든 문자.
  
- **host**: `host = hostname / hostaddr`
  - 호스트 이름 또는 IP 주소 형식입니다.
  
- **hostname**:
  - `hostname = shortname *( "." shortname )`
  - 호스트 이름의 최대 길이는 63자입니다.

---
