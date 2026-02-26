# Development Log

이 문서는 하루 작업 종료 시 작성한다.
목표는 "결정과 방향"을 기록하는 것.


---

## 📅 2026-02-26

### 🎯 오늘 목표 (최대 3개)
- 무기 사운드 시스템 구현 (발사/DryFire/탄피)
- Niagara 머즐 플래시 C++ 연동
- ATDCasing 탄피 액터 구현

---

### 완료한 작업
**[무기 사운드 / 머즐 플래시]**
- FWeaponSoundSet 구조체 추가 (FireIndoor / FireOutdoor / DryFire / CasingDrop)
- WeaponPresetDA에 SoundSet, MuzzleFlashEffect, MuzzleSocketName 프로퍼티 추가
- ATDPlayerController에 bIsIndoor + GetIsIndoor() 추가
- PlayWeaponSfx() 구현 (SpawnSoundAttached 우선, fallback PlaySoundAtLocation)
- FireOnce()에서 발사 성공 시 Indoor/Outdoor 분기 사운드 재생
- FireOnce()에서 탄 0 이하 시 DryFire 사운드 재생
- GetMuzzleTransformWS() 추가 (소켓 존재 로그 포함, fallback ComponentTransform)
- SpawnMuzzleFlash() 추가 → FireOnce()에서 호출
- Build.cs에 Niagara 모듈 추가
- MuzzleSocketName을 DA에서 지정 가능하도록 이전 (기본값 "SCK_Muzzle")
- SetPartsFromPreset()에서 MuzzleSocketName DA 값 적용

**[ATDCasing 탄피 액터]**
- ATDCasing 최소 뼈대 구현 (StaticMeshComp Root, PhysicsOnly, LifeSpan 8s, Tick 비활성화)
- WeaponPresetDA에 TSubclassOf<ATDCasing> CasingClass 추가
- ATDWeaponBase::SpawnCasing() 구현 → SCK_Ejector 소켓 기준 스폰, FireOnce() 발사 성공 경로에서만 호출
- ATDCasing::AddIgnoredActor() 추가 → 스폰 직후 무기 액터(this) Ignore 등록
- ATDCasing::OnHit() 구현 → WorldStatic 충돌 + MinImpactSpeed 조건 시 ImpactSound 1회 재생
- FireOnce()의 CasingDrop 사운드 재생 제거 → 탄피 impact 사운드는 ATDCasing에서만 담당하도록 이관
- 충돌 채널 정리: ECC_Pawn / ECC_PhysicsBody / ECC_Camera → Ignore, PhysicsOnly 설정

---

### 발생한 문제
- Niagara가 원점(0,0,0)에서 스폰되는 현상
    - DrawDebugSphere는 정상 위치를 표시했으나 Niagara만 원점 스폰
- 소켓 이름 불일치 (코드 기본값 "Muzzle" vs 실제 소켓 "SCK_Muzzle")
- ATDCasing 생성자에서 EXCEPTION_ACCESS_VIOLATION (address 0x1c0)
    - SetSimulatePhysics(true)를 생성자에서 호출 → CDO 시점에 물리 핸들 미초기화
- ATDCasing이 캐릭터 바닥 판정에 걸려 들썩임
    - QueryAndPhysics 상태에서 캐릭터 플로어 스윕에 탐지됨

---

### 해결 방법 / 결정 사항
- Niagara 원점 스폰: !bExists일 때 FTransform::Identity 대신 GetComponentTransform() fallback으로 수정
- 소켓명을 DA의 MuzzleSocketName 필드로 이전하여 데이터 기반으로 관리
- ATDCasing 생성자 크래시: SetSimulatePhysics() → BodyInstance.bSimulatePhysics = true 로 변경
    - 생성자에서는 런타임 물리 API 사용 금지, 초기값 직접 설정 패턴 사용
- 바닥 들썩임: PhysicsOnly + ECB_No + ECC_Pawn Ignore 적용 시도 → 미해결, 내일 재점검

---

### 미완료 / 보류
- **ATDCasing 바닥 들썩임 미해결** — PhysicsOnly/ECB_No 적용했으나 캐릭터가 탄피 위에서 튀는 현상 잔존
- Concurrency / Attenuation 세부 튜닝 (Phase 2)
- 실내/실외 자동 판별 (트리거/볼륨 기반, Phase 2)
- ATDCasing ImpactSound 소재별 분기 (Phase 2)

---

### 구조적 메모 (선택)
- DrawDebugSphere와 실제 반환값의 fallback 경로가 다를 경우 디버그가 오히려 혼선을 줄 수 있음
    → 디버그 표시 로직과 실제 반환 로직은 동일한 fallback 경로를 공유해야 함
- 소켓명처럼 메시 의존적인 값은 WeaponBase 하드코딩보다 DA에서 지정하는 편이 안전
- UE4 생성자에서 SetSimulatePhysics() 금지 — CDO 시점에 World/PhysicsScene 없음
    → BodyInstance 멤버 직접 설정으로 대체
- PhysicsOnly와 QueryAndPhysics의 차이: 캐릭터 플로어 스윕은 Query 기반이므로 PhysicsOnly로 차단 가능
    → OnComponentHit는 물리 엔진 레벨 콜백이므로 영향 없음
- 탄피 사운드 책임은 WeaponBase(발사 시점)가 아닌 ATDCasing(충돌 시점)에서 담당
    → 물리 충돌 위치/타이밍이 자연스럽고, 소리 발생 지점이 정확해짐

---

### ▶ 내일 할 일 (최대 3개)
- Hit Impact VFX/사운드
- Hit Marker UI
- Enemy 기본 구조 (Chase + Attack)

---

---

## 📅 2026-02-22

### 🎯 오늘 목표 (최대 3개)
- Niagara 기반 머즐 플래시 구현
- SubUV atlas 적용
- 무기 소켓 네이밍 정리

---

### 완료한 작업
- 4x3 atlas 기반 Niagara 머즐 플래시 시스템 구현
- Unlit + Additive 머티리얼 구성 (ParticleSubUV + ParticleColor 적용)
- Burst 기반 1회성 이펙트 구성
- SubImageIndex 0~11 랜덤 설정
- 소켓 네이밍을 SCK_ 접두사로 통일
- FX 폴더 구조 정리 (FX/Weapons/MuzzleFlash)

---

### 발생한 문제
- 머티리얼에서 ParticleColor를 사용하지 않아 색상 적용이 되지 않음
- SubUV 설정 위치(UI 구조) 파악에 시간 소요

---

### 해결 방법 / 결정 사항
- 머티리얼에서 ParticleColor * SubUV 구조로 수정
- Additive 특성상 Emissive 강도 스케일 적용
- 소켓 네이밍은 enum과 분리 유지, 에셋 레벨에서만 SCK_ 통일

---

### 미완료 / 보류
- 머즐 플래시 광원(Light Renderer) 추가 여부 검토
- 무기 파츠별 머즐 효과 차등 적용 설계

---

### 구조적 메모 (선택)
- 로직(enum Slot)과 에셋 네이밍(Socket)은 분리 유지
- 파츠 변경 시 머즐 기준 컴포넌트 캐시 갱신 구조 유지
- Burst 기반 FX는 풀링 없이 단순 스폰 유지

---

### ▶ 내일 할 일 (최대 3개)
- 탄약/재장전 사운드 훅

---

---

## 📅 2026-02-21

### 🎯 오늘 목표 (최대 3개)
- 플레이어 체력 UI 구현

---

### 완료한 작업
- ATDPlayerCharacter 상속을 ACharacter → ATDBaseCharacter로 변경
    - HealthComponent 자동 획득, HandleDeath 바인딩 포함
- UTDPlayerStatusHUD와 OnHealthChanged 델리게이트 연결
    - BeginPlay에서 위젯 생성 + 바인딩 + 초기값 동기화
- TestDamage 함수 추가 (P키 → 10 데미지)
    - DefaultInput.ini에 TestDamage 액션 매핑 추가
- ReloadBarWidget 중복 BindWeapon 호출 제거

---

### 발생한 문제
- BindKey(EKeys::P) 사용 시 FInputChord 링커 에러 발생
    - InputCore가 PublicDependencyModuleNames에 있음에도 dllimport 심볼 미해결

---

### 해결 방법 / 결정 사항
- BindKey 대신 DefaultInput.ini에 액션 매핑 추가 후 BindAction으로 변경
    - Fire/Reload와 동일한 방식으로 통일

---

### 미완료 / 보류
- 체력 감소 시 색상 변화 연출
- 피격 시 Vignette/붉은 플래시 연동

---

### 구조적 메모 (선택)
- HealthComponent::BeginPlay()는 Actor::BeginPlay()보다 먼저 실행 → 초기값 동기화 안전
- 플레이어/적 모두 ATDBaseCharacter를 통해 동일한 HealthComponent 구조 공유
- 화면 고정 HUD는 PlayerController 위치 제어 불필요 (Ammo/Reload와 다름)

---

### ▶ 내일 할 일 (최대 3개)
- Muzzle Flash 구현
- Hit Impact 구현

---

---
## 📅 2026-02-20

### 🎯 오늘 목표 (최대 3개)
- WeaponPresetDA에 실제 무기 스탯 추가
- WeaponBase가 Preset 기반으로 스탯 초기화하도록 수정
- 탄약 초기값 및 UI 갱신 흐름 안정화

---

### 완료한 작업
- UTDWeaponPresetDA에 FTDWeaponStats 구조체 추가
- Preset에 FireRate, Damage, Range, SpreadDeg, MagazineSize, ReloadTime 필드 정의
- ATDWeaponBase::SetPartsFromPreset()에서:
    - Preset->Stats 기반으로 무기 스탯 적용
    - MagazineSize 기준으로 AmmoInMag 초기화
    - NotifyAmmoChanged() 호출로 UI 동기화
- 권총 기본 스펙 설정 및 정상 동작 확인

---

### 발생한 문제
- Preset에 Stats를 추가했으나 Preset->SpreadDeg 형태로 직접 접근하여 컴파일 에러 발생

---

### 해결 방법 / 결정 사항
- 모든 스탯 접근을 Preset->Stats.XXX 구조로 수정
- 스탯 초기화는 BeginPlay가 아니라 Preset 적용 시점에서 처리하도록 결정
- 무기 데이터의 단일 진입점은 Preset으로 유지
---

### 미완료 / 보류
- Reload 타이머 실제 동작 검증
- 파츠가 스탯에 영향 주는 구조 여부 미정

---

### 구조적 메모 (선택)
- WeaponPresetDA가 "데이터 소스 단일화 역할"을 담당하도록 구조가 정리됨
- WeaponBase는 Preset을 적용받는 실행 객체 역할로 분리 유지
- 스탯 확장은 Preset 구조 안에서만 진행 가능하도록 설계

---

### ▶ 내일 할 일 (최대 3개)
- 플레이어 체력 UI 구현

---

---
## 📅 2026-02-19

### 🎯 오늘 목표 (최대 3개)
- 재장전 인디케이터 UI 구현
- 재장전 시간과 게이지 동기화
- Weapon ↔ UI 이벤트 구조 확정

---

### 완료한 작업
- ReloadBar 위젯 생성 및 Viewport 추가
- WeaponBase의 OnReloadUIStart / OnReloadUIStop 델리게이트와 위젯 바인딩
- 재장전 Duration 기반 애니메이션 재생 속도 동기화 구현
- ReloadBarWidget 위치를 PlayerController에서 마우스 기반으로 갱신

---

### 발생한 문제
- 재장전 게이지가 실제 재장전 시간보다 빨리 차는 현상
    - Percent 트랙 마지막 키가 0.65초에 위치해 있던 문제 발견
- ReloadBarWidget이 마우스를 따라오지 않음
    - PlayerController에서 AmmoWidget만 위치 갱신하고 있던 버그
---

### 해결 방법 / 결정 사항
- PlayerController는 위치 제어만 담당하도록 구조 유지
- UI 갱신은 Tick 기반이 아닌 Weapon Delegate 이벤트 기반 구조 유지
- 현재는 타이머 기반 재장전, 추후 몽타주 Notify 기반으로 전환 예정

---

### 미완료 / 보류
- 몽타주 기반 재장전 전환
- 재장전 중 인터럽트(피격/구르기 등) 상태 처리
- ReloadBar 페이드 아웃 연출 여부 결정
---

### 구조적 메모 (선택)
- UI 애니메이션과 실제 게임 로직 동기화는 "시간 계산"보다 "구조 정합성"이 중요
- 위젯은 Character에서 생성, 위치 제어는 PlayerController에서 처리하는 책임 분리 유지
- 애니메이션 트랙 키 위치 실수는 체감 속도 왜곡의 주요 원인

---

### ▶ 내일 할 일 (최대 3개)
- 체력 UI 구현 (이벤트 기반 구조 유지)
- 무기 DataAsset으로 실제 수치 이전
- WeaponBase 하드코딩 값 정리 및 DA 참조 구조 정리

---

---

## 📅 2026-02-16

### 🎯 오늘 목표 (최대 3개)
- 동적 머티리얼 기반 탄약 파이 UI 구현
- 무기 시스템과 UI를 델리게이트로 연결
- Ammo Indicator를 마우스 추적형 UI로 확장

---

### 완료한 작업
- 도넛 형태의 Dynamic Material 기반 Ammo Pie Indicator 구현
- 무기 클래스의 OnAmmoChanged 델리게이트를 위젯에 바인딩
- 무기 상태 변경 시 UI가 자동 갱신되도록 이벤트 기반 구조 확정
- Ammo Indicator를 마우스 오른쪽에 위치하도록 PlayerController에서 위치 갱신 처리
- Offset을 Blueprint에서 조절 가능하도록 노출

---

### 발생한 문제
- 델리게이트 바인딩을 Blueprint에서 처리하려고 했을 때 동작하지 않는 현상 발생
    - C++에서 델리게이트를 쏘고, BP에서 바인딩/연결하려 했으나
    - 호출(브로드캐스트) 시점이 BP 바인딩보다 먼저 발생하는 케이스가 생겨 UI 갱신이 누락됨
- Offset 하드코딩으로 BP 값이 적용되지 않는 문제 확인됨
---

### 해결 방법 / 결정 사항
- BP에서 연결/바인딩을 맡기지 않고, C++에서 위젯 생성 + 델리게이트 바인딩까지 전부 세팅하도록 변경
    -> 호출 순서(초기화 타이밍) 문제를 제거하고, 동작을 확실하게 고정
- UI 갱신은 Tick 기반 폴링이 아니라 Weapon Delegate 기반 이벤트 구조로 유지
- 하드코딩 제거 후 MouseOffset 멤버 변수를 사용하도록 통일
- UI는 Character에서 생성, 위치 제어는 PlayerController가 담당하는 구조 확정
---

### 미완료 / 보류
- 화면 가장자리 보정 로직
- 마우스 추적 보간 처리 여부 결정
- Reload Indicator UI 설계

---

### 구조적 메모 (선택)
-BP에 바인딩을 맡기면 “동작은 되는데 가끔 안 되는” 초기화 순서 버그가 생기기 쉬움
    -> 핵심 연결부(위젯 생성/바인딩)는 C++에서 확정하는 편이 안전
- Weapon과 UI는 델리게이트로 연결하되, 바인딩 주체는 C++로 고정해 예측 가능성 확보

---

### ▶ 내일 할 일 (최대 3개)
- 재장전 인디케이터 UI 설계 (괄호형 게이지)
- Reload 진행도 계산 방식 결정 (시간 기반)
- 장전 시작/종료 이벤트를 UI 표시/숨김과 연결

---

---

## 📅 YYYY-MM-DD

### 🎯 오늘 목표 (최대 3개)
- 
- 
- 

---

### 완료한 작업
- 
- 
- 

---

### 발생한 문제
- 

---

### 해결 방법 / 결정 사항
- 

---

### 미완료 / 보류
- 

---

### 구조적 메모 (선택)
-
- 

---

### ▶ 내일 할 일 (최대 3개)
- 
- 
- 

---
