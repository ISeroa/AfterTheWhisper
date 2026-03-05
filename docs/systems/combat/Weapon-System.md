# Weapon-System

## Overview

Weapon System은 플레이어가 사용하는 총기의 동작을 관리하는 핵심
시스템이다.

이 시스템은 Weapon DataAsset을 통해 무기 스펙을 정의하고, 발사, 재장전,
사운드, 이펙트 등의 기능을 통합적으로 처리한다.

Weapon System은 Aim System과 연결되어 사격 정확도와 전투 감각을
조정한다.

------------------------------------------------------------------------

## Key Decisions

-   무기 데이터는 **Weapon DataAsset**으로 관리한다
-   무기 로직은 **WeaponActor 기반 구조**를 사용한다
-   발사는 **라인 트레이스 기반 히트스캔 방식**을 사용한다
-   정확도는 **SpreadDeg** 값으로 제어한다
-   사운드 및 이펙트는 WeaponActor에서 직접 처리한다

------------------------------------------------------------------------

## Architecture

Weapon 시스템 전체 구조

    WeaponDataAsset
          |
          v
    WeaponActor
          |
          |-- Fire Logic
          |
          |-- Reload Logic
          |
          |-- Audio System
          |
          |-- Visual Effects
          |
          `-- Casing System

WeaponActor는 다음 기능을 담당한다.

    Fire()
    Reload()
    PlayWeaponSound()
    SpawnMuzzleFlash()
    SpawnCasing()

------------------------------------------------------------------------

## Weapon Data

무기 스펙은 WeaponDataAsset에서 정의된다.

예시 데이터 구조

    WeaponName
    Damage
    FireRate
    SpreadDeg
    MagazineSize
    ReloadTime

### SpreadDeg

SpreadDeg는 발사 시 탄 퍼짐 정도를 나타낸다.

Aim 상태에서는 다음과 같이 계산된다.

    FinalSpread = SpreadDeg * AimSpreadMultiplier

------------------------------------------------------------------------

## Fire Logic

무기 발사는 히트스캔(Line Trace) 방식으로 처리한다.

발사 과정

    Input Fire
        |
        v
    WeaponActor::Fire()
        |
        v
    Apply Spread
        |
        v
    LineTrace
        |
        v
    Damage Apply

히트스캔 방식의 장점

-   구현 단순
-   성능 효율적
-   탑다운 슈터에 적합

------------------------------------------------------------------------

## Reload System

재장전은 WeaponActor에서 관리된다.

재장전 과정

    Reload Input
         |
         v
    Reload Start
         |
         v
    Reload Timer
         |
         v
    Ammo Refill

Reload UI는 별도의 UI 시스템에서 처리한다.

------------------------------------------------------------------------

## Audio System

무기 사운드는 WeaponActor에서 재생한다.

예시

    Fire Sound
    Reload Sound
    Empty Magazine Sound

사운드는 다음 방식으로 재생된다.

    PlaySoundAttached

------------------------------------------------------------------------

## Visual Effects

무기 발사 시 다음 이펙트가 발생한다.

    Muzzle Flash
    Bullet Impact
    Shell Casing

Muzzle Flash는 Niagara 시스템을 사용한다.

------------------------------------------------------------------------

## Trade-offs

-   히트스캔 방식은 탄속 기반 시스템보다 현실성이 낮다.
-   Spread 기반 정확도 시스템은 단순하지만 복잡한 반동 모델에는 한계가
    있다.

------------------------------------------------------------------------

## Future

-   Recoil System 추가
-   Projectile 기반 무기 지원
-   Weapon Attachment 시스템
-   무기 모드 (Single / Burst / Auto)
