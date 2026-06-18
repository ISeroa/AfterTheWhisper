# Reload Indicator

## Overview
무기의 Reload 시작과 종료 상태를 마우스 주변의 진행 UI로 표시하는 시스템이다.
Weapon Delegate로 상태를 갱신하고 Reload Duration을 Blueprint Animation 속도에 전달한다.

## Key Decisions
- `ATDWeaponBase`는 Widget을 모르고 Reload UI Delegate만 Broadcast한다.
- `UTDReloadBarWidget`은 `BindWeapon()`으로 Weapon을 참조하고 Delegate를 구독한다.
- Widget 생성과 바인딩은 `ATDPlayerCharacter::BeginPlay()`에서 수행한다.
- Widget 위치는 `ATDPlayerController::PlayerTick()`이 마우스 좌표와 `ReloadWidgetOffset`으로 갱신한다.
- Reload 시작 시 Duration을 전달하고 Blueprint가 표시와 Animation 동기화를 담당한다.
- Reload 완료와 취소는 모두 `OnReloadUIStop`으로 UI를 숨긴다.

## Architecture
- `ATDWeaponBase::BeginReloadUI()`는 표시 조건을 만족하면 `OnReloadUIStart(Duration)`을 Broadcast한다.
- `UTDReloadBarWidget::HandleReloadUIStart()`는 `BP_ShowReloadBar(Duration)`을 호출한다.
- Reload 완료 또는 취소 시 `EndReloadUI()`가 `OnReloadUIStop`을 Broadcast한다.
- Widget은 `BP_HideReloadBar()`를 호출하고 진행 표시를 종료한다.
- Player Controller는 Ammo Widget과 별도의 Offset을 사용해 Reload Widget 위치를 갱신한다.
- 흐름은 다음과 같다.

```text
Reload Start
  → OnReloadUIStart(Duration)
  → BP_ShowReloadBar(Duration)

Reload Finish / Cancel
  → OnReloadUIStop
  → BP_HideReloadBar()
```

## Trade-offs
- Duration 기반 Animation은 구현이 단순하지만 실제 Montage 구간과 시간이 다르면 시각적 진행도가 어긋날 수 있다.
- Widget이 Weapon을 직접 구독하므로 Weapon 교체 시 안전한 재바인딩과 이전 Delegate 해제가 필요하다.
- 위치는 매 프레임 갱신하지만 Reload 상태 자체는 이벤트 기반이다.
- 현재 `BindWeapon()`은 기존 Weapon Delegate를 명시적으로 해제하지 않아 재바인딩 시 중복 호출 가능성을 확인해야 한다.

## Future
- Weapon 재바인딩 시 이전 Delegate 해제
- Montage Notify State 기반 완료·취소 동기화
- 피격이나 다른 행동에 의한 Reload 인터럽트
- Fade와 완료 피드백
- 관련 문서: [[weapon-system]], [[ammo-indicator]], [[player-hud]]
