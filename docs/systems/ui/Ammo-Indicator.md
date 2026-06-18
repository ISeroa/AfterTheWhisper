# Ammo Indicator

## Overview
현재 무기의 탄약 수와 탄창 크기를 원형 UI로 표시하고 마우스 주변에 배치하는 시스템이다.
탄약 값은 Weapon Delegate로 갱신하고 화면 위치는 Player Controller가 제어한다.

## Key Decisions
- `ATDWeaponBase`는 UI를 모르고 `OnAmmoChanged` Delegate만 Broadcast한다.
- `UTDW_AmmoWidget`은 `BindWeapon()`으로 현재 Weapon에 구독한다.
- Widget 생성과 Weapon 바인딩은 `ATDPlayerCharacter::BeginPlay()`에서 수행한다.
- 탄약 표시는 Blueprint Implementable Event인 `BP_UpdateAmmo()`에 전달한다.
- 화면 위치는 Widget Tick이 아니라 `ATDPlayerController::PlayerTick()`에서 마우스 좌표와 Offset으로 갱신한다.
- 무기가 교체되면 이전 Weapon Delegate를 해제한 뒤 새 Weapon에 바인딩한다.

## Architecture
- `ATDPlayerCharacter`는 `AmmoWidgetClass`로 Widget을 생성하고 Viewport에 추가한다.
- `UTDW_AmmoWidget::BindWeapon()`은 이전 `OnAmmoChanged` 바인딩을 제거하고 새 Weapon에 구독한다.
- 바인딩 직후 현재 탄약과 탄창 크기를 `BP_UpdateAmmo()`로 전달해 초기 상태를 동기화한다.
- 발사와 Reload로 탄약이 바뀌면 `OnAmmoChanged`가 `HandleAmmoChanged()`를 호출한다.
- `ATDPlayerController`는 보관한 Widget 참조를 `MouseOffset`만큼 이동시킨다.

```text
Ammo 변경
  → OnAmmoChanged(AmmoInMag, MagazineSize)
  → UTDW_AmmoWidget::HandleAmmoChanged()
  → BP_UpdateAmmo()

PlayerController::PlayerTick()
  → Mouse Position + MouseOffset
  → SetPositionInViewport()
```

## Trade-offs
- 마우스 추적 위치는 매 프레임 갱신이 필요하지만 탄약 데이터 갱신은 이벤트 기반으로 분리되어 있다.
- Blueprint Event가 실제 Material 표현을 담당하므로 C++만으로 최종 UI 모양을 검증할 수 없다.
- Widget에 캐시 필드가 선언되어 있지만 현재 `HandleAmmoChanged()`에서 중복 갱신 방지에 사용하지 않는다.

## Future
- 캐시 필드 적용 또는 제거
- 화면 가장자리 위치 보정
- 무기 교체 시 Widget 재바인딩 검증
- 탄약 부족 상태의 색상·Animation 피드백
- 관련 문서: [[weapon-system]], [[reload-indicator]], [[player-hud]]
