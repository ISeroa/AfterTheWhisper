# Noise-Driven Encounter System

## Overview
고정된 스테이지 구조에서 플레이마다 다른 교전 상황을 만들기 위한 Encounter 시스템.

플레이어의 소음(Noise) 을 중심으로 좀비가 반응하고 전투 상황이 확장되며, 생존자 AI는 소음에 따라 전술 상태를 변경한다.
이를 통해 맵은 고정이지만 플레이 경험은 매번 달라지는 구조를 만든다.

## Key Decisions
- 생존자는 추가 스폰되지 않는다
    - 인간 AI는 맵 시작 시 배치된 개체만 존재
    - 난이도 변화는 행동 변화(경계 / 포지셔닝)로만 발생
- 좀비는 소음에 확률 기반으로 반응
    - 항상 몰려오는 구조가 아닌 거리 + 확률 기반 반응
    - 플레이어 행동이 난이도를 간접적으로 증가시킴
- 소음 반응과 증원 스폰을 분리
    - 대부분의 반응은 기존 AI 행동 변화로 처리
    - 추가 스폰은 제한적으로만 발생
- Director 기반 전투 흐름 제어
    - 전투 밀도를 제어하는 중앙 관리 시스템 존재
    - 무작위가 아닌 “연출된 랜덤”을 목표로 함

## Architecture
###Core Flow
Weapon Fire
    ↓
Noise Event 발생
    ↓
Encounter Director 전달
    ↓
1) 주변 좀비 반응
2) 생존자 전술 상태 변경
3) 조건 충족 시 좀비 증원 스폰

###Noise Event
Noise Event는 다음 정보를 포함한다.
- Noise Location
- Noise Loudness
- Noise Radius
소음은 다음 상황에서 발생할 수 있다.
- 무기 발사
- 폭발
- 특정 환경 이벤트

###Zombie Reaction
좀비는 Noise Event에 대해 거리 기반 확률 반응을 한다.

반응 조건
distance <= noiseRadius

반응 확률
가까울수록 높은 확률
멀수록 낮은 확률

반응 행동
Idle
  → Investigate (소음 위치 이동)
  → Player 발견 시 Chase
이를 통해 총격이 발생하면 주변 좀비가 점점 전투에 합류하는 구조가 된다.

###Survivor Reaction
생존자는 소음에 대해 스폰이 아닌 전술 상태 변화만 수행한다.
예시
Idle
  → Alert
  → Cover Position 이동
  → Encircle 시도
소음 방향을 기준으로 포지션을 조정하여 플레이어를 견제한다.

###Zombie Reinforcement
소음 이벤트는 조건 충족 시 추가 좀비 스폰을 유발할 수 있다.

하지만 무한 스폰을 방지하기 위해 다음 제한이 존재한다.
- Reinforcement Cooldown
- Max Zombie Count
- Area Enemy Cap
이 제한을 통해 전투 규모를 안정적으로 유지한다.

### Encounter Director
Director 시스템은 전투 밀도를 조절한다.

역할
- Noise 이벤트 처리
- 좀비 반응 브로드캐스트
- 증원 스폰 판단
- 전투 밀도 유지
설계 원칙
- Event-driven 구조
- Tick 기반 로직 최소화
- Timer 기반 제어

이 구조는 프로젝트의 Event-driven Architecture 원칙과 일치한다. 

## Trade-off
###장점
- 작은 맵에서도 다양한 플레이 경험 제공
- 플레이어 행동에 따른 난이도 변화
- AI 행동 다양성 증가
- 시스템 확장 용이

###단점
- 튜닝이 부족하면 전투 밀도가 불안정할 수 있음
- 소음 시스템이 과도하면 전투가 빠르게 확장될 위험
- Director 파라미터 밸런싱 필요

## Future
- Dynamic Horde Event 시스템
- Stealth 기반 플레이 확장
- Noise 종류 세분화 (Explosion / Gunshot / Environmental)
- AI Communication 시스템
- Adaptive Difficulty Director
