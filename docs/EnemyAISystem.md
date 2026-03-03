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

### 현재 구현 (Timer 기반 C++)

Behavior Tree 없이 `ATDEnemyAIController`의 Timer 루프로 이동을 처리한다.

```
ATDEnemyCharacter
  └── ATDEnemyAIController (AAIController)
        ├── OnPossess: RadiusBias 초기화, Timer 시작 (InitialDelay 분산)
        ├── Timer(RepathInterval=0.35s) → UpdateMoveTarget()
        │     ├── GetPlayerPawn()
        │     ├── ComputeMoveGoal() → switch(MovementTactic)
        │     │     ├── DirectChase: PlayerLoc
        │     │     └── Encircle:   ComputeEncircleGoal()
        │     │           └── 슬롯 비용 선택 → CurrentSlotIndex 저장
        │     └── MoveToLocation(Goal, AcceptanceRadius)
        └── OnUnPossess: Timer 해제, CurrentSlotIndex = INDEX_NONE
```

### 계획 (Behavior Tree — 미구현)

공격/사망 등 상태 흐름이 추가되면 Behavior Tree로 전환 예정.

```
Root
 └── Selector
      ├── IsDead → Stop
      └── Sequence
            ├── Set TargetActor
            ├── ComputeDesiredLocation (C++ Task)
            ├── MoveTo
            └── If InRange → Attack
```

------------------------------------------------------------------------

## Core Concept: Encircle Movement

Enemy는 PlayerLocation이 아니라, 플레이어 주변 슬롯 위치를 목표로 이동한다.

### Goal 계산 (현재 구현)

```
슬롯 i 위치:
  Angle   = i * (2π / NumSlots)
  Dir     = WorldForward * cos(Angle) + WorldRight * sin(Angle)
  SlotPos = PlayerLoc + Dir * (EncircleRadius + RadiusBias)

슬롯 i 비용:
  Cost = Dist2D(SelfLoc, SlotPos)
       + OccupancyPenalty  × (반경 내 실제 위치 적 수)
       + SameSlotPenalty   × (같은 슬롯 예약 중인 컨트롤러 수)

→ Cost 최소 슬롯 선택 → MoveToLocation
```

**월드 고정 축(WorldForward/WorldRight) 사용**: PlayerForward 기반 계산은
플레이어 회전 시 목표점이 같이 회전하는 문제가 있어 제거했다.

### Encircle Parameters

-   `NumSlots`: 슬롯 수 (기본 12, 30° 간격)
-   `EncircleRadius`: 전투 스타일에 따라 변경
-   `RadiusBias`: OnPossess에서 1회 고정 (±Jitter 범위, 반경 미세 분산)
-   `OccupancyPenalty` / `SameSlotPenalty`: 중복 점유 방지 비용

------------------------------------------------------------------------

## MovementTactic

`ETDMovementTactic` enum으로 선택.

| 값 | 동작 |
|---|---|
| `DirectChase` | PlayerLocation으로 직진 MoveToLocation |
| `Encircle` | 슬롯 비용 선택 후 MoveToLocation |

------------------------------------------------------------------------

## Combat Style

### V1 — Melee (현재 이동만 구현, 공격 미구현)

-   `EncircleRadius`: 220~320
-   `StopRange`: 120~170 (미구현 — 공격 사거리 내 이동 중단)
-   사거리 진입 시 이동 중단 후 AttackProvider 호출 (예정)

------------------------------------------------------------------------

### V2 — Ranged (Planned)

-   `EncircleRadius`: 600~800
-   MinRange / MaxRange 유지 정책
-   동일 Encircle 로직 재사용, 공격 방식만 변경

------------------------------------------------------------------------

## Attack Architecture (미구현, 설계만)

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

예: Fists / Claws / Body Slam

즉, "무기 없음"이 아니라 "기본 공격 DataAsset 사용"이다.

------------------------------------------------------------------------

### Attack Provider 역할

-   `GetAttackRange()`
-   `GetCooldown()`
-   `PerformAttack()`

PerformAttack 내부에서 Trace / Overlap / Projectile Spawn 등을 수행한다.

------------------------------------------------------------------------

## Performance Rules

-   AI Tick 금지 — Timer 기반 갱신 (RepathInterval ≥ 0.25s)
-   RVO Avoidance 활성화 권장
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

-   Behavior Tree 전환 (공격/사망 상태 흐름 추가 시)
-   AI Perception 도입
-   웨이브 기반 EncircleRadius 조정
-   협동형 AI
-   WeaponDA 기반 Enemy 자동 세팅
