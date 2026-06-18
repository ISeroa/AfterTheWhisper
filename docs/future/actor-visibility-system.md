# Actor Visibility System

## Overview
`UTDVisionComponent`의 판정 결과를 바탕으로 적, 아이템, 탄피 같은 Gameplay Entity의 렌더링 상태를 관리하는 시스템이다.
시야를 직접 계산하지 않고, 이미 계산된 보임·안 보임 결과를 대상 Actor에 적용하는 역할만 담당한다.

## Key Decisions
- 위치와 Actor의 가시성 판정은 `UTDVisionComponent`에 위임한다.
- 이 시스템은 판정 결과를 실제 표시·숨김 상태에 적용하는 책임만 가진다.
- 초기 적용 범위는 Actor의 렌더링 상태이며, Collision, 이동, 공격, AI 로직은 변경하지 않는다.
- 매 프레임 Tick 대신 낮은 빈도의 Timer 기반 갱신을 우선 검토한다.
- 이전 가시 상태를 캐싱하고 상태가 달라졌을 때만 표시 상태를 변경한다.
- 첫 구현은 `ATDEnemyCharacter` 한 종류만 대상으로 검증한 뒤 아이템과 탄피로 확장한다.

## Architecture
- Actor Visibility System은 관리 대상으로 등록된 Actor를 보관한다.
- 갱신 시 `UTDVisionComponent::IsActorVisible()`을 호출한다.
- 판정 결과가 캐싱된 상태와 다를 때만 `SetActorHiddenInGame()` 또는 Mesh의 `SetVisibility()`를 호출한다.
- Actor가 파괴되거나 유효하지 않으면 관리 대상에서 안전하게 제거한다.
- 시야 밖 상태는 렌더링에만 영향을 주며 Gameplay 로직을 중단하지 않는다.
- 대상이 많아질 경우 플레이어 주변 대상만 조회하거나 등록·해제 범위를 제한한다.
- 시스템 관계는 다음과 같다.

```text
UTDVisionComponent
 └─ IsActorVisible(TargetActor)
          ↓
Actor Visibility System
 ├─ Enemy 표시·숨김
 ├─ Item 표시·숨김
 └─ Casing 표시·숨김
```

## Trade-offs
- Timer 기반 갱신은 Tick보다 비용을 줄일 수 있지만, 갱신 간격에 따라 표시 전환이 늦게 보일 수 있다.
- `SetActorHiddenInGame()`은 간단하지만 Actor 전체에 영향을 주므로, 일부 컴포넌트만 유지해야 한다면 Mesh 단위 제어가 필요하다.
- 가시성과 Gameplay 활성 상태를 분리하면 구조가 안전하지만, 보이지 않는 적도 AI와 Collision 연산을 계속 수행한다.
- 모든 대상을 순회하는 방식은 초기 구현에는 단순하지만 대상 수가 증가하면 공간 조회 또는 등록 범위 최적화가 필요하다.

## Future
- 아이템과 탄피로 관리 대상 확장
- 플레이어 주변 대상만 관리하는 공간 기반 조회
- 현재 보이지 않음과 한 번도 발견하지 않음을 구분하는 상태 추가
- Fade 또는 Dissolve 기반 표시 전환
- 시야 밖 적의 Audio Presence System 연동
- 관련 문서: [[vision-system]], [[enemy-audio-presence-system]], [[enemy-alert-system]], [[enemy-system]]
