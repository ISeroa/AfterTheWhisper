# Vision System

## Overview
플레이어를 기준으로 특정 위치나 Actor가 현재 시야 안에 있는지 판정하는 시스템이다.
현재 구현은 원형 반경 판정과 디버그 표시까지 완료되었으며, 전방 콘과 장애물 가림은 후속 구현 범위다.

## Key Decisions
- 시야 판정은 `ATDPlayerCharacter`가 소유하는 `UTDVisionComponent`에서 담당한다.
- `UTDVisionComponent`는 판정 결과만 제공하며, Actor의 Mesh 표시·숨김을 직접 제어하지 않는다.
- 현재 원형 판정은 `VisionRadius`와 `IsLocationInVision()`을 사용한다.
- 후속 구조에서는 원형 반경을 `NearVisionRadius` 책임으로 확장하고, 근거리 밖에 전방 콘 판정을 추가한다.
- 초기 전방 시야 방향은 `Owner->GetActorForwardVector()`를 사용하며, AimTarget 연동은 코어 판정 검증 후 진행한다.
- 거리와 각도는 높이 차이로 인한 판정 흔들림을 줄이기 위해 2D 기준으로 계산한다.
- LineTrace가 추가되면 근거리와 전방 콘 모두 동일한 장애물 가림 규칙을 적용한다.
- 맵 지형은 기본적으로 렌더링하며, Fog 또는 Darkness 표현은 별도 렌더링 시스템의 책임으로 둔다.
- 적, 아이템, 탄피의 실제 표시·숨김은 별도의 Actor Visibility System이 담당한다.
- 적 AI의 플레이어 감지는 기존 `AI Perception Component`의 책임으로 유지한다.

## Architecture
- `ATDPlayerCharacter`는 `UTDVisionComponent`를 생성해 소유한다.
- 현재 `IsLocationInVision()`은 Owner와 대상 위치의 2D 거리 제곱을 `VisionRadius` 제곱과 비교한다.
- 현재 `DrawDebugVision()`은 `bDebugVision`이 활성화되면 원형 경계를 표시한다.
- 디버그 함수는 Player Tick에서 호출되지만 실제 Draw는 Debug Flag가 켜진 경우에만 수행한다.
- 후속 `IsLocationVisible()`은 Near Vision, 전방 콘, LineTrace를 순서대로 조합한다.
- 후속 `IsActorVisible()`은 Actor 위치를 사용해 위치 판정을 재사용한다.
- 코어 컴포넌트는 Tick을 사용하지 않고, 외부 시스템이 필요할 때 조회하는 구조로 유지한다.
- 구현은 원형 시야, 전방 콘, LineTrace, Actor 조회 순서로 작은 단위로 검증한다.
- 디버그 표시는 Shipping 빌드에서 제외한다.
- 시스템 관계는 다음과 같다.

```text
ATDPlayerCharacter
 └─ UTDVisionComponent
     └─ 위치/Actor의 가시성 판정

Actor Visibility System
 └─ 판정 결과를 적/아이템/탄피의 렌더링 상태에 적용

FogOfWar / VisionRenderer
 └─ 맵의 어둠, 마스크, PostProcess 표현

AI Perception
 └─ 적 AI의 플레이어 감지와 Alert 처리
```

## Trade-offs
- `Owner->GetActorForwardVector()`는 AimTarget보다 플레이 의도를 덜 정확하게 반영하지만, 기존 Aim System과의 결합 없이 판정 로직을 먼저 안정화할 수 있다.
- Near Vision은 후방의 가까운 대상도 인지하게 하므로 현실적인 시야보다 게임 플레이의 가독성을 우선한다.
- Near Vision에도 LineTrace를 적용하면 벽 뒤 대상을 숨길 수 있지만, 대상 수가 많아질수록 Trace 비용이 증가한다.
- 조회형 구조는 불필요한 Tick을 피할 수 있지만, 표시 상태를 지속적으로 갱신할 별도 시스템이 필요하다.
- 지형 렌더링과 Gameplay Entity 가시성을 분리하면 책임은 명확해지지만 최종 화면 구현에는 별도의 Fog 및 Actor Visibility 작업이 필요하다.

## Future
- `VisionRadius`와 `IsLocationInVision()`을 Near Vision·최종 Visibility 구조로 확장
- `Owner->GetActorForwardVector()` 기반 전방 콘 판정
- 장애물 LineTrace와 `IsActorVisible()`
- AimTarget 기반 시야 방향 전환
- 손전등 On/Off 및 시야 거리·각도 보정
- RenderTarget 또는 PostProcess 기반 Fog of War
- 현재 가시 상태와 탐색 완료 상태의 분리
- Actor Visibility System을 통한 적, 아이템, 탄피 표시·숨김
- 시야 밖 적의 Audio Presence 연동
- 필요 시 `Vision` 전용 Collision Channel 추가
- 관련 문서: [[actor-visibility-system]], [[flashlight-system]], [[enemy-alert-system]], [[enemy-system]], [[aim-system]]
