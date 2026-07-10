# Inventory Weight System

## Overview
플레이어 인벤토리에 들어간 아이템의 총 무게를 계산하고, 그 무게에 따라 플레이어의 걷기/달리기 이동속도에 배율을 적용하는 시스템이다.

현재 구현 범위는 "슬롯 기반 인벤토리 + 총 무게 계산 + 이동속도 배율 반영"까지다. 실제 루팅 액터, 상자, 인벤토리 UI, 아이템 사용 효과는 아직 포함하지 않는다.

## Key Decisions
- 인벤토리는 격자형 배치가 아니라 슬롯 기반으로 시작한다.
- 모든 아이템은 기본적으로 슬롯 1칸을 차지한다.
- `StackMax > 1`인 아이템만 같은 슬롯에 수량을 합칠 수 있다.
- 아이템의 정적 데이터는 `UTDItemDataAsset`에서 관리한다.
- 아이템 보관 상태와 총 무게 계산은 `UTDInventoryComponent`가 담당한다.
- 플레이어 이동속도 변경은 `ATDPlayerCharacter::UpdateMoveSpeed()`를 통해서만 반영한다.
- `WeightSpeedMultiplier`는 직접 조정하는 값이 아니라 총 무게에서 계산된 결과값이다.
- 실제 밸런싱 값은 `NoPenaltyWeight`, `MaxPenaltyWeight`, `MinWeightSpeedMultiplier`로 조정한다.

## Related Files
- `Source/TopdownShooter/Public/Inventory/Data/TDItemDataAsset.h`
- `Source/TopdownShooter/Private/Inventory/Data/TDItemDataAsset.cpp`
- `Source/TopdownShooter/Public/Inventory/TDInventoryComponent.h`
- `Source/TopdownShooter/Private/Inventory/TDInventoryComponent.cpp`
- `Source/TopdownShooter/Public/Character/TDPlayerCharacter.h`
- `Source/TopdownShooter/Private/Character/TDPlayerCharacter.cpp`
- `Config/DefaultInput.ini`

## Architecture

### Item Data
`UTDItemDataAsset`는 인벤토리에 들어가는 아이템의 기본 데이터를 제공한다.

```text
UTDItemDataAsset
  - ItemID
  - DisplayName
  - Weight
  - StackMax
```

현재 DataAsset은 인벤토리/무게 계산에 필요한 최소 정보만 가진다. 아이콘, 월드 메시, 사용 효과, 장비 타입 등은 후속 작업에서 확장한다.

권장 콘텐츠 폴더:

```text
Content/Game/Items/Data
```

### Inventory Component
`UTDInventoryComponent`는 아이템 슬롯 목록을 가지고, 아이템 추가/제거와 총 무게 계산을 담당한다.

```text
UTDInventoryComponent
  - SlotCapacity
  - Slots
  - CanAddItem()
  - AddItem()
  - RemoveItem()
  - GetTotalWeight()
  - GetUsedSlotCount()
  - OnInventoryChanged
  - OnInventoryWeightChanged
```

`AddItem()` 또는 `RemoveItem()`이 성공하면 `BroadcastInventoryChanged()`를 통해 다음 이벤트를 발생시킨다.

```text
OnInventoryChanged
OnInventoryWeightChanged(GetTotalWeight())
```

총 무게 계산은 각 슬롯의 `Item->Weight * Count`를 더하는 방식이다.

### Player Speed Link
`ATDPlayerCharacter`는 `InventoryComponent`의 `OnInventoryWeightChanged` 이벤트를 구독한다. 이벤트를 받으면 총 무게를 기반으로 `WeightSpeedMultiplier`를 계산하고, `UpdateMoveSpeed()`를 호출한다.

```text
Inventory Add/Remove
  -> OnInventoryWeightChanged(TotalWeight)
  -> HandleInventoryWeightChanged(TotalWeight)
  -> CalculateWeightSpeedMultiplier(TotalWeight)
  -> UpdateMoveSpeed()
  -> CharacterMovement.MaxWalkSpeed 갱신
```

`UpdateMoveSpeed()`는 달리기 상태와 무게 배율을 함께 적용한다.

```text
BaseSpeed = bWantsToSprint ? SprintSpeed : WalkSpeed
MaxWalkSpeed = BaseSpeed * WeightSpeedMultiplier
```

따라서 무게 페널티는 걷기와 달리기 모두에 같은 비율로 적용된다.

## Weight Formula
현재 무게 배율은 단순 구간 보간 방식이다.

```text
TotalWeight <= NoPenaltyWeight
  -> 1.0

TotalWeight >= MaxPenaltyWeight
  -> MinWeightSpeedMultiplier

그 사이
  -> 1.0에서 MinWeightSpeedMultiplier까지 선형 보간
```

예시 기본값:

```text
NoPenaltyWeight = 20
MaxPenaltyWeight = 40
MinWeightSpeedMultiplier = 0.7
```

주의할 점:
- `WeightSpeedMultiplier`를 BP에서 직접 조정하는 값으로 취급하지 않는다.
- 테스트에서 배율을 크게 낮추고 싶다면 `MinWeightSpeedMultiplier`를 조정해야 한다.
- 시작 시점에 `TestInventoryItem`만 지정해도 자동으로 인벤토리에 들어가지는 않는다.
- 테스트 아이템은 현재 `TestAddInventoryItem` 입력으로 `AddItem()`이 호출되어야 실제 `Slots`에 들어간다.

## Debug
테스트용 입력:

```text
TestAddInventoryItem = O
```

`ATDPlayerCharacter::TestInventoryItem`에 아이템 DataAsset을 지정한 뒤, 플레이 중 `O` 키를 누르면 해당 아이템 1개가 인벤토리에 추가된다.

이동속도 디버그는 `bDebugMoveSpeed`로 확인한다.

출력 예:

```text
[MoveSpeed] Vel=123.4 Max=210.0 Walk=300 Sprint=600 WeightMul=0.70 Sprinting=0
```

무게 이벤트 로그 예:

```text
[InventoryWeight] Total=50.0 NoPenalty=20.0 MaxPenalty=40.0 MinMul=0.70 CurrentMul=0.70 MaxWalkSpeed=210.0
```

이 로그에서 `Total=0.0`이면 아이템 DataAsset을 지정만 했고 실제 인벤토리에 추가하지 않은 상태일 수 있다.

## Trade-offs
- 슬롯 기반 인벤토리는 구현과 테스트가 단순하지만, 아이템 크기/부피를 공간 퍼즐처럼 표현하지는 못한다.
- 무게 배율을 단순 선형 보간으로 처리해서 밸런싱이 쉽지만, 특정 무게 구간의 체감 조절은 아직 제한적이다.
- 테스트 입력으로 아이템을 추가하는 구조라 실제 루팅 경험과는 분리되어 있다.
- `UTDInventoryComponent`는 재사용 가능한 구조지만, 플레이어와 상자/루팅 가방의 규칙이 달라지면 옵션 분기가 필요할 수 있다.

## Future
- 실제 Pickup Actor와 상호작용 시스템 연결
- Chest/Loot Bag 같은 컨테이너 인벤토리
- 인벤토리 UI와 현재 무게 표시
- 시작 인벤토리 또는 디버그 초기 지급 옵션
- 아이템 아이콘, 설명, 카테고리, 사용 효과
- 무게에 따른 HUD 경고 또는 과적 상태
- 달리기 스태미나/소음 시스템과 무게 페널티 연동
- 관련 문서: `docs/future/inventory-loot-system.md`
