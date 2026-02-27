# Enemy AI System

## Overview

Enemy AI는 플레이어를 추적하고 공격하는 전투 루프를 담당한다.

단순 돌진이 아닌, 플레이어 주변으로 퍼져서 접근하는 Encircle 기반
포지셔닝을 핵심 구조로 한다.

본 시스템은 근접 적(V1)을 시작점으로 하며, 원거리 적(V2) 및 무기 타입
확장까지 고려하여 설계되었다.

------------------------------------------------------------------------

## Design Goals

-   Tick 최소화
-   데이터 기반 무기 공유 (Player ↔ Enemy)
-   공격 책임 분리
-   근접/원거리 확장 가능 구조
-   포트폴리오 가시성 확보 (Hybrid AI 구조)

------------------------------------------------------------------------

## Architecture

### Hybrid AI Structure

-   Behavior Tree → 상태 흐름 관리
-   C++ Task → Encircle 포지셔닝 계산
-   EnemyCharacter → 공격 의사결정
-   Attack Provider → 실제 공격 판정

------------------------------------------------------------------------

## Core Concept: Encircle Movement

Enemy는 플레이어 위치가 아니라, 플레이어 주변 슬롯 위치를 목표로
이동한다.

    DesiredPosition =
        PlayerLocation
      + Rotated(PlayerForward, SlotAngle)
        * EncircleRadius
      + RandomJitter

### Encircle Parameters

-   SlotAngle: UniqueID 기반 고정값
-   EncircleRadius: 전투 스타일에 따라 변경
-   RandomJitter: 인위적인 배치 방지

------------------------------------------------------------------------

## Behavior Tree Flow

    Root
     └── Selector
          ├── IsDead → Stop
          └── Sequence
                ├── Set TargetActor
                ├── ComputeDesiredLocation (C++ Task)
                ├── MoveTo
                ├── If InRange → Attack

Tick 사용 ❌
Service 또는 Timer 기반 갱신 ✅

------------------------------------------------------------------------

## Combat Style

### V1 -- Melee

-   EncircleRadius: 220~320
-   StopRange: 120~170
-   사거리 진입 시 이동 중단
-   EnemyCharacter → AttackProvider 호출

------------------------------------------------------------------------

### V2 -- Ranged (Planned)

-   EncircleRadius: 600\~800
-   MinRange / MaxRange 유지 정책
-   동일 Encircle 로직 재사용
-   공격 방식만 변경

------------------------------------------------------------------------

## Attack Architecture

### 핵심 원칙

EnemyCharacter는 공격의 "의사결정자"
AttackProvider는 공격의 "구현자"

------------------------------------------------------------------------

### Weapon DataAsset 공유

플레이어와 적은 동일한 Weapon DataAsset 구조를 사용한다.

-   데미지
-   사거리
-   쿨다운
-   공격 타입
-   이펙트 정보

Enemy는 WeaponDA만 교체함으로써 다른 무기 타입을 사용할 수 있다.

------------------------------------------------------------------------

### 무기 없는 적 처리

무기를 사용하지 않는 적도 WeaponDA를 가진다.

예: - Fists - Claws - Body Slam

즉, "무기 없음"이 아니라 "기본 공격 DataAsset 사용"이다.

------------------------------------------------------------------------

### Attack Provider 역할

-   GetAttackRange()
-   GetCooldown()
-   PerformAttack()

PerformAttack 내부에서 Trace / Overlap / Projectile Spawn 등을 수행한다.

------------------------------------------------------------------------

## Performance Rules

-   AI Tick 금지
-   Service 주기 ≥ 0.25s
-   RVO Avoidance 활성화
-   CrowdFollowing 사용 고려

------------------------------------------------------------------------

## Trade-offs

-   컴포넌트 분리로 구조 복잡도 증가
-   초기 설계 비용 발생

하지만:

-   근접/원거리 확장 용이
-   Player/Enemy 무기 데이터 통합
-   책임 분리 명확
-   포트폴리오 설계 설명에 유리

------------------------------------------------------------------------

## Future Expansion

-   AI Perception 도입
-   슬롯 재정렬 로직
-   웨이브 기반 EncircleRadius 조정
-   협동형 AI
-   WeaponDA 기반 Enemy 자동 세팅
