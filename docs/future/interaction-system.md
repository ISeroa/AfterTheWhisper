# Interaction System

## Overview
플레이어가 아이템, 컨테이너, Door, Switch 같은 월드 Actor와 하나의 입력 흐름으로 상호작용하는 시스템이다.
현재는 구현 전 설계이며, Player가 구체 타입을 알지 않도록 공통 Interactable Interface를 사용한다.

## Key Decisions
- 초기 상호작용 입력은 E 키 하나로 통합한다.
- 상호작용 대상은 공통 Interactable Interface를 구현한다.
- Player Character는 대상 타입을 Cast하지 않고 Interface만 호출한다.
- 탑다운 시점에 맞춰 짧은 거리 Trace와 근거리 Overlap 방식 중 작은 범위부터 검증한다.
- Aim 상태에서도 상호작용을 허용한다.
- UI는 현재 후보가 바뀔 때만 안내 문구를 갱신한다.

## Architecture
- 입력이 발생하면 Player의 상호작용 탐지 로직이 후보 Actor를 찾는다.
- 후보가 Interactable Interface를 구현했으면 `Interact(Player)`를 호출한다.
- Pickup, Chest, Loot Bag, Door, Switch는 각자 Interface 동작을 구현한다.
- Player Character는 결과 데이터나 구체 Actor 구현을 직접 알지 않는다.
- 지속적인 후보 강조가 필요하면 낮은 빈도의 탐지 또는 후보 진입·이탈 이벤트를 사용한다.
- 기본 흐름은 다음과 같다.

```text
E Input
  → Interaction Detection
  → Interactable Interface 확인
  → Interact(Player)
      ├─ Pickup
      ├─ Chest / Loot Bag
      └─ Door / Switch
```

## Trade-offs
- 단일 Interface는 호출 구조가 단순하지만 상호작용별 추가 데이터가 늘면 별도 조회 구조가 필요할 수 있다.
- LineTrace는 의도가 명확하지만 탑다운 카메라 방향과 캐릭터 방향 중 어느 기준을 쓸지 튜닝이 필요하다.
- Overlap은 근처 대상을 찾기 쉽지만 여러 후보가 겹칠 때 우선순위 규칙이 필요하다.

## Future
- Interactable Highlight
- 후보 우선순위
- Hold Interaction
- 상호작용 Animation
- 관련 문서: [[inventory-loot-system]], [[aim-modifier-system]]
