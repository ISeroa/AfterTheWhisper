# Player HUD

## Overview
플레이어 체력을 화면 고정형 Progress Bar로 표시하는 HUD 시스템이다.
Health Component의 상태 변경을 Delegate로 전달받아 Tick 없이 갱신한다.

## Key Decisions
- `UTDHealthComponent`는 UI를 모르고 `OnHealthChanged` Delegate만 Broadcast한다.
- `ATDPlayerCharacter`가 Health Delegate를 받아 HUD에 값을 전달한다.
- Widget 생성과 초기 값 동기화는 `ATDPlayerCharacter::BeginPlay()`에서 수행한다.
- 체력 HUD는 화면 고정형이며 마우스를 따라가는 Ammo와 Reload Widget 위치 제어와 분리한다.
- Blueprint는 Progress Bar 표현을 구성하고 C++이 생성과 데이터 흐름을 확정한다.

## Architecture
- `UTDHealthComponent`는 Current Health와 Max Health를 관리한다.
- Damage 처리 후 `OnHealthChanged(NewHealth, Delta)`를 Broadcast한다.
- `ATDPlayerCharacter::HandleHealthChanged()`는 `UTDPlayerStatusHUD::SetHealth()`를 호출한다.
- `SetHealth()`는 Current와 Max의 비율을 Progress Bar Percent로 적용한다.
- Widget 생성 후 현재 Health를 한 번 전달해 초기 표시를 맞춘다.
- 흐름은 다음과 같다.

```text
Damage
  → UTDHealthComponent
  → OnHealthChanged(NewHealth, Delta)
  → ATDPlayerCharacter::HandleHealthChanged()
  → UTDPlayerStatusHUD::SetHealth(Current, Max)
```

## Trade-offs
- Character가 Health와 HUD 사이의 중계자가 되어 연결은 명확하지만 UI 종류가 늘면 바인딩 코드가 많아질 수 있다.
- 현재는 단순 Percent 표시만 제공하여 정확한 수치나 상태 변화 연출이 없다.
- 회복도 같은 Delegate를 재사용할 수 있지만 현재 회복 Gameplay는 구현되어 있지 않다.

## Future
- 피격 시 Vignette 또는 붉은 Flash
- 체력 숫자와 저체력 상태 표현
- 회복 아이템 연동
- 관련 문서: [[ammo-indicator]], [[reload-indicator]]
