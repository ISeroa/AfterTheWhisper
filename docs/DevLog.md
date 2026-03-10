# Development Log

이 문서는 하루 작업 종료 시 작성한다.
목표는 "결정과 방향"을 기록하는 것.


---

## 📅 2026-03-10

### 🎯 오늘 목표 (최대 3개)
- 플레이어 애니메이션 교체 파이프라인 탐색 및 시작

---

### 완료한 작업
- 플레이어 애니메이션 교체 작업 개시 (Mixamo 기반 스켈레톤 → Unreal 기준 애니메이션)
- Blender에서 root bone 추가 후 `mixamorig:Hips`를 하위에 배치해 스켈레톤 구조 수정
- 수정된 FBX Unreal 재임포트 시도

---

### 발생한 문제

| 문제 | 원인 |
|---|---|
| UE import 시 "Hips is root bone but animation has no root track" 오류 | Unreal 기준 애니메이션은 root bone 트랙을 요구하나 Mixamo 스켈레톤에는 Hips가 root |
| 재임포트 후 캐릭터가 Z축 방향으로 이동 | Blender ↔ Unreal 간 축 변환 불일치 |
| `ABP_Player` 애니메이션 그래프 일부 노드 소실 | 스켈레톤 구조 변경으로 기존 ABP와 레퍼런스 불일치 |

---

### 해결 방법 / 결정 사항
- 커밋 히스토리 확인 결과 애니메이션 관련 변경이 아직 커밋되지 않은 상태임을 확인
- 프로젝트 안정성 우선 → `git reset --hard`로 마지막 안정 커밋 상태 복구
- Blender 작업 파일은 별도 폴더에 보존되어 있어 데이터 손실 없음
- 애니메이션 교체 파이프라인을 명확히 정리한 뒤 재시도하기로 결정

---

### 미완료 / 보류
- 플레이어 애니메이션 교체 (파이프라인 재정립 후 재시도)

---

### 구조적 메모
- **Mixamo → Unreal 애니메이션 교체 시 주요 장벽**: root bone 트랙 유무, FBX 축 변환, ABP 레퍼런스 무효화 세 가지가 동시에 발생함
- **임포트 전 체크리스트 필요**: Blender export 시 축 설정 (Forward: -Z, Up: Y), root bone 포함 여부, FBX 버전 통일
- **Skeleton 구조 변경은 기존 ABP를 파괴**: 스켈레톤을 바꾸기 전에 ABP 백업 또는 Retarget 전략을 먼저 확정해야 함
- **미커밋 상태에서 구조 변경 작업 시작**: 실험적 작업은 별도 브랜치에서 진행하거나 중간 커밋을 자주 남길 것

---

### ▶ 다음 작업 계획
- 애니메이션 교체 파이프라인 정리
    - Blender FBX export 축 설정 확인 (Forward / Up 방향)
    - root bone 추가 방식과 Unreal 임포트 옵션 조합 재검토
    - ABP_Player retarget 또는 재작성 방안 결정
- 파이프라인 확정 후 애니메이션 교체 재시도
## 📅 2026-03-08

### 🎯 오늘 목표 (최대 3개)
- 사격 몽타주 정렬 문제 원인 파악 및 해결

---

### 완료한 작업

**[UpperBody 사격 몽타주 테스트 및 적용]**
- `AM_Pistol_Fire` 몽타주를 UpperBody 슬롯에 연결하여 재생 테스트

**[총구 방향 불일치 문제 해결]**
- 사격 시 총구 방향과 실제 발사 방향이 어긋나는 문제 발견 및 원인 조사
- Layered Blend Per Bone 설정이 팔/손 회전에 미치는 영향 확인
- UpperBody 애니메이션 블렌딩 과정에서 팔/손 회전이 틀어지는 문제 확인
- `Mesh Space Rotation Blend` 활성화로 상체 회전 안정화
- 사격 애니메이션을 교체하지 않고 애니메이션 커브 및 레이어 트랙을 직접 수정하여 포즈 보정
- 총구 방향과 사격 방향이 시각적으로 자연스럽게 맞도록 튜닝
- 완벽한 정렬은 아니지만 향후 Aim Spread 적용 시 체감 문제 없을 수준으로 개선

---

### 발생한 문제
- Layered Blend Per Bone에서 Mesh Space Rotation Blend 비활성화 상태에서 상체 회전이 로컬 스페이스로 누산되어 팔 방향 틀어짐 발생

---

### 해결 방법 / 결정 사항
- `Mesh Space Rotation Blend` 활성화 → 상체 본 회전이 메시 스페이스 기준으로 블렌딩되어 팔 방향 안정화
- 사격 시스템 로직은 변경하지 않고 애니메이션 레벨에서 문제 해결 (몽타주/커브 수정)
- 애셋 교체 없이 기존 애니메이션 튜닝으로 해결 → 향후 Aim Spread 도입 시 재검토

---

### 미완료 / 보류
- 정밀 정렬은 Aim Spread 적용 이후 재평가

---

### 구조적 메모 (선택)
- `Layered Blend Per Bone`에서 `Mesh Space Rotation Blend`는 상체 에임 블렌딩 시 거의 필수 — 비활성화하면 본 체인 누산으로 팔 방향이 틀어짐
- 총구 방향 불일치는 발사 로직이 아닌 애니메이션 레이어 설정에서 비롯될 수 있음

---

### ▶ 내일 할 일 (최대 3개)
- Hit Impact VFX/사운드
- Enemy 피격/사망 처리
- Camera Shake (발사/피격)

---

---

## 📅 2026-03-07

### 🎯 오늘 목표 (최대 3개)
- 사격 시 Fire Montage 재생 구현

---

### 완료한 작업

**[Fire Montage 연동]**
- `ATDPlayerCharacter::OnFirePressed()`에 `Montage_Play(FireMontage)` 호출 추가
- `FireMontage` 변수(`EditDefaultsOnly`)는 기존 헤더에 이미 선언되어 있었으므로 CPP만 수정
- AnimGraph 구조: Locomotion → UpperBody Slot → LayeredBlendPerBone(Spine1) — 상체만 사격, 하체 Locomotion 유지
- `BP_TDPlayerCharacter` → Fire Montage 슬롯에 `AM_Pistol_Fire` 할당, 정상 재생 확인

---

### 발생한 문제
- Idle → Fire 시작 포즈 차이가 커서 팝핑 현상 발생 → 현재 `AM_Pistol_Fire` 에셋 교체 필요

---

### 해결 방법 / 결정 사항
- 코드 구조는 확정. 애니메이션 에셋만 교체하면 해결 가능

---

### 미완료 / 보류
- Fire Montage 에셋 교체 (Idle 포즈와 시작 포즈가 연속적인 애니메이션으로)

---

### 구조적 메모 (선택)
- 몽타주 재생은 `OnFirePressed()`에서 처리 — 발사 입력과 동일 타이밍, Tick 불필요
- UpperBody Slot + LayeredBlendPerBone(Spine1) 구조에서 상체 몽타주는 코드 추가 없이 동작

---

### ▶ 내일 할 일 (최대 3개)
- Fire Montage 에셋 교체
- Hit Impact VFX/사운드
- Enemy 피격/사망 처리

---

---

## 📅 2026-03-04

### 🎯 오늘 목표 (최대 3개)
- ATDEnemyAIController 구현 (기반 구조 → MoveToLocation → Encircle 전술 → 다중 적 분산)

---

### 완료한 작업

**[ATDEnemyAIController 기반 구조]**
- `AAIController` 상속 `ATDEnemyAIController` 클래스 신규 생성 (`Public/AI/`, `Private/AI/`)
- `OnPossess` / `OnUnPossess` 오버라이드 + UE_LOG 확인 로그
- `ATDEnemyCharacter` 생성자에서 `AIControllerClass = ATDEnemyAIController::StaticClass()`, `AutoPossessAI = PlacedInWorldOrSpawned`
- Build.cs에 `AIModule` 의존성 추가

**[Timer 기반 UpdateMoveTarget 루프]**
- `RepathInterval`(0.35f) UPROPERTY
- `OnPossess`에서 루핑 Timer 시작 — 적마다 `InitialDelay = RandRange(0, RepathInterval)` 로 첫 실행 타이밍 분산
- `OnUnPossess`에서 `ClearTimer`

**[ETDMovementTactic enum 및 DirectChase]**
- `ETDMovementTactic { DirectChase, Encircle }` (BlueprintType)
- `GetPlayerPawn()` (`UGameplayStatics::GetPlayerPawn(0)`)
- `ComputeMoveGoal()` — switch 분기, DirectChase는 PlayerLocation 반환
- `UpdateMoveTarget()`에서 `MoveToLocation(Goal, AcceptanceRadius=60, bStopOnOverlap=true)`

**[Encircle 전술 — 슬롯 기반 비용 선택 최종 구조]**
- `NumSlots`(12) 개의 고정 슬롯을 플레이어 주변 원 위에 배치 (월드 고정 X/Y 축 기준, 플레이어 회전 무관)
- 매 `UpdateMoveTarget`에서 각 슬롯 비용을 계산해 최저 비용 슬롯 선택:
  ```
  Cost = Dist2D(SelfLoc, SlotPos)
       + OccupancyPenalty(400)  × (반경 OccupancyRadius(160) 안 실제 위치한 적 수)
       + SameSlotPenalty(100000) × (다른 ATDEnemyAIController 중 CurrentSlotIndex == i 인 수)
  ```
- `CurrentSlotIndex` — 선택 즉시 저장, 다른 컨트롤러가 `TActorIterator`로 읽어 중복 회피
- `OnUnPossess`에서 `CurrentSlotIndex = INDEX_NONE` — 사망/해제 즉시 슬롯 해방
- `RadiusBias = RandRange(-Jitter, Jitter)` — `OnPossess` 1회 고정 (반경 미세 분산)
- 디버그: 후보 슬롯 회색 점 / 선택 Goal 초록 구체 (비 Shipping)

**[UTDEnemyMeleeAttackComponent — 근접 공격 컴포넌트]**
- `UActorComponent` 상속, `PrimaryComponentTick.bCanEverTick = false` (Tick 미사용)
- `TryAttack(AActor* Target)` — `bOnCooldown` 또는 `DistSquared > AttackRange²` 이면 즉시 return
- 범위 내 + 쿨다운 없을 시: `bOnCooldown = true` → `WindupTimerHandle` 시작
- `ExecuteHit()` — 윈드업 후 DistSquared 재확인 → `UGameplayStatics::ApplyDamage` 호출 → `CooldownTimerHandle` 시작
- `ResetCooldown()` — `bOnCooldown = false`
- `TWeakObjectPtr<AActor> PendingTarget` — 윈드업 중 타깃 사망 시 자동 null 처리
- `ATDEnemyCharacter` 생성자에서 `CreateDefaultSubobject` — BP에서 AttackRange / WindupTime / Cooldown / AttackDamage 조정 가능
- `ATDEnemyAIController::UpdateMoveTarget()` 에 `TryAttack(Player)` 호출 2줄 추가 (기존 구조 최소 수정)

---

### 발생한 문제 및 폐기된 중간 시도

| 시도 | 문제 | 결정 |
|---|---|---|
| PlayerForward 기반 Encircle 방향 계산 | 플레이어 제자리 회전 시 적 목표도 회전 | 월드 고정 축(X/Y)으로 교체 |
| UniqueID 기반 고정 SlotAngle | 스폰 위치와 무관한 각도 → 뭉침 | 상대 위치 atan2로 교체 |
| AngleLockMode (Locked/Unlocked/Reacquire) | 슬롯 고정/갱신 제어는 해결책이 아니었음 | 슬롯 비용 선택으로 전면 교체, 제거 |
| NumSlots 양자화(RoundToInt) | 다수 적이 같은 양자화 슬롯으로 몰림 | 비용 함수(OccupancyPenalty + SameSlotPenalty)로 대체 |
| AngleBiasRad 인스턴스 편향 | 슬롯 기반에서 의미 없어짐 | 제거, RadiusBias만 유지 |

---

### 해결 방법 / 결정 사항
- **위치 기반 + 의도 기반 이중 패널티**: 실제 위치(OccupancyPenalty)와 예약 의도(SameSlotPenalty)를 모두 비용에 반영 → 전역 매니저 없이 분산 달성
- **SameSlotPenalty = 100000**: 거리 비용(수백)을 압도하는 크기여야 중복 슬롯 선택이 사실상 차단됨
- **InitialDelay 분산**: 같은 프레임에 모든 적이 동시에 슬롯을 선택하는 경쟁 완화
- **`MoveToLocation` 전제**: 레벨에 RecastNavMesh 필수

---

### 미완료 / 보류
- Enemy 피격/사망 처리 (HealthComponent 공용화)
- 정지거리 — AttackRange 내 진입 시 이동 중단 (현재는 이동 중에도 공격 판정)

---

### 구조적 메모
- 전역 슬롯 예약 테이블 없이 `CurrentSlotIndex` 공개 멤버 + `TActorIterator` 순회만으로 의도 기반 분산 구현 가능
- `const` 함수에서 멤버 상태(`CurrentSlotIndex`)를 변경해야 할 경우 함수 자체를 non-const로 선언하는 것이 `mutable`보다 의도가 명확함
- OccupancyPenalty(위치)와 SameSlotPenalty(의도)는 스케일이 달라야 함 — 의도 패널티가 거리 비용보다 수백 배 커야 실질적 회피가 됨

---

### ▶ 내일 할 일 (최대 3개)
- Enemy HealthComponent 연결 및 피격/사망 처리
- Hit Impact VFX/사운드
- Camera Shake (발사/피격)

---

---

## 📅 2026-02-27

### 🎯 오늘 목표 (최대 3개)
- ATDCasing 캐릭터 충돌 버그 원인 파악 및 해결

---

### 완료한 작업
- ATDCasing 캐릭터 충돌 이슈 원인 규명 및 수정
    - BP_TDCasing에 남아 있던 Collision 오버라이드 값을 "Reset to Default"로 초기화
    - C++ 생성자에서 설정한 PhysicsOnly / ECC_Pawn Ignore가 런타임에 정상 적용됨을 확인
- CLAUDE.md 생성 및 Physics/Collision 주의 사항 섹션 추가

---

### 발생한 문제
- C++에서 Collision 설정을 변경해도 런타임에 반영되지 않음
    - PhysicsOnly, Pawn Ignore를 생성자에서 설정했으나 캐릭터가 탄피 위에서 계속 들썩임

---

### 해결 방법 / 결정 사항
- **근본 원인**: Unreal Engine은 Blueprint가 C++ 컴포넌트 기본값을 인스턴스별로 오버라이드할 수 있으며,
  이미 생성된 BP는 이후 C++ 기본값 변경을 자동으로 상속하지 않음
- BP_TDCasing 에디터에서 MeshComp Collision 프로퍼티 → "Reset to Default" 초기화로 해결
- 향후 보장이 필요한 Collision 설정은 `BeginPlay()`에서 강제 적용하는 패턴도 사용 가능

---

### 미완료 / 보류
- 탄피 사운드 소재별 분기 (Phase 2)

---

### 구조적 메모 (선택)
- C++ 생성자 기본값 변경 후 BP에서 의도대로 동작하지 않으면 BP 오버라이드 잔존 여부를 먼저 의심
- Physics/Collision처럼 런타임 동작에 직결되는 설정은 `BeginPlay()` 강제 적용을 표준 패턴으로 고려
- BP에서 "Reset to Default" 후 저장하지 않으면 변경이 유지되지 않으므로 반드시 저장 확인

---

### ▶ 내일 할 일 (최대 3개)
- Hit Impact VFX/사운드
- Hit Marker UI
- Enemy 기본 구조 (Chase + Attack)

---

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
- ~~**ATDCasing 바닥 들썩임**~~ → 2026-02-27 해결 (BP_TDCasing 오버라이드 초기화)
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
