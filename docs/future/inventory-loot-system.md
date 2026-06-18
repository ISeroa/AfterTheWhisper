# Inventory and Loot System

## Overview
플레이어가 상자, 적 드롭, 바닥 아이템에서 전리품을 획득하고 보관하는 시스템이다.
현재는 구현 전 설계이며, DataAsset 기반 아이템 데이터와 공통 Inventory Component를 중심으로 구성한다.

## Key Decisions
- 아이템 정적 데이터는 Item DataAsset으로 관리한다.
- Player, Chest, Loot Bag은 동일한 Inventory Component를 재사용한다.
- Loot는 컨테이너를 처음 열 때 생성하여 맵 로드 시 불필요한 계산을 피한다.
- Loot Table은 Roll 수와 `SpawnWeight` 기반 가중치 랜덤을 사용한다.
- `StackMax > 1`인 아이템만 Stack을 허용한다.
- 초기 무게 시스템은 단순한 Weight Class를 사용한다.
- Loot Box, Enemy Drop, Pickup을 하나의 아이템 전달 흐름으로 연결한다.

## Architecture
- Item DataAsset은 `ItemID`, Icon, `StackMax`, Weight Class를 제공한다.
- Inventory Component는 아이템 추가·제거, Stack 처리, 총 무게와 수용 가능 여부를 관리한다.
- Loot Table DataAsset은 `RollsMin`, `RollsMax`, Loot Entry 목록을 가진다.
- Loot Entry는 Item, `SpawnWeight`, `MinCount`, `MaxCount`로 구성한다.
- Chest Actor는 첫 상호작용 시 Loot Table을 굴리고 결과를 자신의 Inventory에 저장한다.
- Loot Bag Actor는 Enemy Drop 결과를 저장하고 Chest와 동일한 조회·이동 흐름을 사용한다.
- Pickup Actor는 단일 아이템을 표현하고 상호작용 성공 시 Player Inventory로 전달한다.
- 아이템 전달은 수량과 무게 제한을 확인한 뒤 성공한 수량만 원본 Inventory에서 제거한다.
- 예상 생성 흐름은 다음과 같다.

```text
Loot Source Open
  → RollCount 결정
  → SpawnWeight 기반 Item 선택
  → Count 결정
  → Source Inventory에 추가
  → Interaction을 통해 Player Inventory로 이동
```

## Trade-offs
- 공통 Inventory Component는 재사용성이 높지만 Player와 Container의 서로 다른 규칙을 옵션으로 분기해야 할 수 있다.
- Weight Class는 구현과 튜닝이 단순하지만 세밀한 무게 밸런싱에는 한계가 있다.
- 처음 열 때 생성하면 초기 비용은 줄지만 저장·불러오기 시 생성 여부와 결과를 보존해야 한다.
- 단일 Loot Table 구조는 초기 규모에는 충분하지만 지역과 난이도별 변형이 늘면 관리가 복잡해질 수 있다.

## Future
- 지역·컨테이너 타입별 Loot Table
- 희귀도와 아이템 등급
- float 기반 Item Weight
- Loot Bag 자동 제거 및 저장 규칙
- 관련 문서: [[interaction-system]], [[death-system]], [[extraction-loop]]
