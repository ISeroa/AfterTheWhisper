# Hit Reaction and Hit Marker System

## Overview
무기 명중 시 플레이어가 즉시 피격감을 느낄 수 있도록, 적의 이동 반응과 간단한 UI Hit Marker를 제공하는 시스템이다.
리얼한 톤을 유지하기 위해 적 Mesh를 번쩍이게 하는 방식보다 무기별 저지력에 따른 둔화와 짧은 이동 스턴을 우선한다.

## Key Decisions
- 저지력은 무기별 고정 특성이므로 `UTDWeaponPresetDA`의 `Stats`에서 조정한다.
- Damage와 Stopping Power는 분리한다. 데미지가 높은 무기와 저지력이 높은 무기가 반드시 같을 필요는 없다.
- 적 피격 반응은 `ATDEnemyCharacter`가 처리한다. Weapon은 명중한 적에게 저지력 정보를 전달할 뿐, 적의 이동/스턴 구현을 직접 제어하지 않는다.
- 1차 Hit Reaction은 시각 이펙트 없이 이동속도 감소와 짧은 이동 스턴만 사용한다.
- Heavy 저지력도 AI Controller를 `UnPossess()`하지 않는다. 사망 처리와 피격 반응의 책임을 분리한다.
- Hit Marker는 적 명중 여부를 알려주는 UI 피드백으로만 사용한다. 월드 충돌이나 빗나감에는 표시하지 않는다.
- Hit Marker는 과한 연출 없이 짧은 X자 표시로 시작한다.

### Stopping Power Tier

무기 스탯에는 단계형 저지력을 둔다.

```text
None
  - 피격 반응 없음

Light
  - 짧은 이동속도 감소
  - 예: 0.15초, 기본 이동속도의 75%

Medium
  - 더 확실한 이동속도 감소
  - 예: 0.25초, 기본 이동속도의 50%

Heavy
  - 짧은 이동 스턴 후 둔화
  - 예: 0.12초 이동 정지, 이후 0.2초 동안 기본 이동속도의 50%
```

예상 무기 분류:

```text
권총 / SMG
  → Light

소총
  → Medium

샷건 / 대구경 권총 / 저격총
  → Heavy
```

## Architecture
- `FTDWeaponStats`에 `StoppingPowerTier`를 추가한다.
- `ATDWeaponBase::SetPartsFromPreset()`는 `Preset->Stats.StoppingPowerTier`를 런타임 무기 필드에 복사한다.
- `ATDWeaponBase::FireOnce()`는 LineTrace가 `ATDEnemyCharacter`를 맞췄을 때 Point Damage를 적용하고 Hit Reaction을 요청한다.
- `ATDEnemyCharacter`는 받은 Stopping Power Tier에 따라 `CharacterMovement->MaxWalkSpeed`를 임시 조정한다.
- Heavy Tier는 `StopMovementImmediately()`와 짧은 `MaxWalkSpeed = 0` 구간으로 이동 스턴을 표현한다.
- 이동속도 복구는 Timer 기반으로 처리하고 Tick을 사용하지 않는다.
- 연속 피격 시 이동속도는 중복 곱하지 않는다. 원본 속도 기준으로 다시 적용하고 Timer만 갱신한다.
- 사망 시 Hit Reaction 복구 Timer는 의미가 없으므로 정리하거나, 죽은 상태에서는 복구 처리를 무시한다.

```text
Weapon FireOnce
  ├─ LineTrace
  ├─ Enemy 명중
  │   ├─ ApplyPointDamage
  │   ├─ Enemy.ApplyHitReaction(StoppingPowerTier)
  │   └─ OnHitMarker.Broadcast()
  └─ World 명중
      └─ Impact Sound
```

### Hit Marker Flow

```text
Enemy 명중
  → Weapon OnHitMarker
  → PlayerCharacter 또는 HUD Widget이 수신
  → X자 Hit Marker 표시
  → 0.08~0.12초 후 숨김
```

탑다운 시점에서는 화면 중앙보다 조준점 또는 마우스 근처 표시가 자연스러울 수 있다.
현재 Ammo Indicator와 Reload Indicator가 마우스 주변 UI 흐름을 사용하므로, Hit Marker도 같은 위치 제어 철학을 따르는 방향을 우선 검토한다.

## Trade-offs
- 둔화와 스턴은 명중감을 자연스럽게 만들지만, 연사 무기와 결합하면 적 접근 난이도를 크게 낮출 수 있다.
- Heavy 스턴이 공격 Windup까지 끊으면 근접 적이 플레이어를 거의 공격하지 못할 수 있다. 1차 구현에서는 이동만 멈추고 공격 인터럽트는 보류한다.
- Hit Marker는 리얼한 화면 톤과 다소 거리가 있을 수 있지만, 짧은 X자 표시는 명중 정보 전달용 UI로 받아들일 수 있다.
- 저지력 Tier는 튜닝이 쉽지만 세밀한 수치형 반응보다 표현 폭이 좁다.
- `ATDWeaponBase`에서 Enemy 타입을 직접 확인하면 구현은 단순하지만, 이후 다양한 피격 대상이 늘어나면 인터페이스 기반으로 분리할 필요가 있다.

## Future
- `ITDHitReactionReceiver` 같은 인터페이스로 피격 반응 대상을 일반화한다.
- 방어구, 약점, 치명타에 따라 Hit Marker 모양이나 Stopping Power 보정값을 다르게 적용한다.
- Heavy Tier에서 제한적으로 공격 Windup 인터럽트를 지원한다.
- 무기별 Stopping Power 수치를 Tier가 아닌 Curve 또는 Data Table로 확장한다.
- 관련 문서: [[weapon-system]], [[enemy-system]], [[ammo-indicator]], [[reload-indicator]]
