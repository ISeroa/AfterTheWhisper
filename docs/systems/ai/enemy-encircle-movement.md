# Enemy Encircle Movement

## Overview
적이 플레이어 위치로 일제히 돌진하지 않고, 플레이어 주변의 고정 슬롯을 목표로 선택하여 자연스럽게 분산되도록 하는 이동 전술이다.
이 문서는 공격과 상태 판단을 제외하고 Encircle 목표 위치 계산만 다룬다.

## Key Decisions
- 슬롯 방향은 플레이어 회전과 무관한 World Forward와 World Right를 기준으로 계산한다.
- 각 적은 거리 비용, 실제 점유 비용, 예약 슬롯 비용을 합산해 가장 낮은 비용의 슬롯을 선택한다.
- `CurrentSlotIndex`를 다른 Controller가 조회하여 동일 슬롯 예약을 피한다.
- `RadiusBias`는 Possess 시 한 번만 정해 적들이 같은 반경에 완전히 겹치지 않도록 한다.
- 첫 Timer 실행 시점은 `0 ~ RepathInterval` 범위로 분산한다.
- 후보 슬롯과 최종 목표 디버그 표시는 Shipping 빌드에서 제외한다.

## Architecture
- 슬롯 위치는 다음 계산을 사용한다.

```text
Angle   = SlotIndex * (2π / NumSlots)
Dir     = WorldForward * cos(Angle) + WorldRight * sin(Angle)
SlotPos = PlayerLocation + Dir * (EncircleRadius + RadiusBias)
```

- 각 슬롯의 비용은 다음 요소로 구성한다.

```text
Cost = Dist2D(SelfLocation, SlotPos)
     + OccupancyPenalty * 슬롯 주변 다른 Pawn 수
     + SameSlotPenalty  * 같은 슬롯을 예약한 다른 Controller 수
```

- `OccupancyRadius` 안에 실제 Pawn이 있으면 `OccupancyPenalty`를 추가한다.
- 다른 `ATDEnemyAIController`가 동일한 `CurrentSlotIndex`를 가지면 `SameSlotPenalty`를 추가한다.
- 최소 비용 슬롯을 선택한 뒤 `CurrentSlotIndex`와 이동 목표를 갱신한다.
- `OnUnPossess()`에서 `CurrentSlotIndex`를 `INDEX_NONE`으로 초기화하여 예약을 해제한다.
- 현재 기본 튜닝 값은 `RepathInterval=0.35`, `EncircleRadius=280`, `NumSlots=12`, `OccupancyRadius=160`이다.

## Trade-offs
- 모든 Pawn과 Enemy Controller를 반복 조회하므로 적 수가 늘면 슬롯 계산 비용이 빠르게 증가할 수 있다.
- 큰 `SameSlotPenalty`는 중복 예약을 강하게 막지만 이동 가능한 NavMesh와 실제 도달 가능성은 고려하지 않는다.
- World 축 고정 슬롯은 플레이어 회전에 안정적이지만 환경 구조나 엄폐 위치를 반영하지 않는다.
- 주기적인 재선택은 동적인 분산을 만들지만 값이 너무 짧으면 목표가 자주 바뀌어 움직임이 불안정해질 수 있다.

## Future
- 주변 적 목록 캐싱 또는 Coordinator 기반 슬롯 예약
- 도달 불가능한 슬롯 제외
- 근접·원거리 적 타입별 반경 프리셋
- 슬롯 유지 비용을 추가하여 과도한 목표 변경 방지
- 관련 문서: [[enemy-system]]
