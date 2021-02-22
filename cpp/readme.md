# TelegramBot 설정 방법
1. 

# Boost

#### DBBD 코어 프로젝트에 세팅

#### 다운로드 경로 : https://www.boost.org/users/history/version_1_75_0.html

#### 설치 경로 : cpp/

1. 프로젝트 빌드 시 boost 링크 오류 날 시 부스트 설치 필요

   - boost_1_74_0.zip 압축 해제
   - boost_1_74_0 폴더에서 booststrap.bat 실행
   - 명령 프롬포트에서 boost_1_74_0 폴더 진입 아래 명령어 실행
     - b2 release debug threading=multi --build-type=complete --toolset=msvc address-model=64 stage

2. 프로젝트 설정
   - 추가 포함 디렉터리
     - \$(SolutionDir)..\boost_1_74_0\
   - 추가 라이브러리 디렉터리
     - \$(SolutionDir)..\boost_1_74_0\stage\lib

# Redis

#### GUI Tool 다운로드 경로 : https://www.electronjs.org/apps/p3x-redis-ui

1. 프로젝트 빌드 시 Redis 설치 필요

   1. Redis 설치 전
      - Windows 8.1 SDK 다운로드 및 설치
      - msvc v140 build tool set 다운로드 및 설치
   2. 1번을 진행한 이후
      - cmd에서 cpp폴더로 이동
      - git clone https://github.com/Cylix/cpp_redis.git
      - cd cpp_redis
      - git submodule init && git submodule update
      - 이후 msvc15폴더에 들어가 솔루션 실행 및 빌드
      - build 옵션은 Release, x64로 해주자

2. 프로젝트 설정
   - 추가 포함 디렉터리
     - \$(SolutionDir)..\cpp_redis\includes
     - \$(SolutionDir)..\cpp_redis\tacopie\includes
   - 추가 라이브러리 디렉터리
     - \$(SolutionDir)..\cpp_redis\msvc15\build 옵션(x64\Release)
   - 추가 종속성
     - cpp_redis.lib
     - tacopie.lib

# MySql

#### MySql Connector c++ 다운로드 경로 : https://dev.mysql.com/downloads/connector/cpp/

#### 설치 경로 : cpp/

1. 프로젝트 빌드 시 링크 오류날 시 MySql Connector c++ 설치 필요

   - 경로는 cpp\ 에 설치한다.

2. 프로젝트 설정
   - 추가 포함 디렉터리
     - \$(SolutionDir)..\MySQL\Connector C++ 8.0\include
   - 추가 라이브러리 디렉터리
     - \$(SolutionDir)..\MySQL\Connector C++ 8.0\lib64(Debug, Release)\vs14
   - 추가 종속성
     - mysqlcppconn8.lib
   - 전처리기 세팅
     - \_SILENCE_CXX17_ITERATOR_BASE_CLASS_DEPRECATION_WARNING
     - \_SILENCE_ALL_CXX17_DEPRECATION_WARNINGS

# VLD

#### 하위 프로젝트에 세팅

#### 다운로드 경로 : https://kinddragon.github.io/vld/

#### 설치 경로 : cpp/

1. 프로젝트 빌드 시 vld 링크 오류 날 시 vld 설치 필요

   - vld-2.5.1-setup.exe 설치
     - 설치 시 환경변수는 세팅 필요

2. 프로젝트 설정
   - 추가 포함 디렉터리
     - \$(SolutionDir)..\Visual Leak Detector\include
   - 추가 라이브러리 디렉터리
     - \$(SolutionDir)..\Visual Leak Detector\lib\Win64

# yaml-cpp

#### 설치 경로 : cpp/
1. 설치
   - git clone https://github.com/jbeder/yaml-cpp
   - yaml-cpp 폴더 내에 build 폴더 생성
   - cmake-gui를 이용해 yaml-cpp를 빌드(Test는 COnfigure에서 제외)
   - YAML_CPP.sln 솔루션에서 Debug, Release로 빌드

2. 프로젝트 설정
   - 추가 포함 디렉터리
     - \$(SolutionDir)..\yaml-cpp\include
   - 추가 라이브러리 디렉터리
     - \$(SolutionDir)..\yaml-cpp\build\Debug or Release
   - 추가 종속성
     - Debug : yaml-cppd.lib, Release : yaml-cpp.lib


# spdlog

#### 설치 경로 : cpp/
1. 설치
   - git clone https://github.com/gabime/spdlog.git
   - spdlog 폴더 내에 build 폴더 생성
   - cmake-gui를 이용해 spdlog를 빌드
   - spdlog.sln 솔루션에서 Debug, Release로 빌드

2. 프로젝트 설정
   - 추가 포함 디렉터리
     - \$(SolutionDir)..\spdlog\include
   - 추가 라이브러리 디렉터리
     - \$(SolutionDir)..\spdlog\build\Debug or Release
   - 추가 종속성
     - Debug : spdlogd.lib, Release : spdlog.lib