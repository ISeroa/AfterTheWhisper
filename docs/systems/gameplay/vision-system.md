# Vision System

## Overview
플레이어를 기준으로 특정 위치나 Actor가 현재 시야 안에 있는지 판정하는 시스템이다.
현재 구현은 근거리 원형(NearVision) + 전방 콘(ConeVision) 판정과 디버그 표시까지 완료된 상태다.
장애물 가림(LineTrace)과 Actor 단위 조회는 후속 구현 범위다.

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
- LineTrace가 추가되면 NearVision과 ConeVision 모두 동일한 장애물 가림 규칙을 적용한다.
- 맵 지형은 기본적으로 렌더링하며, Fog 또는 Darkness 표현은 별도 렌더링 시스템의 책임으로 둔다.
- 적, 아이템, 탄피의 실제 표시·숨김은 별도의 Actor Visibility System이 담당한다.
- 적 AI의 플레이어 감지는 기존 `AI Perception Component`의 책임으로 유지한다.

## Architecture
- `ATDPlayerCharacter`는 생성자에서 `CreateDefaultSubobject`로 `UTDVisionComponent`를 생성한다.
- `UTDVisionComponent`는 Tick을 사용하지 않는다(`PrimaryComponentTick.bCanEverTick = false`).
- `DrawDebugVision()`은 `ATDPlayerCharacter::Tick`에서 호출되며, `bDebugVision` 플래그로 제어된다.
- 디버그 표시는 `#if !UE_BUILD_SHIPPING` 가드 안에서만 실행된다.
- 코어 컴포넌트는 외부 시스템이 필요할 때 `IsLocationInVision()`을 호출하는 조회형 구조다.

### 변수

| 변수 | 기본값 | 설명 |
|---|---|---|
| `NearVisionRadius` | 600.f | 방향 무관 근거리 원형 시야 반경 |
| `ConeVisionDistance` | 1400.f | 전방 콘 최대 거리 |
| `ConeHalfAngleDeg` | 45.f | 전방 콘 반각 (도, 단방향) |
| `bDebugVision` | false | 디버그 표시 활성화 |

### 판정 순서 (`IsLocationInVision`)

```text
1. Owner 위치 기준 2D 거리 제곱 계산 (DistSquared2D)
2. DistSq2D <= NearVisionRadius²  →  true  (방향 무관)
3. DistSq2D >  ConeVisionDistance²  →  false (콘 거리 초과)
4. ForwardVector 2D 추출, NearlyZero 가드
5. DotProduct(ForwardNorm2D, ToTargetNorm2D) >= cos(ConeHalfAngleDeg)  →  true
```

### 디버그 표시 색상

| 요소 | 색상 |
|---|---|
| NearVision 원 | Cyan |
| 콘 중심선 | Green |
| 콘 좌우 경계선 | Yellow |
| 콘 끝부분 호 (16 세그먼트) | Yellow |

### 시스템 관계

```text
ATDPlayerCharacter
 └─ UTDVisionComponent
     └─ 위치의 가시성 판정 (Near OR Cone)

Actor Visibility System  [미구현]
 └─ 판정 결과를 적/아이템/탄피의 렌더링 상태에 적용

FogOfWar / VisionRenderer  [미구현]
 └─ 맵의 어둠, 마스크, PostProcess 표현

AI Perception
 └─ 적 AI의 플레이어 감지와 Alert 처리 (별도 책임)
```

## Trade-offs
- `Owner->GetActorForwardVector()`는 AimTarget보다 플레이 의도를 덜 정확하게 반영하지만, 기존 Aim System과의 결합 없이 판정 로직을 먼저 안정화할 수 있다.
- NearVision은 후방의 가까운 대상도 인지하게 하므로 현실적인 시야보다 게임 플레이의 가독성을 우선한다.
- NearVision에도 LineTrace를 적용하면 벽 뒤 대상을 숨길 수 있지만, 대상 수가 많아질수록 Trace 비용이 증가한다.
- 조회형 구조는 불필요한 Tick을 피할 수 있지만, 표시 상태를 지속적으로 갱신할 별도 시스템이 필요하다.
- 지형 렌더링과 Gameplay Entity 가시성을 분리하면 책임은 명확해지지만 최종 화면 구현에는 별도의 Fog 및 Actor Visibility 작업이 필요하다.

## Future
- `IsActorVisible(const AActor*)` 추가 — Actor 위치 기반으로 `IsLocationInVision()` 재사용
- 장애물 LineTrace (`bUseLineOfSightCheck`, `VisionTraceChannel = ECC_Visibility`)
- `Owner->GetActorForwardVector()` → AimTarget 기반 시야 방향 전환
- 손전등 On/Off 및 시야 거리·각도 보정
- RenderTarget 또는 PostProcess 기반 Fog of War
- 현재 가시 상태와 탐색 완료 상태의 분리
- Actor Visibility System을 통한 적, 아이템, 탄피 표시·숨김
- 시야 밖 적의 Audio Presence 연동
- 필요 시 `Vision` 전용 Collision Channel 추가
- 관련 문서: [[actor-visibility-system]], [[flashlight-system]], [[enemy-alert-system]], [[enemy-system]], [[aim-system]]
