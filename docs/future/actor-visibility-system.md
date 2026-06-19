# Actor Visibility System

## Overview
`UTDVisionComponent`의 판정 결과를 바탕으로 적, 아이템, 탄피 같은 Gameplay Entity의 렌더링 상태를 관리하는 시스템이다.
시야를 직접 계산하지 않고, 이미 계산된 보임·안 보임 결과를 대상 Actor에 적용하는 역할만 담당한다.
현재는 구현 전 설계 단계이며, 아래 구조는 초기 구현 예정 방향을 정리한 것이다.

## Key Decisions
- 위치와 Actor의 가시성 판정은 `UTDVisionComponent`에 위임한다.
- 이 시스템은 판정 결과를 실제 표시·숨김 상태에 적용하는 책임만 가진다.
- 초기 적용 범위는 Actor의 렌더링 상태이며, Collision, 이동, 공격, AI 로직은 변경하지 않는다.
- 초기 구현에서는 매 프레임 Tick이나 Actor별 Timer 대신, Actor Visibility System이 소유하는 중앙 Timer 하나를 사용할 예정이다.
- 초기 갱신 간격은 `0.1~0.2초` 범위에서 시작하고 실제 플레이 감각과 대상 수를 기준으로 조정할 예정이다.
- 이전 가시 상태를 캐싱하고 상태가 달라졌을 때만 표시 상태를 변경하는 구조를 우선 사용한다.
- 첫 구현은 `ATDEnemyCharacter` 한 종류만 대상으로 검증한 뒤 아이템과 탄피로 확장할 예정이다.

## Architecture
- 초기 구현은 Actor Visibility System이 관리 대상으로 등록된 Actor를 보관하는 구조로 시작한다.
- Level에 미리 배치된 적은 시작 시 한 번 수집하고, 동적으로 Spawn된 적은 `RegisterActor()`로 추가하는 방식을 우선 사용한다.
- 중앙 Timer가 등록된 대상을 순회하며 `UTDVisionComponent::IsActorVisible()`을 호출할 예정이다.
- `IsActorVisible()`은 원형·콘 시야를 먼저 검사하고, 범위 밖이면 LineTrace 없이 즉시 false를 반환한다.
- 판정 결과가 캐싱된 상태와 다를 때만 `SetActorHiddenInGame()` 또는 Mesh의 `SetVisibility()`를 호출한다.
- 상태가 유지되면 렌더링 함수를 다시 호출하지 않는다.
- Actor가 파괴되거나 유효하지 않으면 관리 대상에서 안전하게 제거한다.
- 시야 밖 상태는 렌더링에만 영향을 주며 Gameplay 로직을 중단하지 않는다.
- 대상이 많아질 경우 플레이어 주변 대상만 조회하거나 등록·해제 범위를 제한한다.
- 시스템 관계는 다음과 같다.

```text
Actor Visibility System
 └─ Central Timer (0.1~0.2초)
      └─ 등록 Actor 순회
           └─ UTDVisionComponent::IsActorVisible(TargetActor)
                ├─ Vision 범위 밖 → LineTrace 생략 → false
                └─ Vision 범위 안 → LineTrace 가림 판정
                     ↓
                이전 상태와 비교
                     ├─ false → true  : 표시
                     ├─ true  → false : 숨김
                     └─ 상태 유지     : 변경 없음
```

## Trade-offs
- Timer 기반 갱신은 Tick보다 비용을 줄일 수 있지만, 갱신 간격에 따라 표시 전환이 늦게 보일 수 있다.
- `SetActorHiddenInGame()`은 간단하지만 Actor 전체에 영향을 주므로, 일부 컴포넌트만 유지해야 한다면 Mesh 단위 제어가 필요하다.
- 가시성과 Gameplay 활성 상태를 분리하면 구조가 안전하지만, 보이지 않는 적도 AI와 Collision 연산을 계속 수행한다.
- 모든 대상을 순회하는 방식은 초기 구현에는 단순하지만 대상 수가 증가하면 공간 조회 또는 등록 범위 최적화가 필요하다.
- 낮은 빈도의 중앙 Timer는 Actor별 Timer보다 관리와 부하 제어가 쉽지만, 한 번의 갱신 시점에 조회가 몰릴 수 있다.

## Future
- 아이템과 탄피로 관리 대상 확장
- 적 수와 Spawn 경로가 늘어나면 넓은 SphereOverlap으로 플레이어 주변 후보 목록을 자동 수집하는 구조 검토
- 후보 수집 반경은 `NearVisionRadius`와 `ConeVisionDistance` 중 큰 값에 여유 거리를 더해 설정
- 후보 수집은 `0.5~1초`, 가시성 판정은 `0.1~0.2초`처럼 서로 다른 갱신 주기로 분리 가능
- 후보 범위를 벗어난 Actor는 숨김 처리 후 관리 목록에서 제거
- SphereOverlap 전환 여부는 실제 동시 적 수와 Visibility/LineTrace 비용을 프로파일링한 뒤 결정
- 대상 수가 늘어날 경우 한 번의 Timer에서 처리할 Actor 수 분산
- 현재 보이지 않음과 한 번도 발견하지 않음을 구분하는 상태 추가
- Fade 또는 Dissolve 기반 표시 전환
- 시야 밖 적의 Audio Presence System 연동
- 관련 문서: [[vision-system]], [[enemy-audio-presence-system]], [[enemy-alert-system]], [[enemy-system]]
