# Vision System

## Current Implementation Snapshot (2026-06-23)

전장의 안개 1차 구현은 현재 `RenderTarget + PostProcess Material` 방식으로 동작한다.

- `UTDVisionComponent`는 시야 규칙의 기준 데이터를 가진다.
  - `NearVisionRadius`
  - `ConeVisionDistance`
  - `ConeHalfAngleDeg`
  - `bUseLineOfSightCheck`
  - `VisionTraceChannel`
- `UTDActorVisibilityComponent`는 `UTDVisionComponent::IsActorVisible()`을 사용해 Enemy 같은 동적 대상을 숨기거나 표시한다.
- `UTDVisionRendererComponent`는 `UTDVisionComponent`의 설정을 읽어 벽에 막힌 `VisibilityPolygonPoints`를 계산한다.
- `UTDVisionRendererComponent::DrawToRenderTarget()`는 계산된 폴리곤을 `VisionMaskRenderTarget`에 흰색 삼각형 Fan으로 그린다.
- `M_PP_VisionDarkness` / `MI_PP_VisionDarkness`는 `RT_VisionMask`를 읽어서 화면을 어둡게 처리한다.
  - Mask black: 시야 밖, 어둡게 표시
  - Mask white: 시야 안, 원래 화면 유지
- `PostProcessVolume`에 `MI_PP_VisionDarkness`를 등록하고 `Infinite Extent (Unbound)`를 켜서 테스트 맵 전체에 적용한다.

현재 구현은 “기억된 탐색 영역”을 사용하지 않는다. 정적 환경은 시야 밖이어도 어둡게 보이고, 동적 대상은 별도 Actor Visibility 시스템으로 현재 시야 밖에서 숨기는 방향이다.

### Render Target Flow

```text
UTDVisionRendererComponent::UpdatePolygon
 ├─ UTDVisionComponent 설정 읽기
 ├─ Ray Fan LineTrace (VisionTraceChannel)
 ├─ VisibilityPolygonPoints 갱신
 └─ DrawToRenderTarget
     ├─ RT_VisionMask 검정 Clear
     ├─ Owner / Polygon Points를 화면 좌표로 투영
     ├─ RenderTarget 크기에 맞게 좌표 스케일링
     └─ 흰색 Triangle Fan으로 현재 시야 영역 그리기

PostProcess Material
 ├─ SceneTexture(PostProcessInput0)
 ├─ TextureSampleParameter2D(VisionMask = RT_VisionMask)
 └─ FinalColor = Lerp(SceneColor * DarknessAmount, SceneColor, VisionMask.R)
```

### Current Tuning Notes

- `UpdateInterval = 0.1f`는 움직일 때 마스크가 뚝뚝 끊겨 보일 수 있다.
- 테스트 결과 `UpdateInterval = 0.033f` 쪽이 체감상 더 자연스럽다.
- `RayCount`는 우선 128을 유지한다.
  - 시간상 끊김은 `UpdateInterval` 문제일 가능성이 크다.
  - 경계선 모양이 각져 보일 때만 `RayCount`를 올린다.
- 다음 최적화 후보는 “플레이어 위치/회전이 일정 이상 변했을 때만 갱신”하는 방식이다.
- 현재 마스크 경계는 하드 엣지다. 다음 품질 개선 작업에서 Feather/Blur를 고려한다.

## Overview
플레이어를 기준으로 특정 위치나 Actor가 현재 시야 안에 있는지 판정하고, 그 결과로 Enemy의 표시·숨김을 제어하는 시스템이다.
근거리 원형(NearVision) + 전방 콘(ConeVision) 판정, 장애물 LineTrace, Actor 단위 가시성 조회(`IsActorVisible`), 고정 배치 Enemy의 주기적 표시·숨김(`UTDActorVisibilityComponent`)까지 구현되어 있다.
전장의 안개 표현을 위한 첫 단계로 `UTDVisionRendererComponent`의 Ray Fan 기반 Visibility Polygon 계산과 디버그 표시가 추가되었으며, RenderTarget과 PostProcess 화면 표현은 아직 미구현 상태다.

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
- 시야를 가리는 대상은 전용 Trace Channel인 `VisionObstacle`로 구분한다.
- `VisionObstacle`의 기본 반응은 Ignore이며, 벽과 기둥처럼 시야를 실제로 막는 대상만 `VisionOccluder` Collision Preset으로 Block한다.
- Enemy, Player, 탄피, 아이템 같은 동적 대상은 `VisionObstacle`을 Ignore하여 Visibility Polygon을 자르지 않게 한다.
- 고정 배치 Enemy의 표시·숨김은 `UTDActorVisibilityComponent`가 담당한다. 아이템·탄피로의 확장은 미구현이다.
- 맵 지형은 기본적으로 렌더링하며, Fog 또는 Darkness 표현은 별도 렌더링 시스템의 책임으로 둔다.
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
| `VisionTraceChannel` | C++ 기본값 `ECC_Visibility` | BP에서 `VisionObstacle`로 설정하여 Actor LOS와 Visibility Polygon이 동일한 차단 규칙을 공유 |
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

### UTDVisionRendererComponent
- `ATDPlayerCharacter`가 `CreateDefaultSubobject`로 소유한다.
- Tick을 사용하지 않고 `UpdateInterval` 주기의 중앙 Timer로 Visibility Polygon을 갱신한다.
- 같은 Owner의 `UTDVisionComponent`를 BeginPlay에서 찾아 캐싱하고, 컴포넌트가 없으면 Warning 로그 후 갱신을 시작하지 않는다.
- 360도를 `RayCount`만큼 균등 샘플링하고 콘 좌우 경계 Ray 2개를 추가한다.
- 전방 콘 안의 Ray는 `Max(NearVisionRadius, ConeVisionDistance)`, 콘 밖의 Ray는 `NearVisionRadius`를 최대 거리로 사용한다.
- `bUseLineOfSightCheck`가 활성화된 경우 `UTDVisionComponent::VisionTraceChannel`로 LineTrace하여 최초 Blocking Hit 지점에서 폴리곤을 자른다.
- 계산된 지점은 각도순 `VisibilityPolygonPoints`에 저장하며, RenderTarget 구현에서 재사용할 수 있도록 읽기 전용 Getter를 제공한다.
- 현재 단계에서는 `bDebugVisionPolygon`이 활성화된 경우 인접 지점을 Orange Debug Line으로 연결하는 기능만 구현되어 있다.

#### 변수

| 변수 | 기본값 | 설명 |
|---|---|---|
| `UpdateInterval` | 0.1f | Visibility Polygon 갱신 주기 (초) |
| `RayCount` | 128 | 360도 균등 샘플링 Ray 수. 실제 지점에는 콘 경계 2개가 추가됨 |
| `bDebugVisionPolygon` | false | Orange Debug Line 표시 활성화 |

#### 갱신 흐름

```text
BeginPlay
 ├─ 같은 Owner의 UTDVisionComponent 캐싱
 └─ Timer 등록 (UpdateInterval, 반복)

UpdatePolygon
 ├─ 360도 균등 각도 + 콘 좌우 경계 각도 생성 및 정렬
 ├─ 각 방향이 전방 콘 안인지 판정
 ├─ Near 또는 Cone 최대 거리 결정
 ├─ VisionTraceChannel LineTrace
 │   ├─ Blocking Hit → ImpactPoint 저장
 │   └─ Hit 없음      → 최대 거리 지점 저장
 └─ Debug 활성화 시 인접 지점을 연결해 닫힌 폴리곤 표시
```

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
 ├─ UTDVisionRendererComponent
 │   └─ Ray Fan + VisionObstacle Trace → Visibility Polygon 계산 및 Debug 표시
 └─ UTDActorVisibilityComponent
     └─ 레벨 배치 Enemy 표시·숨김 (Timer 기반 주기 갱신)

FogOfWar 화면 표현  [미구현]
 └─ Visibility Polygon → RenderTarget Mask → PostProcess Darkness

AI Perception
 └─ 적 AI의 플레이어 감지와 Alert 처리 (별도 책임)
```

## Trade-offs
- `Owner->GetActorForwardVector()`는 AimTarget보다 플레이 의도를 덜 정확하게 반영하지만, 기존 Aim System과의 결합 없이 판정 로직을 먼저 안정화할 수 있다.
- NearVision은 후방의 가까운 대상도 인지하게 하므로 현실적인 시야보다 게임 플레이의 가독성을 우선한다.
- NearVision에도 LineTrace를 적용하면 벽 뒤 대상을 숨길 수 있지만, 대상 수가 많아질수록 Trace 비용이 증가한다.
- 조회형 구조는 불필요한 Tick을 피할 수 있지만, 표시 상태를 지속적으로 갱신할 별도 시스템이 필요하다.
- `GetAllActorsOfClass`는 BeginPlay에서 한 번만 실행하므로, 동적으로 Spawn된 적은 자동 추가되지 않는다. 고정 배치 스테이지 구성에서는 충분하다.
- 병렬 배열(`TrackedEnemies` + `LastVisibilityState`)은 `TMap`보다 캐시 효율이 좋으나, 두 배열이 항상 같은 인덱스를 유지해야 한다는 불변식이 있다. `RemoveAtSwap`은 항상 두 배열에 동시에 적용한다.
- 지형 렌더링과 Gameplay Entity 가시성을 분리하면 책임은 명확해지지만 최종 화면 구현에는 별도의 Fog 및 Actor Visibility 작업이 필요하다.
- Ray Fan은 고정 개수의 Trace를 주기적으로 실행하므로 `RayCount`와 `UpdateInterval`은 실제 맵 복잡도에서 프로파일링해야 한다.
- `VisionObstacle`은 물체의 Static/Dynamic 여부가 아니라 게임적으로 시야를 막는지에 따라 Block/Ignore를 설정한다. 닫힌 문처럼 동적이지만 시야를 막는 대상은 Block이 필요하다.
- Actor LOS와 Visibility Polygon이 서로 다른 Trace Channel을 사용하면 적 표시와 화면 마스크가 어긋날 수 있으므로 같은 `VisionTraceChannel`을 공유한다.

## Future
- `Owner->GetActorForwardVector()` → AimTarget 기반 시야 방향 전환
- 손전등 On/Off 및 시야 거리·각도 보정
- Visibility Polygon을 RenderTarget Mask에 기록
- PostProcess Material로 시야 밖 SceneColor 어둡게 처리
- 현재 가시 상태와 탐색 완료 상태의 분리
- `UTDActorVisibilityComponent`: 동적 Spawn 적 대응 (RegisterActor / UnregisterActor)
- `UTDActorVisibilityComponent`: 아이템, 탄피로 대상 확장
- 시야 밖 적의 Audio Presence 연동
- `VisionOccluder` Preset을 벽·기둥용 공통 Blueprint 또는 모듈형 벽에 적용
- 관련 문서: [[flashlight-system]], [[enemy-alert-system]], [[enemy-system]], [[aim-system]]
