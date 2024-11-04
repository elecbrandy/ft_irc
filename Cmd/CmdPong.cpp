/*
    Pong
    Command: PONG
    매개변수: <daemon> [<daemon2>]

    PONG 메시지는 PING 메시지에 대한 응답으로 사용됩니다. <daemon2> 
    매개변수가 주어지면 이 메시지는 해당 데몬으로 전달되어야 합니다. 
    <daemon> 매개변수는 PING 메시지에 응답하고 이 메시지를 생성한 
    데몬의 이름입니다.

    에러 응답 코드:

           ERR_NOORIGIN                    ERR_NOSUCHSERVER

    예시:

    PONG csd.bu.edu tolsun.oulu.fi  ; csd.bu.edu에서 tolsun.oulu.fi로 
                                     PONG 메시지를 전송하는 경우


*/