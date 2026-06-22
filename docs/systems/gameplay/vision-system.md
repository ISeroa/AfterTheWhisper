# Vision System

## Overview
플레이어를 기준으로 특정 위치나 Actor가 현재 시야 안에 있는지 판정하고, 그 결과로 Enemy의 표시·숨김을 제어하는 시스템이다.
근거리 원형(NearVision) + 전방 콘(ConeVision) 판정, 장애물 LineTrace, Actor 단위 가시성 조회(`IsActorVisible`), 고정 배치 Enemy의 주기적 표시·숨김(`UTDActorVisibilityComponent`)까지 구현 완료된 상태다.

## Key Decisions
- 시야 판정은 `ATDPlayerCharacter`가 소유하는 `UTDVisionComponent`에서 담당한다.
- `UTDVisionComponent`는 판정 결과만 제공하며, Actor의 Mesh 표시·숨김을 직접 제어하지 않는다.
- 판정 규칙은 **NearVision OR ConeVision**이다. 둘 중 하나를 통과하면 visible 후보다.
- `NearVisionRadius` 안이면 전방 방향과 무관하게 visible 후보로 처리한다.
- `NearVisionRadius` 밖이면 `ConeVisionDistance` + `ConeHalfAngleDeg` 기반 전방 콘을 검사한다.
- 초기 전방 시야 방향은 `Owner->GetActorForwardVector()`를 사용하며, AimTarget 연동은 코어 판정 검증 후 진행한다.
- 거리와 각도는 높이 차이로 인한 판정 흔들림을 줄이기 위해 2D 기준으로 계산한다.
- 거리 비교는 sqrt 없이 DistSquared2D를 사용한다.
- 콘 각도 비교는 DotProduct와 cos(ConeHalfAngleDeg) 비교로 처리한다.
- `IsActorVisible()`은 `IsLocationInVision()`을 내부에서 호출한 뒤 `bUseLineOfSightCheck`가 켜져 있으면 LineTrace로 장애물 가림을 추가 검사한다.
- LineTrace는 NearVision과 ConeVision 모두에 동일하게 적용된다.
- 고정 배치 Enemy의 표시·숨김은 `UTDActorVisibilityComponent`가 담당한다. 아이템·탄피로의 확장은 미구현이다.
- 시야는 대상을 밝히는 조명이 아니라, 현재 화면에 표시될 수 있는 공간적 조건으로 정의한다.
- 공간적 가시성은 **시야 형상 안에 있음 + 장애물에 가리지 않음**으로 판정한다.
- 맵 지형은 기본적으로 렌더링하며, 시야 밖과 벽 뒤의 Darkness 표현은 별도 `VisionRenderer`의 책임으로 둔다.
- `VisionRenderer`는 시야 안의 색을 밝게 보정하지 않고 시야 밖만 어둡게 한다. 시야 안의 실제 밝기는 레벨 조명과 손전등이 결정한다.
- 조도는 초기 Actor 표시·숨김 조건에 포함하지 않는다. 어두운 시야 안의 Actor는 엔진 조명 결과에 따라 어둡게 렌더링한다.
- 적 AI의 플레이어 감지는 기존 `AI Perception Component`의 책임으로 유지한다.

## Architecture

### UTDVisionComponent
- `ATDPlayerCharacter`는 생성자에서 `CreateDefaultSubobject`로 `UTDVisionComponent`를 생성한다.
- Tick을 사용하지 않는다(`PrimaryComponentTick.bCanEverTick = false`).
- `DrawDebugVision()`은 `ATDPlayerCharacter::Tick`에서 호출되며, `bDebugVision` 플래그로 제어된다.
- 디버그 표시는 `#if !UE_BUILD_SHIPPING` 가드 안에서만 실행된다.
- 외부 시스템이 필요할 때 `IsLocationInVision()` / `IsActorVisible()`을 직접 호출하는 조회형 구조다.

#### 변수

| 변수 | 기본값 | 설명 |
|---|---|---|
| `NearVisionRadius` | 600.f | 방향 무관 근거리 원형 시야 반경 |
| `ConeVisionDistance` | 1400.f | 전방 콘 최대 거리 |
| `ConeHalfAngleDeg` | 45.f | 전방 콘 반각 (도, 단방향) |
| `bUseLineOfSightCheck` | true | `IsActorVisible`에서 LineTrace 장애물 가림 검사 활성화 |
| `VisionTraceChannel` | `ECC_Visibility` | `IsActorVisible` LineTrace Collision Channel |
| `bDebugVision` | false | 디버그 표시 활성화 |

#### 판정 순서 (`IsLocationInVision`)

```text
1. Owner 위치 기준 2D 거리 제곱 계산 (DistSquared2D)
2. DistSq2D <= NearVisionRadius²  →  true  (방향 무관)
3. DistSq2D >  ConeVisionDistance²  →  false (콘 거리 초과)
4. ForwardVector 2D 추출, NearlyZero 가드
5. DotProduct(ForwardNorm2D, ToTargetNorm2D) >= cos(ConeHalfAngleDeg)  →  true
```

#### 판정 순서 (`IsActorVisible`)

```text
1. Owner, TargetActor 유효성 확인
2. IsLocationInVision(TargetActor 위치) 실패 → false
3. bUseLineOfSightCheck == false → true (LineTrace 생략)
4. Owner 위치 → TargetActor 위치 LineTrace (VisionTraceChannel)
5. Owner만 Ignore (FCollisionQueryParams::AddIgnoredActor)
6. Blocking Hit 없음 → true
7. 첫 Hit Actor == TargetActor → true
8. 다른 Actor가 먼저 맞음 → false
```

#### 디버그 표시 색상

| 요소 | 색상 | 조건 |
|---|---|---|
| NearVision 원 | Cyan | `DrawDebugVision()` 호출 시 |
| 콘 중심선 | Green | `DrawDebugVision()` 호출 시 |
| 콘 좌우 경계선 | Yellow | `DrawDebugVision()` 호출 시 |
| 콘 끝부분 호 (16 세그먼트) | Yellow | `DrawDebugVision()` 호출 시 |
| LOS Trace (가시) | Green Line | `IsActorVisible()` 호출 시, visible == true |
| LOS Trace (차단) | Red Line + Hit Point | `IsActorVisible()` 호출 시, 장애물에 가림 |

디버그 표시는 모두 `#if !UE_BUILD_SHIPPING` 가드 안에서만 실행된다.

---

### UTDActorVisibilityComponent
- `ATDPlayerCharacter`가 소유하며, BeginPlay에서 레벨 내 모든 `ATDEnemyCharacter`를 `GetAllActorsOfClass`로 한 번 수집한다.
- 같은 Owner의 `UTDVisionComponent`를 `FindComponentByClass`로 캐싱한다.
- `VisibilityUpdateInterval`(기본 0.15초) 주기의 Timer 하나로 수집된 Enemy를 일괄 갱신한다.
- BeginPlay에서 Timer 등록 후 `UpdateVisibility()`를 즉시 1회 호출해 초기 상태를 적용한다.
- 이전 가시 상태와 달라졌을 때만 `SetActorHiddenInGame`을 호출한다.
- Enemy는 `TWeakObjectPtr`로 보관하며, 갱신 시마다 유효하지 않은 항목을 목록에서 제거한다.
- `LastVisibilityState` 초기값은 `true`(레벨 배치 Actor의 기본 상태 = visible). 첫 갱신에서 시야 밖 Enemy가 즉시 숨겨진다.
- Tick을 사용하지 않는다(`PrimaryComponentTick.bCanEverTick = false`).

#### 변수

| 변수 | 기본값 | 설명 |
|---|---|---|
| `VisibilityUpdateInterval` | 0.15f | 가시성 갱신 주기 (초) |

#### 갱신 흐름

```text
BeginPlay
 ├─ VisionComponent 캐싱 (Owner->FindComponentByClass)
 ├─ GetAllActorsOfClass → TrackedEnemies 수집 (LastVisibilityState 초기값 true)
 ├─ Timer 등록 (VisibilityUpdateInterval, 반복)
 └─ UpdateVisibility() 즉시 1회 호출

UpdateVisibility (Timer 콜백)
 └─ TrackedEnemies 역순 순회
     ├─ 유효하지 않은 항목 → RemoveAtSwap (TrackedEnemies + LastVisibilityState 동기화)
     └─ IsActorVisible(Enemy) != LastVisibilityState[i]
          └─ SetActorHiddenInGame(!bVisible) + 상태 갱신
```

---

### 시스템 관계

```text
ATDPlayerCharacter
 ├─ UTDVisionComponent
 │   ├─ 위치 가시성 판정 (Near OR Cone) — IsLocationInVision()
 │   └─ Actor 가시성 판정 (위치 판정 + LineTrace LOS) — IsActorVisible()
 └─ UTDActorVisibilityComponent
     └─ 레벨 배치 Enemy 표시·숨김 (Timer 기반 주기 갱신)

FogOfWar / VisionRenderer  [미구현]
 ├─ Ray Fan + Visibility Polygon으로 벽 뒤 공간 차단
 ├─ RenderTarget + PostProcess로 시야 밖 공간을 어둡게 표현
 └─ 시야 안의 SceneColor는 유지하여 실제 조명 결과를 보존

Lighting / Flashlight  [미구현]
 ├─ 공간의 실제 밝기와 그림자 표현
 └─ 필요 시 대상 식별(Recognition) 보정값 제공

AI Perception
 └─ 적 AI의 플레이어 감지와 Alert 처리 (별도 책임)
```

### 시야·조명·식별 책임 분리

```text
Spatial Visibility
 ├─ InVisionShape
 └─ HasLineOfSight
          ↓
화면에 표시될 수 있는가?
          ↓
Lighting
 └─ Ambient / Local Light / Flashlight
          ↓
실제로 얼마나 밝게 보이는가?
          ↓
Recognition  [필요 시 추후 구현]
 └─ UI 표시, 조준 보정, 상호작용 또는 AI 탐지 보정
```

- `UTDVisionComponent`는 Spatial Visibility만 판정한다.
- `UTDActorVisibilityComponent`는 Spatial Visibility 결과만 동적 대상의 렌더링 상태에 적용한다.
- Lighting은 Unreal의 실제 광원과 그림자로 표현하며, Vision 판정이 SceneColor를 밝히지 않는다.
- 조도 기반 Recognition이 필요해질 때 별도 계층으로 추가한다. 초기 구현에서 조도 임계값으로 Actor를 즉시 숨기지 않는다.

### VisionRenderer 예정 방향

- 플레이어에서 여러 방향으로 Ray를 발사하여 각 방향의 최초 장애물 충돌점을 수집한다.
- 충돌점을 연결한 Visibility Polygon을 RenderTarget에 기록한다.
- PostProcess Material은 Visibility Polygon 바깥을 어둡게 처리한다.
- 시야 경계는 Material에서 부드럽게 보간하되, 벽 뒤 차단 경계가 과도하게 번지지 않게 조정한다.
- 공간 마스크와 Actor LOS는 동일한 `VisionTraceChannel` 및 장애물 규칙을 공유한다.
- Ray Fan은 매 프레임이 아닌 낮은 주기로 갱신하고, 화면 전환은 보간하는 방향을 우선 검토한다.

## Trade-offs
- `Owner->GetActorForwardVector()`는 AimTarget보다 플레이 의도를 덜 정확하게 반영하지만, 기존 Aim System과의 결합 없이 판정 로직을 먼저 안정화할 수 있다.
- NearVision은 후방의 가까운 대상도 인지하게 하므로 현실적인 시야보다 게임 플레이의 가독성을 우선한다.
- NearVision에도 LineTrace를 적용하면 벽 뒤 대상을 숨길 수 있지만, 대상 수가 많아질수록 Trace 비용이 증가한다.
- 조회형 구조는 불필요한 Tick을 피할 수 있지만, 표시 상태를 지속적으로 갱신할 별도 시스템이 필요하다.
- `GetAllActorsOfClass`는 BeginPlay에서 한 번만 실행하므로, 동적으로 Spawn된 적은 자동 추가되지 않는다. 고정 배치 스테이지 구성에서는 충분하다.
- 병렬 배열(`TrackedEnemies` + `LastVisibilityState`)은 `TMap`보다 캐시 효율이 좋으나, 두 배열이 항상 같은 인덱스를 유지해야 한다는 불변식이 있다. `RemoveAtSwap`은 항상 두 배열에 동시에 적용한다.
- 지형 렌더링과 Gameplay Entity 가시성을 분리하면 책임은 명확해지지만 최종 화면 구현에는 별도의 Fog 및 Actor Visibility 작업이 필요하다.
- 공간 마스크와 Actor LOS가 다른 Collision 규칙을 사용하면 어두운 공간에 Actor가 표시되거나 밝은 공간의 Actor가 숨는 불일치가 발생할 수 있다.
- Auto Exposure가 어두운 공간을 자동으로 밝힐 수 있으므로 조명 구현 시 노출 범위를 고정하거나 제한하는 튜닝이 필요하다.

## Future
- `Owner->GetActorForwardVector()` → AimTarget 기반 시야 방향 전환
- Ray Fan + Visibility Polygon 기반 벽 가림 마스크
- RenderTarget + PostProcess 기반 `VisionRenderer`
- 손전등 SpotLight 및 조도 기반 Recognition 보정
- 현재 가시 상태와 탐색 완료 상태의 분리
- `UTDActorVisibilityComponent`: 동적 Spawn 적 대응 (RegisterActor / UnregisterActor)
- `UTDActorVisibilityComponent`: 아이템, 탄피로 대상 확장
- 시야 밖 적의 Audio Presence 연동
- 필요 시 `Vision` 전용 Collision Channel 추가
- 관련 문서: [[flashlight-system]], [[enemy-alert-system]], [[enemy-system]], [[aim-system]]
