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
- 관리 대상과 이전 가시 상태는 하나의 Entry로 묶어 `TArray`에 보관할 예정이다.
- 가시성 갱신은 관리 대상 전체 순회가 필수이므로, 단일 Actor 검색에 유리한 `TSet`이나 `TMap`보다 연속 메모리 순회에 유리한 `TArray`를 우선 사용한다.
- 첫 구현은 `ATDEnemyCharacter` 한 종류만 대상으로 검증한 뒤 아이템과 탄피로 확장할 예정이다.
- Actor 표시 조건은 Spatial Visibility(`InVisionShape && HasLineOfSight`)만 사용한다.
- 조명은 표시된 Actor가 얼마나 밝게 렌더링되는지를 담당하며, 초기 구현에서는 조도 임계값으로 Actor를 숨기지 않는다.

## Architecture
- 초기 구현은 Actor Visibility System이 관리 대상으로 등록된 Actor를 보관하는 구조로 시작한다.
- Level에 미리 배치된 적은 시작 시 한 번 수집하고, 동적으로 Spawn된 적은 `RegisterActor()`로 추가하는 방식을 우선 사용한다.
- 각 Entry는 `TWeakObjectPtr<ATDEnemyCharacter>`, 이전 가시 상태, 첫 판정 여부를 함께 보관한다.
- `TWeakObjectPtr`를 사용하여 Destroy된 Actor를 안전하게 감지하고, 역순 순회와 `RemoveAtSwap()`으로 유효하지 않은 Entry를 제거한다.
- 중앙 Timer가 등록된 대상을 순회하며 `UTDVisionComponent::IsActorVisible()`을 호출할 예정이다.
- `IsActorVisible()`은 원형·콘 시야를 먼저 검사하고, 범위 밖이면 LineTrace 없이 즉시 false를 반환한다.
- 판정 결과가 캐싱된 상태와 다를 때만 `SetActorHiddenInGame()` 또는 Mesh의 `SetVisibility()`를 호출한다.
- 상태가 유지되면 렌더링 함수를 다시 호출하지 않는다.
- Actor가 파괴되거나 유효하지 않으면 관리 대상에서 안전하게 제거한다.
- 시야 밖 상태는 렌더링에만 영향을 주며 Gameplay 로직을 중단하지 않는다.
- 시야 안이지만 어두운 Actor는 숨김 처리하지 않고 Unreal 조명 결과에 따라 어둡게 렌더링한다.
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
- `TArray`는 전체 순회에 적합하지만 특정 Actor의 등록 여부 확인과 제거 대상 검색은 O(N)이다. 초기 적 규모와 낮은 등록·해제 빈도에서는 단순성과 순회 효율을 우선한다.
- 적 수가 늘어났을 때 `TMap`이나 `TSet`으로 바꾸더라도 전체 가시성 순회 횟수는 줄지 않으므로, Container 변경보다 후보 대상 수를 줄이는 방식을 우선한다.
- 조도를 Boolean 표시 조건으로 사용하면 광원 경계에서 Actor가 갑자기 나타날 수 있다. 식별 판정이 필요하면 렌더링 가시성과 별도 Recognition 상태로 관리한다.
- `SetActorHiddenInGame()`은 Actor 전체와 그림자 표시에 영향을 줄 수 있으므로, 최종 조명 연출에서 그림자 동작을 확인해야 한다.

## Future
- 아이템과 탄피로 관리 대상 확장
- 적 수와 Spawn 경로가 늘어나면 Broad Phase와 Narrow Phase로 조회 단계를 분리하는 구조 검토
- Broad Phase는 넓은 SphereOverlap으로 플레이어 주변 Enemy만 수집하여 `TArray` 후보 목록을 갱신
- Narrow Phase는 후보 `TArray`만 순회하며 `IsActorVisible()`과 LineTrace를 수행
- 후보 수집 반경은 `NearVisionRadius`와 `ConeVisionDistance` 중 큰 값에 여유 거리를 더해 설정
- 후보 수집은 `0.5~1초`, 가시성 판정은 `0.1~0.2초`처럼 서로 다른 갱신 주기로 분리 가능
- 후보 범위를 벗어난 Actor는 숨김 처리 후 관리 목록에서 제거
- SphereOverlap 전환 여부는 실제 동시 적 수와 Visibility/LineTrace 비용을 프로파일링한 뒤 결정
- 대상 수가 늘어날 경우 한 번의 Timer에서 처리할 Actor 수 분산
- 현재 보이지 않음과 한 번도 발견하지 않음을 구분하는 상태 추가
- Fade 또는 Dissolve 기반 표시 전환
- 시야 밖 적의 Audio Presence System 연동
- 관련 문서: [[vision-system]], [[enemy-audio-presence-system]], [[enemy-alert-system]], [[enemy-system]]
