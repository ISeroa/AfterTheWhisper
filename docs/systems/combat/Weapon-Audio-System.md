# Weapon Audio System

## Overview
Weapon Preset에 저장된 발사, Dry Fire 사운드를 런타임 상황에 맞게 재생하는 시스템이다.
무기별 사운드 교체는 코드 변경 없이 DataAsset 설정으로 처리하고, 탄피 충돌음은 `ATDCasing`이 별도로 담당한다.

## Key Decisions
- 무기 사운드는 `FWeaponSoundSet`으로 묶어 `UTDWeaponPresetDA`에 저장한다.
- 발사음은 `ATDPlayerController::bIsIndoor` 값에 따라 Indoor와 Outdoor 슬롯을 선택한다.
- 사운드 재생 정책은 `ATDWeaponBase::PlayWeaponSfx()`에 모은다.
- Fire와 Dry Fire는 Muzzle Socket 부착 재생을 우선하고 실패 시 월드 위치 재생으로 fallback한다.
- 탄피 바닥 충돌음은 Weapon이 아니라 `ATDCasing::OnHit()`에서 재생한다.
- 사운드는 발사 시도 또는 성공 이벤트 시점에만 재생하고 Tick으로 상태를 감시하지 않는다.

## Architecture
- `UTDWeaponPresetDA::SoundSet`은 `FireIndoor`, `FireOutdoor`, `DryFire`, `CasingDrop` 슬롯을 가진다.
- 발사 성공 시 Controller의 Indoor 상태를 조회하여 발사음을 선택한다.
- 탄약이 없는 발사 시도는 Dry Fire를 재생한다.
- `PlayWeaponSfx()`는 Muzzle 제공 컴포넌트와 Socket 존재 여부를 확인한 뒤 재생 위치를 결정한다.
- 현재 `SoundSet.CasingDrop`은 Weapon 재생 흐름에서 사용하지 않는다.
- 실제 탄피 충돌음과 재생 횟수 제한은 `ATDCasing`의 `ImpactSound`, `MaxImpactSounds`가 담당한다.

## Trade-offs
- 수동 `bIsIndoor` 값은 구조 검증에는 단순하지만 Level 구역이 바뀔 때 별도 갱신이 필요하다.
- Socket 부착음은 무기 위치를 잘 따르지만 짧은 One-shot에서는 위치 재생과 체감 차이가 작을 수 있다.
- Preset의 `CasingDrop`과 Casing Actor의 `ImpactSound`가 함께 존재해 데이터 소유권이 혼동될 여지가 있다.
- Concurrency와 Attenuation 설정이 부족하면 다수 무기의 동시 발사에서 소리가 혼잡해질 수 있다.

## Future
- Indoor 상태 자동 판정
- Reload Start, Insert, End 사운드
- 표면 재질별 Casing Impact
- Concurrency와 Attenuation 튜닝
- 미사용 `SoundSet.CasingDrop` 슬롯 정리 여부 결정
- 관련 문서: [[weapon-system]]
