# Weapon Audio System

## Overview
권총 기준으로 발사(실내/실외), 빈 탄창(DryFire), 탄피 배출(CasingDrop optional) 사운드를 데이터 에셋(WeaponPresetDA) 기반으로 연결한다.
무기별 사운드 교체는 코드 수정 없이 DA만 교체하도록 하고, WeaponBase 변경은 최소화한다

## Key Decisions
- 사운드 슬롯을 FWeaponSoundSet 구조체로 묶어 WeaponPresetDA에 보관
    - 무기 추가/교체 시 “DA만 바꾸면 끝”인 데이터 기반 확장 구조 확보
- 실내/실외 판정은 Phase 1에서는 수동 제공(bIsIndoor)
    - 자동 판별(볼륨/트리거)은 Phase 2로 미루고, 지금은 동작 검증/구조 안정성을 우선
- 사운드 재생은 PlayWeaponSfx() 단일 함수로 통일
    - Attach(소켓) 우선 + 실패 시 Location fallback으로 재생 정책을 한 곳에 고정

## Architecture
- WeaponPresetDA
    - FWeaponSoundSet(FireIndoor/FireOutdoor/DryFire/CasingDrop) 보유
- WeaponBase
    - “언제 어떤 소리를 낼지”만 결정 (탄약 체크, 실내/실외 분기)
    - 사운드 재생 자체는 PlayWeaponSfx()에 위임
- Environment Provider(Phase 1: 수동)
    - bIsIndoor를 PlayerController(또는 Character)에서 제공
    - WeaponBase는 값을 “조회”만 함 (판정 로직 직접 구현 X)
- 재생 위치(소켓)
    - Fire/DryFire: SCK_Muzzle
    - CasingDrop: SCK_Ejector
핵심 원칙:
- WeaponBase는 DA의 사운드 “데이터”만 읽고, 오디오 시스템의 세부 재생 정책은 모른다.
- Tick 기반 폴링 없이, 발사 시도/성공 같은 이벤트 지점에서만 호출한다.

## Trade-offs
- Phase 1의 실내/실외는 자동 판별이 아니라 수동 값이라, 맵에 따라 토글 관리가 필요하다.
- 탄피 소리는 Ejector 소켓에서 재생하므로 “바닥에 떨어지는 위치”의 리얼함은 부족할 수 있다.
- Concurrency/Attenuation은 적용 가능하지만, 상세 튜닝은 Phase 2에서 진행하는 편이 안전하다.

## Future
- 실내/실외 자동 판별: 트리거/오디오 볼륨 기반으로 bIsIndoor 자동 갱신
- Reload 사운드 확장: ReloadStart/Insert/End 슬롯 추가 + 애님 노티파이 타이밍 연동
- 탄피 바닥 사운드: 탄피 액터/간이 트레이스로 접촉 지점에서 재생하도록 개선
- 무기별 사운드 다양화: Fire 레이어(메카닉/테일) 또는 SoundCue/MetaSound로 확장
