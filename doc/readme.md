## Mermaid 사용
\``` mermaid  
\```  
위 코드블럭으로 mermaid 코드블럭을 생성

## Sequence diagram
코드블럭 안에 `sequenceDiagram` 키워드로 사용  
- ->> 실선
- -->> 점선
- : 메시지
- loop end : loop 블럭 생성
- alt else end opt : if else 블럭 생성
- Note right of ??? 메모 생성

``` mermaid
sequenceDiagram
A->>B:->>
B-->>A:-->>
loop Update
    A->>A:AI Do
end
A->>B: LoginReq
Note right of B: 로그인 검사
alt success
    B->>A: LoginResp, user info
else fail
    B->>A: LoginResp, exit
end
```

## Flowchart
코드블럭 안에 `graph TD;` 키워드로 사용
- a --> b a에서 B로 진행
``` mermaid
graph TD;
A-->B;
B-->C;
C-->A;
```

<!-- ## Class diagram
코드블럭 안에 `classDiagram` 키워드로 사용

``` mermaid
classDiagram
TcpServer : int a
TcpServer --* SessionServer
``` -->