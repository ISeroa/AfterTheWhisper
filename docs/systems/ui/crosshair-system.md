# Crosshair System

## Overview
플레이어의 마우스 조준 위치를 기준으로 가운데가 비어 있는 십자형 Crosshair를 표시한다.
1차 구현은 C++에서 Widget 생성, 위치 갱신, 무기 기본 `SpreadDeg` 전달을 담당하고, 실제 선분 배치와 색상 표현은 `WBP_Crosshair` Blueprint에서 처리한다.
현재 단계의 Crosshair는 순간 탄착 퍼짐이 아니라 무기 기본 정확도를 보여주는 UI이며, 연사/이동/조준 상태에 따른 동적 확장은 후속 작업으로 둔다.

## Key Decisions
- Crosshair 기본 형태는 가운데가 비어 있는 십자형으로 한다.
- 상/하/좌/우 4개의 선분으로 구성하고, 중앙 AimPoint는 가리지 않는다.
- 별도 Material은 사용하지 않고 UMG `Border` 4개로 1차 구현한다.
- 기본 상태는 반투명 흰색으로 표시한다.
- 조준 상태에서는 완전한 흰색으로 전환할 수 있도록 `bIsAiming` 입력 자리는 열어둔다.
- 1차 구현에서는 `bIsAiming`을 실제 RMB 정밀 조준 입력과 연결하지 않는다.
- `ATDWeaponBase`는 UI를 직접 알지 않고, `GetSpreadDeg()` getter로 Spread 값만 제공한다.
- Crosshair 위치 제어는 Widget 내부 Tick이 아니라 `ATDPlayerController::PlayerTick()`에서 처리한다.
- Widget 생성은 기존 Ammo / Reload / HitMarker UI처럼 `ATDPlayerCharacter::BeginPlay()`에서 처리한다.
- Crosshair 기본 Offset은 `(0, 0)`으로 둔다.
- BP 그래프는 라인별 노드를 직접 늘어놓기보다 `UpdateLine`, `UpdateCrosshairLines` 함수로 정리하는 방향을 선택했다.
- 튜닝 변수 이름은 방향 혼동을 줄이기 위해 `SegmentLength`, `SegmentThickness`, `BaseCenterGap`, `SpreadGapScale`처럼 방향 독립적인 의미를 우선한다.

## Architecture
- `ATDWeaponBase`
  - `GetSpreadDeg()`로 현재 무기의 기본 Spread 값을 제공한다.
  - Crosshair Widget을 직접 참조하지 않는다.
- `ATDPlayerCharacter`
  - `CrosshairWidgetClass`와 `CrosshairWidget`을 가진다.
  - `BeginPlay()`에서 Crosshair Widget을 생성하고 PlayerController에 전달한다.
  - `Tick()`에서 현재 무기의 `GetSpreadDeg()` 값을 `BP_UpdateCrosshair(SpreadDeg, false)`로 전달한다.
  - Crosshair 표시 문제 확인을 위해 non-shipping 빌드에서 1초 간격 디버그 로그를 남긴다.
- `ATDPlayerController`
  - `SetCrosshairWidget()`으로 Widget 참조를 받는다.
  - `PlayerTick()`에서 마우스 화면 좌표 기준으로 Crosshair 위치를 갱신한다.
  - Crosshair 위치 갱신 문제 확인을 위해 non-shipping 빌드에서 1초 간격 디버그 로그를 남긴다.
- `UTDCrosshairWidget`
  - C++에서는 Blueprint 이벤트만 제공한다.
  - 이벤트: `BP_UpdateCrosshair(float SpreadDeg, bool bIsAiming)`
- `WBP_Crosshair`
  - 부모 클래스는 `UTDCrosshairWidget`이다.
  - `CrosshairRoot` 아래에 `TopLine`, `BottomLine`, `LeftLine`, `RightLine` Border를 둔다.
  - 각 Border의 Canvas Slot Size/Position을 변경해 선분 길이, 두께, 중앙 간격을 표현한다.
  - Brush Image Size는 사용하지 않고, 색상과 알파는 Brush Color로 제어한다.
  - Visibility는 입력을 막지 않도록 `Not Hit-Testable` 계열을 사용한다.

```text
ATDWeaponBase
  └─ GetSpreadDeg()

ATDPlayerCharacter
  ├─ CrosshairWidgetClass
  ├─ CrosshairWidget 생성
  └─ BP_UpdateCrosshair(SpreadDeg, false)

ATDPlayerController
  ├─ SetCrosshairWidget()
  └─ Mouse Position + CrosshairWidgetOffset

UTDCrosshairWidget
  └─ BP_UpdateCrosshair(float SpreadDeg, bool bIsAiming)

WBP_Crosshair
  └─ CrosshairRoot
      ├─ TopLine    : Border
      ├─ BottomLine : Border
      ├─ LeftLine   : Border
      └─ RightLine  : Border
```

### Blueprint Layout Notes
- `CrosshairRoot`는 기준 컨테이너 역할만 한다.
- 실제 크기와 위치 조정 대상은 `TopLine`, `BottomLine`, `LeftLine`, `RightLine`이다.
- 각 Line Border는 `Slot as Canvas Slot`을 통해 `Set Size`, `Set Position`을 호출한다.
- 세로 라인:
  - Size = `(SegmentThickness, SegmentLength)`
- 가로 라인:
  - Size = `(SegmentLength, SegmentThickness)`
- 위치:
  - Top = `(0, -CurrentGap)`
  - Bottom = `(0, CurrentGap)`
  - Left = `(-CurrentGap, 0)`
  - Right = `(CurrentGap, 0)`
- `CurrentGap = BaseCenterGap + SpreadDeg * SpreadGapScale`
- `Alpha = bIsAiming ? 1.0 : IdleAlpha`

## Trade-offs
- UMG Border 기반 구현은 Material 없이 빠르게 검증할 수 있고, 현재 십자형 표현에는 충분하다.
- Border 4개를 쓰면 선 끝 페이드, 글로우, 원형 마스크 같은 효과는 제한적이다.
- 현재 `SpreadDeg`는 무기 기본 퍼짐 값이므로 1차 구현의 벌어짐은 순간 명중률이 아니라 무기 기본 정확도 표시에 가깝다.
- 이동, 연사, 조준 상태까지 반영하려면 추후 `CurrentSpread` 또는 Aim Modifier System에서 계산된 값을 별도로 제공해야 한다.
- Crosshair BP 내부 튜닝 값을 과도하게 열어두지 않으면 구조가 단순하지만, 플레이 테스트 중 미세 조정은 BP를 직접 열어야 한다.
- 디버그 로그는 Crosshair 표시 문제를 추적하는 데 유용하지만, 1차 검증 후 필요 없으면 제거하거나 디버그 플래그로 묶는 편이 좋다.

## Future
- 연사 시 `CurrentSpread` 증가
- 발사 중 Crosshair 간격 확장
- 일정 시간 후 Crosshair 간격 회복
- RMB 기반 정밀 조준 상태와 `bIsAiming` 연결
- 조준 중 Crosshair 알파를 1.0 흰색으로 전환
- Aim Modifier System과 Spread 보정 연동
- Hit Marker를 `WBP_Crosshair` 내부 Layer로 통합
- 무기 종류별 Crosshair 크기, 선 길이, 반응 속도 조절
- 관련 문서: [[aim-modifier-system]], [[hit-reaction-and-hit-marker-system]], [[weapon-system]]
