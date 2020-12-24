# DBBD 서버 구조

## 서버의 종류
- ### LoginServer(웹서버로 LB처리?)
  현재 SessionServer에 접속해 있는  
  유저의 수를 확인해 SessionServer를 선택해 주는  
  간단한 LB 역할을 하는 서버
- ### SessionServer(확장 가능)
  게임의 실질적인 컨텐츠 처리를 담당하는 서버
- ### CommunityServer(확장 불가)
  여러 머신, 여러 Server로 나뉘어 있는 SessionServer에  
  통합처리가 필요한 작업을 담당하는 서버
- ### ChattingServer(확장 불가)
  유저 Client가 직접 접속하여 채팅만을 담당
