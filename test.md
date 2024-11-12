# 테스트

## clone

``` bash
git clone git@github.com:gotjd7837/IRC.git tmp1
git clone git@github.com:marineks/Ft_irc.git tmp2
git git@github.com:barimehdi77/ft_irc.git tmp3
git https://github.com/lpincoli/FT_IRC42 tmp4

```

## `/pass`
- 인증 시 `/pass 1234` : 비번 틀린 경우
- 인증 후 `/pass ~` : 이미 인증된 유저입니다.

## `/nick`
- `/nick dongwook` : 정상작동
- `/nick 123456789012345678901` : MAX_LEN 초과(20)
- `/nick dongwook` : 이미 존재하는 닉네임
- `/nick` : 인자 부족
- `/nick dongwook$%!` : 숫자, 알파벳만 가능

## `/user`
- 인증 후 `/user ~` : 이미 인증된 유저입니다.

## 논소켓블로킹
- 클라이언트1 버퍼 읽는 중 클라이언트2에 제대로 반응하는지 확인

## `^D`
- 서브젝트에 나와있는 `^D` 예시 다양하게 테스트 (`/com^Dman^Dd`)
- irssi -> 상용서버, 우리서버, 다른사람서버
- nc -> 상용서버, 우리서버, 다른사람서버

## 512자
- irssi는 한번에 최대 `/r/n` 포함 413글자 전송 가능
- 아래 `a`(411) + `c`(2) 조합 보내보기
- irssi -> 상용서버, 우리서버, 다른사람서버
- nc -> 상용서버, 우리서버, 다른사람서버

``` bash
# 413: (411 + 2)
aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaacc
```

``` bash
# 512: (510 + 2)
aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaacc
```
