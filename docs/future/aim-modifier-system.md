# Aim Modifier System

## Overview
RMB 입력으로 정밀 조준 상태에 진입하여 사격 정확도와 전방 확인 범위를 개선하는 대신 이동 성능에 페널티를 적용하는 시스템이다.
기존 Aim System의 조준 지점 계산은 유지하고, 정밀 조준 상태가 다른 시스템에 제공하는 Modifier만 별도로 관리한다.

## Key Decisions
- LMB 발사는 Aim 상태와 관계없이 허용한다.
- 초기 입력 방식은 RMB Hold를 사용한다.
- Aim 상태는 발사 가능 여부를 변경하지 않고 Weapon Spread, 이동 속도, 카메라에만 영향을 준다.
- Aim 상태의 소유자는 `ATDPlayerCharacter`로 두고 다른 시스템은 상태를 읽거나 필요한 Modifier만 전달받는다.
- 기존 Aim System은 마우스 기반 AimPoint 계산과 회전을 계속 담당한다.
- 무기별 Aim 수치는 `UTDWeaponPresetDA`의 `Stats` 구조 안에서 관리하는 방향을 우선 검토한다.
- Camera, Weapon, Movement 연동은 각각 게임에서 확인 가능한 작은 작업 단위로 나누어 구현한다.

## Architecture
- 입력 시작 시 `bIsAiming`을 활성화하고 입력 종료 시 비활성화한다.
- Weapon System은 Aim 상태에 따라 기본 `SpreadDeg`에 Aim Spread Modifier를 적용한다.
- Movement System은 기본 이동 속도에 Aim Movement Modifier를 적용한다.
- Camera System은 Aim 상태에서 조준 방향 오프셋 또는 줌 값을 변경한다.
- 여러 이동 속도 Modifier가 생길 경우 최종 속도 계산 책임을 한 곳으로 모아 직접 값 덮어쓰기를 피한다.
- 상태가 변경될 때만 필요한 시스템을 갱신하며 매 프레임 폴링하지 않는다.
- 예상 흐름은 다음과 같다.

```text
RMB Press / Release
  → Player Aim State 변경
      ├─ Weapon Spread Modifier
      ├─ Movement Speed Modifier
      └─ Camera Offset Modifier

Existing Aim System
  → AimPoint 계산과 캐릭터 회전 유지
```

## Trade-offs
- 정확도 보상과 이동 페널티가 강하면 정지 사격만 지나치게 유리해질 수 있다.
- Camera, Weapon, Movement를 동시에 연결하면 기능 범위와 테스트 조합이 빠르게 커진다.
- PlayerCharacter가 모든 Modifier 값을 직접 계산하면 결합도가 높아질 수 있지만, 초기 구현에서 별도 Manager를 추가하는 것도 과한 추상화가 될 수 있다.
- 무기별 수치를 DataAsset에 추가하면 튜닝은 쉬워지지만 기존 `FTDWeaponStats` 변경과 모든 Preset 점검이 필요하다.

## Future
- RMB Toggle 입력 옵션
- 무기별 Aim Spread 및 이동 속도 Modifier
- 카메라 줌과 조준 방향 오프셋
- 무기별 Aim 애니메이션
- Vision System과의 전방 시야 보정 연동 여부 검토
- 관련 문서: [[aim-system]], [[weapon-system]], [[vision-system]]
