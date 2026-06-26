# Weapon System

## Overview
플레이어 무기의 조립, 발사, 재장전, 탄약, 사운드와 발사 효과를 관리하는 시스템이다.
`UTDWeaponPresetDA`를 무기 설정의 단일 진실 소스로 사용하고 `ATDWeaponBase`가 런타임 동작을 수행한다.

## Key Decisions
- 무기 설정은 `UTDWeaponPresetDA`에 모으고 스탯은 항상 `Preset->Stats`를 통해 접근한다.
- Preset 적용과 파츠 조립은 `SetPartsFromPreset()` 흐름에서 함께 초기화한다.
- 파츠는 `ETDWeaponSlot`을 FName으로 변환하여 `PartComps` Map에서 관리한다.
- Muzzle 위치는 `MuzzlePrioritySlot`의 `MuzzleSocketName`을 우선 사용하고 Base Mesh를 fallback으로 사용한다.
- 발사는 Hitscan LineTrace 방식이며 `SpreadDeg`를 방향 Cone에 적용한다.
- Hitscan Impact 분류는 `ATDWeaponBase`가 담당하고, 적 대상과 월드 대상을 1차 분기한다.
- 자동 발사는 Trigger 상태와 `FireRate` 기반 Timer로 처리한다.
- 발사 성공 후 탄약과 Trace 처리를 완료한 시점에 `OnWeaponFired`를 Broadcast한다.
- UI는 Weapon의 Ammo와 Reload Delegate를 구독한다.

## Architecture
- `UTDWeaponPresetDA`는 `FTDWeaponStats`, Sound Set, Parts, Muzzle Flash, Muzzle Socket, Casing Class를 제공한다.
- `ATDWeaponBase`는 Preset 값을 런타임 필드에 적용하고 파츠 컴포넌트를 조립한다.
- `StartFire()`는 Trigger를 활성화하고 가능한 경우 즉시 발사한 뒤 반복 발사 Timer를 시작한다.
- `StopFire()`는 Trigger와 반복 Timer를 정리한다.
- `FireOnce()`는 탄약과 Reload 상태를 검사하고 Spread가 적용된 LineTrace로 Point Damage를 전달한다.
- 성공한 발사는 Muzzle Flash, Casing, 발사음, Impact 사운드, Ammo Delegate, `OnWeaponFired`를 발생시킨다.
- 탄약이 없을 때는 Dry Fire를 재생하고 Reload를 요청한다.
- Reload는 Timer 완료 또는 `NotifyReloadFinished()`를 통해 마무리할 수 있으며 취소 시 상태와 UI Delegate를 정리한다.

```text
Fire Success
  ├─ Ammo 감소 → OnAmmoChanged
  ├─ LineTrace → Impact Sound → ApplyPointDamage
  ├─ Muzzle Flash / Casing / Fire Sound
  └─ OnWeaponFired → Character Fire Montage

Reload
  ├─ OnReloadUIStart(Duration)
  ├─ Timer 또는 NotifyReloadFinished
  ├─ Ammo 보충 → OnAmmoChanged
  └─ OnReloadUIStop
```

## Trade-offs
- Hitscan은 반응성과 구현이 단순하지만 탄속과 낙차를 표현하지 않는다.
- `ATDWeaponBase`가 발사 효과와 사운드까지 조정하므로 기능이 늘면 클래스 책임이 커질 수 있다.
- Preset 값을 런타임 필드로 복사하면 조회는 단순하지만 Preset 변경 시 재적용 시점을 명확히 관리해야 한다.
- Timer 기반 자동 발사는 Fire Rate를 안정적으로 유지하지만 Animation 길이와 별도로 동기화해야 한다.

## Future
- Hit Impact와 표면 반응
- Recoil과 Camera Shake
- Burst 또는 Projectile 방식
- Reload Notify State 기반 인터럽트 정교화
- 관련 문서: [[weapon-audio-system]], [[ammo-indicator]], [[reload-indicator]], [[aim-system]]
