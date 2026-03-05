# Player HUD System

## Overview
플레이어의 체력 상태를 화면에 표시하는 HUD 시스템.
HealthComponent의 델리게이트를 구독하여, Tick 의존 없이 이벤트 기반으로 갱신한다.
화면 고정형 UI로, 마우스를 따라다니는 Ammo/Reload 위젯과 역할이 분리된다.

## Key Decisions
- UI는 HealthComponent를 직접 참조하지 않는다.
    - `OnHealthChanged(float NewHealth, float Delta)` 델리게이트를 통해 상태만 전달받는다.
- 위젯 생성과 바인딩은 C++에서 확정한다.
    - Blueprint 초기화 순서 문제를 방지.
- 초기 체력값은 바인딩 직후 `SetHealth()`를 명시적으로 호출하여 동기화한다.
    - HealthComponent::BeginPlay()가 Actor::BeginPlay()보다 먼저 실행됨을 보장.
- ATDPlayerCharacter는 ATDBaseCharacter를 상속하여 HealthComponent를 공유한다.
    - 플레이어/적 모두 동일한 HealthComponent 구조 사용.

## Architecture
- HealthComponent는 UI를 모른다.
- UTDPlayerStatusHUD는 HealthComponent 델리게이트에 구독한다.
- UI 갱신은 Tick 기반이 아닌 이벤트 기반 구조.
- 위젯 생성/바인딩은 ATDPlayerCharacter::BeginPlay()에서 처리.
- PlayerController는 체력 HUD 위치에 관여하지 않는다 (화면 고정).

## Update Flow
```
TakeAnyDamage
→ HealthComponent::HandleTakeAnyDamage()
→ OnHealthChanged.Broadcast(NewHealth, Delta)
→ ATDPlayerCharacter::HandleHealthChanged()
→ UTDPlayerStatusHUD::SetHealth(NewHealth, MaxHealth)
→ ProgressBar_Health->SetPercent()
```

## Classes
| 클래스 | 역할 |
|--------|------|
| `UTDHealthComponent` | 체력 수치 관리, 델리게이트 브로드캐스트 |
| `UTDPlayerStatusHUD` | ProgressBar 갱신, C++ 위젯 기반 클래스 |
| `ATDPlayerCharacter` | 위젯 생성 및 델리게이트 바인딩 |

## Trade-offs
- 현재 ProgressBar 단순 퍼센트 표시만 구현. 숫자/색상 변화 연출 없음.
- 체력 회복 시스템 없음 (데미지 단방향).

## Future
- 피격 시 화면 Vignette/붉은 플래시 연동.
- 체력 회복 아이템 연동 시 `OnHealthChanged` 그대로 재사용 가능.
