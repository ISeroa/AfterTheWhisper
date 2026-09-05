# Interaction System

## Overview
플레이어가 아이템, 컨테이너, Door, Switch 같은 월드 Actor와 하나의 입력 흐름으로 상호작용하는 시스템이다.
공통 Interactable Interface를 통해 Player가 구체 타입을 알지 않도록 구현되어 있으며, 현재는 기존 Item Pickup만 이 구조로 연결된 상태다.

## Current State
- `ITDInteractableInterface`(`Source/TopdownShooter/Public/Interaction/TDInteractableInterface.h`)를 추가했다. `Interact(ATDPlayerCharacter* Interactor)`는 `BlueprintNativeEvent`로, C++/Blueprint 양쪽에서 구현 가능하다.
- `ATDPlayerCharacter`는 더 이상 `ATDItemPickupActor`를 알지 않는다. `FocusedPickupActor` 대신 `AActor* FocusedInteractableActor`를 보관하고, `GetFocusedInteractableActor()` / `SetFocusedInteractableActor()`로 조회·설정한다.
- `OnInteractPressed()`는 `FocusedInteractableActor`가 Interface를 구현했는지(`Implements<UTDInteractableInterface>()`) 확인한 뒤 `ITDInteractableInterface::Execute_Interact()`만 호출한다. 구체 타입으로 Cast하지 않는다.
- `ATDItemPickupActor`가 `ITDInteractableInterface`를 구현한다. `Interact_Implementation()`은 기존 `TryPickup(Interactor)`를 그대로 호출하고 결과를 반환한다. `AddItem`/로그/성공 시 Destroy 동작은 변경되지 않았다.
- Pickup의 `InteractionSphere` BeginOverlap/EndOverlap은 `FocusedInteractableActor`를 등록/해제한다. EndOverlap은 현재 Focus 대상이 자기 자신일 때만 해제한다.
- `ATDExtractionZone`(`Source/TopdownShooter/Public/Interaction/TDExtractionZone.h`)이 `ITDInteractableInterface`를 구현한다. Focus 등록/해제 규칙은 `ATDItemPickupActor`와 동일하다.
- `Interact_Implementation()`은 `RequiredItem`을 `InventoryComponent::HasItem()`으로 검사해 보유 여부를 반환한다. 현재는 성공·실패 로그(`[Extraction] Success/Required item missing`)만 출력하며, 승리 처리·레벨 이동·아이템 제거·UI는 후속 작업이다.

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
- Extraction 성공에 따른 승리/패배 조건 및 레벨 처리
- Door / Switch 등 다른 Interactable 구현체
- OfficeKey 등 특정 아이템 보유 여부 검사
- 상호작용 성공/실패 Delegate
- Interactable Highlight
- 후보 우선순위
- Hold Interaction
- 상호작용 Animation
- 관련 문서: [[inventory-loot-system]], [[aim-modifier-system]]
