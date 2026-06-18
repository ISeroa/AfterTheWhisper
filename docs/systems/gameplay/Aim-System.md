# Aim System

## Overview
마우스 위치를 월드 좌표로 변환하여 플레이어의 조준 지점을 계산하고, 캐릭터와 무기가 같은 목표를 바라보도록 유지하는 시스템이다.
현재 구현은 정밀 조준 상태가 아니라 안정적인 AimPoint 계산, 회전 보간, Weapon AimTarget 전달을 담당한다.

## Key Decisions
- 마우스 화면 좌표는 `DeprojectMousePositionToWorld()`로 월드 Ray로 변환한다.
- 기본 AimPoint는 `AimTraceChannel` 기반 LineTrace의 충돌 지점을 사용한다.
- Trace가 실패하면 `bFallbackToAimPlane` 설정에 따라 플레이어 높이의 평면과 Ray가 만나는 지점을 사용한다.
- 평면 투영 시 `WorldDir.Z`가 `MinAbsDirZForPlane`보다 작으면 불안정한 계산을 피하기 위해 실패 처리한다.
- AimPoint는 플레이어 기준 2D 거리로 검사하며, `AimMinDistance`보다 가까운 위치는 사용하지 않는다.
- `AimMaxDistance`를 초과하는 지점은 최대 거리로 제한한다.
- 작은 마우스 흔들림은 `AimPointDeadZone`으로 제거하고, 유효한 AimPoint를 얻지 못하면 마지막 값을 유지한다.
- 캐릭터 회전은 `TurnSpeedDegPerSec`를 기준으로 프레임당 최대 회전량을 제한한다.
- 계산된 `SmoothedAimPoint`는 캐릭터 회전과 `ATDWeaponBase::SetAimTarget()`에 함께 사용한다.
- RMB 기반 정밀 조준 효과는 별도의 Aim Modifier System으로 분리한다.

## Architecture
- `ATDPlayerCharacter::Tick()`에서 현재 조준 지점을 갱신한다.
- `GetMouseAimPointRaw()`은 다음 순서로 원시 AimPoint를 계산한다.
  1. 마우스 위치를 월드 Ray로 변환한다.
  2. `AimTraceDistance`까지 LineTrace를 수행한다.
  3. 충돌 지점 또는 Aim Plane fallback 지점을 선택한다.
  4. `AimMinDistance`와 `AimMaxDistance`를 적용한다.
- 기존 AimPoint와 새 AimPoint의 2D 거리가 Dead Zone보다 작으면 `LastAimPoint`를 유지한다.
- 최초 유효 지점은 `LastAimPoint`와 `SmoothedAimPoint`에 즉시 저장한다.
- 이후 AimPoint는 보간된 뒤 `UpdateAimRotationFromPoint()`에 전달된다.
- `UpdateAimRotationFromPoint()`는 Z축을 제외한 방향의 목표 Yaw를 구하고, 프레임당 회전량을 제한하여 Actor Rotation을 갱신한다.
- `CurrentWeapon`은 `SmoothedAimPoint`를 받아 발사 방향 계산에 사용한다.
- `bDebugAimTrace`가 활성화되면 Trace, 충돌 지점, Forward 방향과 로그를 표시하며 Shipping 빌드에서는 제외한다.
- 시스템 흐름은 다음과 같다.

```text
Mouse Position
  → Deproject to World Ray
  → LineTrace 또는 Aim Plane fallback
  → 거리 제한 및 Dead Zone
  → SmoothedAimPoint
      ├─ Player Rotation
      └─ Weapon AimTarget
```

## Trade-offs
- Aim 계산을 Tick에서 수행하므로 마우스 움직임에 즉시 반응하지만 매 프레임 LineTrace 비용이 발생한다.
- 마지막 유효 AimPoint를 유지하면 일시적인 Trace 실패에도 회전이 안정적이지만, 실패가 지속되면 오래된 지점을 계속 바라볼 수 있다.
- Aim Plane fallback은 빈 공간 조준을 가능하게 하지만 카메라 Ray가 평면과 거의 평행하면 사용할 수 없다.
- 거리 제한은 비정상적으로 멀거나 가까운 AimPoint를 막지만 화면 가장자리에서 조준 지점이 커서 위치와 다르게 제한될 수 있다.
- 현재 보간은 캐릭터 회전의 안정성을 우선하므로 빠른 마우스 이동에서 약간의 지연감이 생길 수 있다.

## Future
- Aim Modifier System을 통한 RMB 정밀 조준 상태
- Aim 상태에 따른 Weapon Spread 보정
- Aim 상태에 따른 이동 속도와 카메라 오프셋 변경
- 무기별 Aim 파라미터 적용
- 관련 문서: [[aim-modifier-system]], [[weapon-system]], [[vision-system]]
