# AGENTS.md

This file provides guidance to Codex (Codex.ai/code) when working with code in this repository.

---

## Build

```bat
build.bat
```

내부적으로 UE 4.27 Build.bat을 호출한다. UE 경로: `D:\EpicGames\UE_4.27`

에디터에서 빌드할 경우: Ctrl+Shift+B (Hot Reload 포함)
C++ 추가/삭제 후 `.uproject` → "Generate Visual Studio project files" 필요.

---

## Architecture

### Class Hierarchy

```
ACharacter
 └─ ATDBaseCharacter          (UTDHealthComponent 소유)
     ├─ ATDPlayerCharacter    (무기 생성/에임/입력/UI 바인딩 담당)
     └─ ATDEnemyCharacter

AActor
 └─ ATDWeaponBase             (발사/재장전/파츠 조립)
 └─ ATDCasing                 (탄피 물리 액터)

UDataAsset
 └─ UTDWeaponPresetDA         (무기 전체 설정 — Stats, SoundSet, Parts, MuzzleFlash, CasingClass)
 └─ UTDWeaponPartDA           (슬롯 단위 파츠 — Mesh, AttachSocket, RelativeOffset)

UUserWidget
 └─ UTDW_AmmoWidget           (탄약 파이 UI, C++에서 BindWeapon으로 바인딩)
 └─ UTDReloadBarWidget        (재장전 바 UI)
 └─ UTDPlayerStatusHUD        (체력 등 통합 HUD)

UAnimInstance
 └─ UTDAnimInstance           (Speed, MoveAngle, bIsMoving — NativeUpdateAnimation)
```

### Weapon System

- `UTDWeaponPresetDA`가 단일 진실 소스. `Stats`, `SoundSet`, `Parts`, `MuzzleFlashEffect`, `CasingClass` 를 모두 포함.
- 스탯 접근은 항상 `Preset->Stats.XXX`. `Preset->FireRate` 식의 직접 접근은 컴파일 에러.
- `SetPartsFromPreset()` 호출 시점에 스탯과 파츠가 초기화됨 (BeginPlay 아님).
- 파츠는 `ETDWeaponSlot` enum → `TDSlotToName()` → FName 키로 `TMap<FName, UStaticMeshComponent*>` 에 관리.
- 머즐 위치는 `MuzzlePrioritySlot`(기본 "Barrel") 컴포넌트의 `MuzzleSocketName` 소켓에서 추출.

### Delegate Events

| Delegate | 선언 위치 | 용도 |
|---|---|---|
| `FOnAmmoChanged(int32, int32)` | TDWeaponBase.h | 탄약 변경 → UI 갱신 |
| `FOnReloadUIStart(float)` | TDWeaponBase.h | 재장전 시작 → 바 UI 표시 |
| `FOnReloadUIStop` | TDWeaponBase.h | 재장전 종료/취소 → 바 UI 숨김 |
| `FTDOnHealthChanged(float, float)` | TDHealthComponent.h | 체력 변경 → HUD 갱신 |
| `FTDOnDeath` | TDHealthComponent.h | 사망 처리 |

### UI 책임 분리

- **위젯 생성**: `ATDPlayerCharacter::BeginPlay()`
- **위젯 위치 제어**: `ATDPlayerController::PlayerTick()` (MouseOffset, ReloadWidgetOffset)
- **이벤트 바인딩**: C++에서 확정 (`BindWeapon()` 호출). BP에서 바인딩하면 초기화 순서 문제 발생.

### Indoor/Outdoor

`ATDPlayerController::bIsIndoor` 플래그로 현재 환경 구분.
발사음은 `SoundSet.FireIndoor` / `SoundSet.FireOutdoor` 중 선택.

---

## Physics & Collision

### ATDCasing — C++ 기본값 vs Blueprint 오버라이드

**이슈**: C++에서 `MeshComp`의 Collision 설정을 변경해도 런타임에 적용되지 않는 경우가 있다.

**원인**: Unreal Engine에서 Blueprint(`BP_TDCasing`)는 C++ 컴포넌트 기본값을 인스턴스별로 오버라이드할 수 있다.
이미 생성된 Blueprint는 이후 C++ 기본값 변경을 **자동으로 상속하지 않는다.**
→ C++에서 `PhysicsOnly`, `Pawn → Ignore` 등으로 수정해도 BP에 저장된 오버라이드 값이 우선 적용됨.

**증상**: 플레이어 캐릭터가 쌓인 탄피 위에서 들썩이거나 튕기는 현상.

**해결 방법 (우선순위 순)**:

1. BP 에디터에서 해당 컴포넌트 프로퍼티 → **"Reset to Default"** 로 오버라이드 초기화.
2. 확실한 보장이 필요하면 `BeginPlay()`에서 충돌 설정을 강제 적용:

   ```cpp
   void ATDCasing::BeginPlay()
   {
       Super::BeginPlay();
       MeshComp->SetCollisionResponseToChannel(ECC_Pawn, ECR_Ignore);
       // 필요한 채널 추가
   }
   ```

**일반 원칙**: Physics/Collision처럼 런타임 동작에 직접 영향을 주는 설정은, C++ 생성자 기본값만 믿지 말고 BP에서 오버라이드가 없는지 확인하거나 `BeginPlay`에서 강제 설정한다.

---

## Key Conventions

- 새 기능 아이디어는 구현하지 않고 `docs/Roadmap.md` Backlog에만 기록.
- Roadmap 체크는 기능 단위 완료 시에만.
- DevLog는 `docs/DevLog.md`에 작업 종료 전 기록.
