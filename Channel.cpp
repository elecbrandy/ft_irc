//자자 채널을 만들어 보자

// 할일 순서
// 1. 채널을 만들기 위한 클래스 구조를 만든다.
// 2. join 명령어 공부 (파라미터 구조, 이 파라미터를 어떻게 받아서 처리할 것인가)
// 	2.1 파라미터는 채널명과 비밀번호가 있다.
// 3. 해당 채널명 존재 여부 확인, 없으면 채널 만들기
// 4. 채널에 입장하기
// 5. 채널 참여 완료 관련 처리 e.g)메세지 출력 등


// 1. #secret 채널이 invite-only (+i) 모드
// 2. 운영자가 alice를 초대 (/INVITE alice #secret)
// 3. alice가 초대 목록에 추가됨
// 4. alice가 채널 참여 (/JOIN #secret)
// 5. removeInvite로 alice의 초대 정보 삭제
// 6. alice가 나갔다가 다시 들어오려면 새로운 초대 필요

