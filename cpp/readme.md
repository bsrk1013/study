# Boost

##### DBBD 코어 프로젝트에 세팅

1. 프로젝트 빌드 시 boost 링크 오류 날 시 부스트 설치 필요

   - boost_1_74_0.zip 압축 해제
   - boost_1_74_0 폴더에서 booststrap.bat 실행
   - 명령 프롬포트에서 boost_1_74_0 폴더 진입 아래 명령어 실행  
     ~ b2 release debug threading=multi --build-type=complete --toolset=msvc address-model=64 stage

2. 프로젝트 링크 설정
   - 추가 포함 디렉터리
     - \$(SolutionDir)..\boost_1_74_0\
   - 추가 라이브러리 디렉터리
     - \$(SolutionDir)..\boost_1_74_0\stage\lib

# VLD

##### 하위 프로젝트에 세팅

1. 프로젝트 빌드 시 vld 링크 오류 날 시 vld 설치 필요

   - vld-2.5.1-setup.exe 설치
     - 설치 시 환경변수는 세팅 불필요

2. 프로젝트 링크 설정
   - 추가 포함 디렉터리
     - \$(SolutionDir)..\Visual Leak Detector\include
   - 추가 라이브러리 디렉터리
     - \$(SolutionDir)..\Visual Leak Detector\lib\Win64