# DBBD Core

- ## 주요 git Library
  - ### redis : https://github.com/luca3m/redis3m
    - redis3m library
    - boost, hiredis 필요
    - connection pooling 지원
    - Linux, Windows 지원

- ## Server
TcpServerBase를 상속받아 서버를 생성할 수 있다.  
start()로 실행하며 stop()으로 중지할 수 있다.
``` cpp
void main(){
    SessionServer server("Session", "127.0.0.1", 8100, 4); // 서버명, ip, port, 스레드 갯수
    server.start();

    if(종료){
        server.stop();
    }
}
```

- ### 상속받아 구현해야하는 함수
  - void stopInternal() : 서버 중지전에 처리해야할 작업들을 해준다
  - void acceptInternal(DBBD::SocketSP, size_t) : accept된 soccket과 session id를 파라미터로 받아 session을 생성해준다
    - 생성된 session에 bindingStopInternal을 이용하여 binding 해준다.
  - void disconnectInternal(size_t) : disconnect되는 socket의 session id를 받아 종료 로직을 작성한다.


``` cpp
void stopInternal(){
    server에 연결된 session들 종료 처리
}

void acceptInternal(DBBD::SocketSP, size_t){
    session = 세션 생성(socket);
    session->bindingStopInternal(바인드(&disconnectInternal, this));
    sessionMap에 추가(sessionId, session);
    session->start();
}

void disconnectInternal(size_t){
    session = sessionMap[sessionId];
    session->stop();
}
```

- ## Client
TcpClientBase를 상속받아 기본적인 클라이언트를 생성할 수 있다.  
server에 연결되어 세션을 생성해줄때 TcpClientSession을 이용해 주고 생성할 때  
readInternal를 생성자에 넘겨준다.
``` cpp
void main(){
    PlayerClient client("127.0.0.1", 8100, true) // 접속할 서버 ip, 접속할 서버 port, 재접속 여부
    client.start();

    if(종료){
        client.stop();
    }
}
```

### 상속받아 구현해야하는 함수
- void startInternal() : 서버 시작중 처리해야할 작업들을 해준다
- void stopInternal() : 서버 종료중 처리해야할 작업들을 해준다
- void readInternal(DBBD::Buffer) : session의 read 처리를 위해 바인딩 될 함수
  - 하나의 프로토콜 블럭을 담은 buffer가 넘어오므로 header를 읽은 후 해당하는 프로토콜로 deserialize 해주자

``` cpp
void readInternal(DBBD::Buffer buffer)
{
    headerBlock = buffer.viewByteBlock(DBBD::HeaderSize);
    DBBD::Header header(headerBlock);

    switch(header.typeId){
        case ...Req:
        break;
        case ...Req:
        break;
    }
}
```