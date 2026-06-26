# Weapon Audio System

## Overview
Weapon Preset에 저장된 발사, Dry Fire 사운드를 런타임 상황에 맞게 재생하는 시스템이다.
무기별 사운드 교체는 코드 변경 없이 DataAsset 설정으로 처리하고, 탄피 충돌음은 `ATDCasing`이 별도로 담당한다.

## Key Decisions
- 무기 사운드는 `FWeaponSoundSet`으로 묶어 `UTDWeaponPresetDA`에 저장한다.
- 발사음은 `ATDPlayerController::bIsIndoor` 값에 따라 Indoor와 Outdoor 슬롯을 선택한다.
- 사운드 재생 정책은 `ATDWeaponBase::PlayWeaponSfx()`에 모은다.
- Fire와 Dry Fire는 Muzzle Socket 부착 재생을 우선하고 실패 시 월드 위치 재생으로 fallback한다.
- 총알 Impact 사운드의 분류 책임은 `ATDWeaponBase`가 가진다.
- 총알이 `ATDEnemyCharacter`에 맞으면 Enemy Hit 사운드, 그 외 대상에 맞으면 World Hit 사운드를 재생한다.
- Impact 사운드는 무기 위치가 아니라 `FHitResult::ImpactPoint`에서 월드 위치 사운드로 재생한다.
- 탄피 바닥 충돌음은 Weapon이 아니라 `ATDCasing::OnHit()`에서 재생한다.
- 사운드는 발사 시도 또는 성공 이벤트 시점에만 재생하고 Tick으로 상태를 감시하지 않는다.

## Architecture
- `UTDWeaponPresetDA::SoundSet`은 `FireIndoor`, `FireOutdoor`, `DryFire`, `CasingDrop`, `EnemyHit`, `WorldHit` 슬롯을 가진다.
- 발사 성공 시 Controller의 Indoor 상태를 조회하여 발사음을 선택한다.
- 탄약이 없는 발사 시도는 Dry Fire를 재생한다.
- `PlayWeaponSfx()`는 Muzzle 제공 컴포넌트와 Socket 존재 여부를 확인한 뒤 재생 위치를 결정한다.
- `PlayImpactSfx(const FHitResult& Hit)`는 `Hit.GetActor()`를 기준으로 Enemy Hit / World Hit 사운드를 고르고 `Hit.ImpactPoint`에서 재생한다.
- 현재 `SoundSet.CasingDrop`은 Weapon 재생 흐름에서 사용하지 않는다.
- 실제 탄피 충돌음과 재생 횟수 제한은 `ATDCasing`의 `ImpactSound`, `MaxImpactSounds`가 담당한다.

## Impact Sound Policy
- 1차 구현은 표면 재질별 사운드를 분기하지 않는다.
- 적 피격음은 "적의 반응 사운드"가 아니라 "총알이 적에게 맞은 Impact 사운드"로 취급한다.
- 적 자체의 피격 반응(플래시, 넉백, 짧은 스턴, Hit Reaction Animation)은 `ATDEnemyCharacter` 또는 별도 피격 피드백 구조에서 다룬다.
- `ApplyPointDamage()` 성공 여부와 Impact 사운드 재생은 같은 의미가 아니다. 벽이나 바닥에 맞아도 Impact 사운드는 재생될 수 있다.
- Hit Marker, UI성 Hit Confirm 사운드, 표면별 Physical Material 분기는 이번 범위에 포함하지 않는다.

## Trade-offs
- 수동 `bIsIndoor` 값은 구조 검증에는 단순하지만 Level 구역이 바뀔 때 별도 갱신이 필요하다.
- Socket 부착음은 무기 위치를 잘 따르지만 짧은 One-shot에서는 위치 재생과 체감 차이가 작을 수 있다.
- Preset의 `CasingDrop`과 Casing Actor의 `ImpactSound`가 함께 존재해 데이터 소유권이 혼동될 여지가 있다.
- Concurrency와 Attenuation 설정이 부족하면 다수 무기의 동시 발사에서 소리가 혼잡해질 수 있다.
- `ATDWeaponBase`가 Impact 분류까지 맡으므로 표면별/대상별 피드백이 복잡해지면 별도 Resolver나 인터페이스 분리를 검토한다.

## Future
- Indoor 상태 자동 판정
- Reload Start, Insert, End 사운드
- 표면 재질별 Bullet Impact
- 표면 재질별 Casing Impact
- Concurrency와 Attenuation 튜닝
- Hit Confirm UI 사운드 분리 여부 검토
- 미사용 `SoundSet.CasingDrop` 슬롯 정리 여부 결정
- 관련 문서: [[weapon-system]]
