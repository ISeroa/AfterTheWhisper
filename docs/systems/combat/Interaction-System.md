
# Interaction System

## Overview
Interaction System은 플레이어가 월드 오브젝트와 상호작용할 수 있도록 하는 시스템이다.

플레이어는 기본적으로 **E 키**를 통해 상호작용을 수행하며,
아이템 줍기, 컨테이너 열기, 루팅, 스위치 조작 등 다양한 게임플레이 요소를
단일 인터페이스 구조로 처리하는 것을 목표로 한다.

---

## Key Decisions

- 상호작용 입력은 **E 키 하나로 통합**한다.
- 상호작용 대상은 **Interactable 인터페이스**를 통해 처리한다.
- PlayerCharacter는 대상 타입을 직접 알지 않고 **인터페이스 호출만 수행**한다.
- 상호작용 대상 탐지는 **라인 트레이스 또는 범위 체크**로 처리한다.
- Aim 상태에서도 **상호작용(E)은 가능**하다.

---

## Architecture

Interaction 시스템 기본 구조

```
Input (E)
   |
   v
PlayerCharacter
   |
   v
Interaction Check
   |
   v
IInteractable Interface
   |
   |-- PickupActor
   |
   |-- ChestActor
   |
   |-- LootBagActor
   |
   `-- Door / Switch (future)
```

PlayerCharacter는 상호작용 대상의 구체적인 타입을 알 필요가 없다.
대신 **IInteractable 인터페이스를 호출**한다.

예시 구조

```
PlayerCharacter
   |
   v
Interact()
   |
   v
IInteractable::Interact(Player)
```

---

## Interaction Detection

상호작용 대상 탐지는 다음 방식으로 이루어진다.

```
Player Input (E)
      |
      v
Forward Trace / Overlap Check
      |
      v
Interactable Actor 발견
      |
      v
Interact() 호출
```

탐지 방식 후보

```
Line Trace (카메라 방향)
Sphere Overlap (근거리)
```

탑다운 구조에서는 **짧은 거리 LineTrace + Overlap 보조 방식**이 적합하다.

---

## Interaction Types

현재 계획된 상호작용 타입

```
Pickup Item
Open Chest
Loot Body (LootBag)
Open Door
Activate Switch
```

각 Actor는 IInteractable 인터페이스를 구현한다.

예시

```
PickupActor
ChestActor
LootBagActor
```

---

## UI Feedback

상호작용 가능한 대상이 있을 경우
플레이어에게 UI 피드백을 제공할 수 있다.

예시

```
Press E to Interact
Press E to Loot
Press E to Open
```

UI 시스템은 Interaction 대상 상태를 읽어 표시한다.

---

## Trade-offs

- 단일 인터페이스 구조는 단순하지만
  상호작용 타입이 많아질 경우 인터페이스 확장이 필요할 수 있다.

- LineTrace 기반 탐지는
  복잡한 환경에서 정확도 조정이 필요할 수 있다.

---

## Future

- Highlight 시스템 추가 (Interactable Outline)
- 상호작용 우선순위 시스템
- Hold Interaction (길게 누르기)
- Interaction Animation
