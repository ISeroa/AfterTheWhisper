# Crosshair System

## Overview
플레이어의 마우스 조준 위치를 기준으로 가운데가 비어 있는 십자형 Crosshair를 표시한다.
1차 목표는 무기 기본 `SpreadDeg`에 따라 선분 간격이 달라지는 마우스 추적형 UI를 만드는 것이다.
정밀 조준 입력, 이동/연사 기반 동적 Spread, Hit Marker 통합은 후속 작업으로 분리한다.

## Key Decisions
- Crosshair 기본 형태는 가운데가 비어 있는 십자형으로 한다.
- 상/하/좌/우 4개의 선분으로 구성하고, 중앙 AimPoint는 가리지 않는다.
- 기본 상태는 반투명 흰색으로 표시한다.
- 조준 상태에서는 완전한 흰색으로 전환할 수 있도록 `bIsAiming` 입력 자리는 열어둔다.
- 1차 구현에서는 `bIsAiming`을 실제 RMB 정밀 조준 입력과 연결하지 않는다.
- `ATDWeaponBase`는 UI를 직접 알지 않고, Spread 값만 getter로 제공한다.
- Crosshair 위치 제어는 Widget 내부 Tick이 아니라 `ATDPlayerController::PlayerTick()`에서 처리한다.
- Widget 생성은 기존 Ammo / Reload / HitMarker UI처럼 `ATDPlayerCharacter::BeginPlay()`에서 처리한다.
- 실제 선분 배치, 색상, 애니메이션은 Blueprint Widget에서 처리한다.

## Architecture
- `ATDWeaponBase`
  - `GetSpreadDeg()`로 현재 무기의 기본 Spread 값을 제공한다.
  - Crosshair Widget을 직접 참조하지 않는다.
- `ATDPlayerCharacter`
  - `CrosshairWidgetClass`와 `CrosshairWidget`을 가진다.
  - `BeginPlay()`에서 Crosshair Widget을 생성하고 PlayerController에 전달한다.
  - 현재 무기의 Spread 값을 Crosshair Widget에 전달한다.
- `ATDPlayerController`
  - `SetCrosshairWidget()`으로 Widget 참조를 받는다.
  - `PlayerTick()`에서 마우스 화면 좌표 기준으로 Crosshair 위치를 갱신한다.
  - Crosshair 기본 Offset은 `(0, 0)`으로 둔다.
- `UTDCrosshairWidget`
  - C++에서는 값 전달용 함수와 Blueprint 이벤트만 제공한다.
  - 예시 이벤트: `BP_UpdateCrosshair(float SpreadDeg, bool bIsAiming)`
- `WBP_Crosshair`
  - Crosshair Base 선분 4개를 배치한다.
  - Spread 값에 따라 선분 간격을 조절한다.
  - 추후 Hit Marker Layer를 내부에 겹칠 수 있는 구조를 고려한다.

```text
ATDWeaponBase
  └─ GetSpreadDeg()

ATDPlayerCharacter
  ├─ CrosshairWidgetClass
  ├─ CrosshairWidget
  └─ Weapon Spread 전달

ATDPlayerController
  ├─ SetCrosshairWidget()
  └─ Mouse Position 기준 위치 갱신

UTDCrosshairWidget
  └─ BP_UpdateCrosshair(float SpreadDeg, bool bIsAiming)

WBP_Crosshair
  ├─ Top Line
  ├─ Bottom Line
  ├─ Left Line
  ├─ Right Line
  └─ Future: Hit Marker Layer
```

## Trade-offs
- 십자형 Crosshair는 `SpreadDeg`에 따른 명중률 피드백을 직관적으로 보여주기 좋다.
- 가운데를 비우면 조준 지점과 작은 대상을 덜 가리지만, 매우 밝은 배경에서는 선분이 약하게 보일 수 있다.
- 현재 `SpreadDeg`는 무기 기본 퍼짐 값이므로 1차 구현의 벌어짐은 순간 명중률이 아니라 무기 기본 정확도 표시에 가깝다.
- 이동, 연사, 조준 상태까지 반영하려면 추후 `CurrentSpread` 또는 Aim Modifier System에서 계산된 값을 별도로 제공해야 한다.
- Hit Marker를 당장 통합하지 않으면 UI 위젯이 잠시 분리되어 남지만, 1차 검증 범위를 작게 유지할 수 있다.

## Future
- RMB 기반 정밀 조준 상태와 `bIsAiming` 연결
- 조준 중 Crosshair 알파를 1.0 흰색으로 전환
- 이동/연사/상태 기반 `CurrentSpread` 계산
- Aim Modifier System과 Spread 보정 연동
- Hit Marker를 `WBP_Crosshair` 내부 Layer로 통합
- 무기 종류별 Crosshair 크기, 선 길이, 반응 속도 조절
- 관련 문서: [[aim-modifier-system]], [[hit-reaction-and-hit-marker-system]], [[weapon-system]]
