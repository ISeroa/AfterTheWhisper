# Enemy Encircle Movement

## Goal

Enemy가 플레이어를 단순 추적(돌진)하는 것이 아니라, 플레이어 주변의 원(EncircleRadius) 위
고정 슬롯 위치를 목표로 이동하여 자연스럽게 퍼지고 둘러싸는 포지셔닝을 만든다.

이 문서는 **이동/포지셔닝만** 다룬다. (공격/애니/무기/BT 제외)

---

## Core Idea

플레이어 주변에 NumSlots개의 고정 슬롯 위치를 배치한다.
각 적은 매 RepathInterval마다 "거리 + 점유 비용"이 가장 낮은 슬롯을 선택해 MoveToLocation한다.

```
슬롯 i 위치:
  Angle   = i * (2π / NumSlots)
  Dir     = WorldForward * cos(Angle) + WorldRight * sin(Angle)   ← 플레이어 회전 무관
  SlotPos = PlayerLoc + Dir * (EncircleRadius + RadiusBias)

슬롯 i 비용:
  Cost = Dist2D(SelfLoc, SlotPos)
       + OccupancyPenalty  × (반경 OccupancyRadius 안에 실제 위치한 다른 적 수)
       + SameSlotPenalty   × (CurrentSlotIndex == i 인 다른 ATDEnemyAIController 수)

→ Cost 최소 슬롯을 선택 → CurrentSlotIndex 저장 → MoveToLocation
```

**월드 고정 축 사용 이유**: PlayerForward 기반으로 계산하면 플레이어가 제자리 회전할 때
적 목표 위치도 같이 회전하는 문제가 발생한다.

---

## MovementTactic

`ETDMovementTactic` enum으로 전술 선택.

| 값 | 동작 |
|---|---|
| `DirectChase` | 매 틱 PlayerLocation으로 MoveToLocation |
| `Encircle` | 슬롯 비용 선택 후 MoveToLocation |

---

## 중복 점유 방지 구조

### 1) 위치 기반 — OccupancyPenalty
다른 적이 슬롯 근처에 실제로 있을 때 비용을 올린다.
`UGameplayStatics::GetAllActorsOfClass(APawn)` 순회 후 `Dist2D < OccupancyRadius` 체크.

### 2) 의도 기반 — SameSlotPenalty
다른 `ATDEnemyAIController`가 이미 같은 슬롯을 예약했을 때 비용을 압도적으로 올린다.
`TActorIterator<ATDEnemyAIController>` 순회 후 `Other->CurrentSlotIndex == i` 체크.
`SameSlotPenalty`(기본 100000)는 거리 비용을 압도해야 실질적으로 중복이 차단된다.

### 3) 시간 분산 — InitialDelay
`OnPossess`에서 Timer 시작 시 `RandRange(0, RepathInterval)`을 초기 딜레이로 줘서
동시에 같은 슬롯을 선택하는 경쟁을 줄인다.

### 4) 반경 분산 — RadiusBias
`OnPossess`에서 `RandRange(-Jitter, Jitter)`로 1회 고정.
같은 슬롯을 선택해도 반경이 달라 완전히 겹치지 않는다.

### CurrentSlotIndex 생명주기

| 이벤트 | 값 |
|---|---|
| OnPossess | `INDEX_NONE` (초기화) |
| 슬롯 선택 | 선택된 인덱스 저장 |
| OnUnPossess | `INDEX_NONE` (즉시 해방) |

---

## UPROPERTY 파라미터

| 프로퍼티 | 기본값 | 설명 |
|---|---|---|
| `RepathInterval` | 0.35 | 슬롯 재선택 주기 (초) |
| `AcceptanceRadius` | 60 | MoveToLocation 도착 판정 반경 |
| `EncircleRadius` | 280 | 플레이어 중심 원 반경 |
| `Jitter` | 30 | RadiusBias 랜덤 범위 (±) |
| `NumSlots` | 12 | 원 위 슬롯 수 (30° 간격) |
| `OccupancyRadius` | 160 | 위치 기반 점유 판정 반경 |
| `OccupancyPenalty` | 400 | 위치 점유 1개당 패널티 |
| `SameSlotPenalty` | 100000 | 의도 점유 1개당 패널티 |

---

## Tuning Guide

| 상황 | 조정 |
|---|---|
| 적이 여전히 뭉침 | `SameSlotPenalty` 증가 또는 `NumSlots` 증가 |
| 적이 너무 멀리 퍼짐 | `EncircleRadius` 감소 |
| 슬롯 전환이 너무 잦음 | `RepathInterval` 증가 |
| 적이 동시에 움직임 | `RepathInterval` 다변화 (BP별로 다르게) |

### 적 수 기준 NumSlots 권장값

| 동시 최대 적 수 | NumSlots |
|---|---|
| ~4마리 | 6 |
| ~8마리 | 10~12 |
| ~12마리 | 16 |

---

## Example Configs (Blueprint Defaults)

### Zombie (Melee, 무지성 돌진)
- `MovementTactic` = Encircle
- `EncircleRadius` = 220
- `RepathInterval` = 0.25~0.35
- `NumSlots` = 8
- `Jitter` = 20

### Hostile Survivor (Melee, 협공형)
- `MovementTactic` = Encircle
- `EncircleRadius` = 320~450
- `RepathInterval` = 0.4~0.6
- `NumSlots` = 12
- `Jitter` = 30

### Hostile Survivor (Ranged, 포지션 유지)
- `MovementTactic` = Encircle
- `EncircleRadius` = 700~950
- `RepathInterval` = 0.5~0.8
- `NumSlots` = 12
- `Jitter` = 40

---

## Debug (비 Shipping)

| 표시 | 의미 |
|---|---|
| 회색 점 (6px) | 후보 슬롯 전체 (매 RepathInterval) |
| 초록 구체 (r=24) | 선택된 Goal |
