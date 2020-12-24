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
alt success
    B->>A: LoginResp, user info
else fail
    B->>A: LoginResp, exit
end
```

## Class diagram
