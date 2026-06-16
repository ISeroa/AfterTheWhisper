# TD Vision System Design

## Overview

이 문서는 TopdownShooter 프로젝트의 플레이어 시야 판정 시스템 설계 기준을 정리한다.

현재 목표는 실제 Fog of War 렌더링, RenderTarget, PostProcess 구현이 아니라, 플레이어 기준으로 특정 위치나 액터가 현재 보이는지 판정하는 코어 규칙을 먼저 안정적으로 정의하는 것이다.

`UTDVisionComponent`는 초기 단계에서 “화면을 어둡게 그리는 시스템”이 아니라, 적/아이템/탄피 같은 엔티티가 플레이어 시야 안에 들어왔는지 판단하기 위한 조회형 컴포넌트로 시작한다.

---

## Key Decisions

### 1. VisionComponent는 판정 전용으로 시작한다

`UTDVisionComponent`는 최종 시야 시스템 전체를 담당하는 만능 컴포넌트가 아니다.

이 컴포넌트의 1차 책임은 다음 질문에 답하는 것이다.

> 플레이어 기준으로 이 위치 또는 액터가 현재 보이는가?

따라서 초기 구현 범위는 다음으로 제한한다.

- 근거리 원형 시야 판정
- 전방 콘 시야 판정
- 벽/장애물 가림 LineTrace 판정
- 개발용 디버그 시각화
- `IsLocationVisible()` / `IsActorVisible()` 조회 함수 제공

다음 기능들은 `UTDVisionComponent`가 직접 담당하지 않는다.

- 손전등 배터리 소모
- 손전등 On/Off 입력 처리
- 손전등 사운드 / 이펙트
- 실제 Fog of War 렌더링
- RenderTarget 갱신
- PostProcess / Material 마스킹
- 적 AI Alert 상승 처리
- 적 AI의 시각 / 청각 감지
- 시야 밖 적의 사운드 존재감 처리
- 아이템 UI 표시 / 숨김 처리

### 2. 맵 지형지물과 엔티티 가시성은 분리한다

맵에 배치된 벽, 바닥, 큰 지형지물은 기본적으로 화면에 보이는 대상으로 둔다.

Fog of War 또는 어둠 표현은 지형을 완전히 숨기는 방식이 아니라, 안개나 어둠을 씌운 것처럼 표현하는 방향을 우선 고려한다.

반면 적, 아이템, 탄피 같은 엔티티는 `UTDVisionComponent`의 판정 결과를 바탕으로 표시/숨김을 결정한다.

정리하면 다음과 같다.

```text
Map Geometry
- 기본적으로 화면에 존재한다.
- Fog / Darkness / PostProcess 계열 시스템에서 어둡게 표현한다.
- VisionComponent가 직접 표시/숨김을 제어하지 않는다.

Gameplay Entities
- Enemy, Item, Casing 등.
- VisionComponent 판정 결과를 바탕으로 별도 ActorVisibility 계열 시스템이 표시/숨김을 결정한다.
```

### 3. 초기 전방 시야 방향은 Owner ForwardVector 기준으로 한다

최종 목표는 마우스 AimTarget 기준 시야 방향이다.

하지만 초기 구현에서는 플레이어 Aim 시스템과의 연결 범위를 늘리지 않기 위해, 전방 콘 시야 방향을 `Owner->GetActorForwardVector()` 기준으로 계산한다.

AimTarget 기반 방향 전환은 `UTDVisionComponent`의 원형/콘/LineTrace 판정이 안정화된 뒤 별도 단계에서 진행한다.

이 결정은 1차 구현 범위를 시야 판정 컴포넌트 내부로 제한하고, 기존 플레이어 Aim 로직을 불필요하게 건드리지 않기 위한 것이다.

### 4. NearVision은 방향과 무관한 기본 시야다

`NearVisionRadius` 안에 있는 위치는 전방 콘 각도와 무관하게 visible 후보로 처리한다.

`NearVisionRadius` 밖에 있는 위치만 전방 콘 시야를 검사한다.

즉, 기본 판정 관계는 다음과 같다.

```text
NearVision 통과 OR ConeVision 통과 → visible 후보
visible 후보 AND LineTrace 가림 없음 → 최종 visible
```

NearVision은 “콘 방향을 보지 않아도 가까운 것은 감지된다”는 게임플레이 규칙이다.

단, `bUseLineOfSightCheck`가 켜져 있는 경우에는 NearVision으로 visible 후보가 되었더라도 벽/장애물 가림 검사는 동일하게 적용한다.

이 규칙은 가까운 적/아이템이 플레이어 뒤에 있어도 보이게 하되, 벽 뒤 엔티티까지 무조건 보이는 문제를 막기 위한 것이다.

### 5. LineTrace는 엔티티 가시성의 가림 판정에 사용한다

LineTrace는 맵 지형을 숨기기 위한 기능이 아니다.

초기 LineTrace는 적/아이템/탄피 같은 엔티티가 플레이어 시야 판정에는 들어왔지만, 벽이나 큰 장애물 뒤에 있는 경우 최종적으로 hidden 처리하기 위한 가림 판정으로 사용한다.

초기 구현의 `VisionTraceChannel` 기본값은 `ECC_Visibility`로 둔다.

LineTrace는 Owner Actor를 Ignore한다.

별도 `Vision` 전용 Trace Channel은 1차 구현 범위에 포함하지 않는다. 필요성이 확인되면 후속 단계에서 프로젝트 Collision 설정에 추가한다.

---

## Architecture

### 시스템 역할 구분

```text
PlayerCharacter
 ├─ UTDVisionComponent
 │   └─ 플레이어 기준 시야 판정 코어
 │
 ├─ UTDFlashlightComponent
 │   └─ 손전등 상태, 배터리, 시야 보너스 제공
 │
 ├─ FogOfWar / VisionRenderer 계열 시스템
 │   └─ 실제 화면 어둠, 마스크, RenderTarget, PostProcess 처리
 │
 ├─ EnemyVisibility / ActorVisibility 계열 시스템
 │   └─ VisionComponent 판정 결과를 바탕으로 적/아이템/탄피 표시 여부 결정
 │
 └─ AudioPresence 계열 시스템
     └─ 시야 밖 적의 존재감을 소리로 전달
```

### VisionComponent가 담당한다

- `NearVisionRadius`
- `ConeVisionDistance`
- `ConeHalfAngleDeg`
- 현재 시야 방향 계산
- 위치 기반 시야 판정
- 액터 기반 시야 판정
- 벽/장애물 가림 체크
- 개발용 디버그 드로우

### VisionComponent가 직접 담당하지 않는다

- 손전등 배터리 / 입력 / 사운드 / 이펙트
- 실제 Fog of War 렌더링
- RenderTarget / PostProcess / Material 마스킹
- 적 AI Alert 상승
- 적 AI의 시각 / 청각 감지
- 적/아이템/탄피 Mesh 표시/숨김의 최종 적용
- 시야 밖 적의 오디오 존재감 처리

`UTDVisionComponent`는 “판정 결과”를 제공하고, 실제 표시/숨김 적용은 별도 시스템이 담당한다.

---

## Visibility Rules

### 판정 순서

`IsLocationVisible()`은 다음 순서로 판정한다.

1. Owner와 Target 위치를 2D 기준으로 비교한다.
2. `NearVisionRadius` 안이면 방향과 무관하게 visible 후보로 처리한다.
3. `NearVisionRadius` 밖이면 전방 콘 시야를 검사한다.
4. 콘 시야는 `ConeVisionDistance`와 `ConeHalfAngleDeg`를 모두 만족해야 visible 후보가 된다.
5. visible 후보가 아니면 즉시 false를 반환한다.
6. visible 후보인 경우, `bUseLineOfSightCheck`가 true면 LineTrace로 벽/장애물 가림을 검사한다.
7. LineTrace가 막히지 않았을 때만 최종 true를 반환한다.

### 2D 기준 판정

탑다운 슈터 특성상 거리와 각도 판정은 기본적으로 2D 기준으로 처리한다.

Z 차이는 초기 시야 판정의 핵심 요소로 사용하지 않는다.

이는 캡슐 높이, 바닥 높이 차이, 계단, 피벗 위치 차이 때문에 시야 판정이 흔들리는 문제를 줄이기 위한 것이다.

### Tick 정책

`UTDVisionComponent`는 초기 구현에서 Tick을 사용하지 않는다.

이 컴포넌트는 매 프레임 적/아이템/탄피를 직접 순회하지 않고, 외부 시스템이 필요할 때 `IsLocationVisible()` 또는 `IsActorVisible()`을 호출하는 조회형 컴포넌트로 시작한다.

주기적인 표시/숨김 갱신이 필요해지는 경우, 별도의 `ActorVisibility` 계열 시스템에서 낮은 빈도의 Timer 기반 갱신을 검토한다.

---

## AI Perception과의 구분

Unreal `AI Perception Component`는 주로 AI가 플레이어를 감지하는 데 사용한다.

예시:

- 좀비가 플레이어를 봤는가?
- 생존자가 총소리를 들었는가?
- 적의 Alert 상태를 올릴 것인가?

반면 `UTDVisionComponent`는 플레이어 화면/탐사 시야를 위한 판정 기준이다.

예시:

- 이 적이 플레이어 시야 안에 있는가?
- 이 아이템이 어둠 속에 가려져야 하는가?
- 벽 뒤 엔티티를 아직 숨겨야 하는가?
- 근거리 원형 시야 또는 전방 콘 시야 안에 있는가?

정리하면 다음과 같다.

```text
AI Perception
- 적의 눈과 귀
- AIController 또는 Enemy 쪽 책임
- 감지 이벤트와 Alert 상태에 사용

UTDVisionComponent
- 플레이어 화면의 눈
- PlayerCharacter 쪽 책임
- 보임/안 보임 판정과 Fog of War 기준에 사용
```

---

## Initial Implementation Scope

초기 구현은 작은 단계로 나누어 진행한다.

### 1단계: 컴포넌트 뼈대

- `UTDVisionComponent` 생성
- 기본 변수 선언
- `IsLocationVisible()` 임시 구현
- `ATDPlayerCharacter`에 컴포넌트 부착
- 컴파일 확인

### 2단계: 원형 시야 판정

- 2D 거리 기준으로 `NearVisionRadius` 판정
- 원형 디버그 표시
- 테스트 위치가 원 안에 있으면 visible 후보 처리

### 3단계: 전방 콘 시야 판정

- 초기 구현은 `Owner->GetActorForwardVector()` 기준 방향 사용
- `ConeVisionDistance` 거리 체크
- `ConeHalfAngleDeg` 각도 체크
- 좌/우 경계선 및 중심선 디버그 표시

### 4단계: 벽/장애물 가림 체크

- `bUseLineOfSightCheck` 옵션 추가
- `VisionTraceChannel` 기반 LineTrace
- Owner Actor ignore 처리
- 벽/장애물에 막히면 hidden 처리

### 5단계: 액터 조회 함수 추가

- `IsActorVisible(const AActor* TargetActor) const` 추가
- Actor Location 기준으로 `IsLocationVisible()` 재사용

---

## Trade-offs

### ForwardVector 우선 사용

초기 구현에서 `Owner->GetActorForwardVector()`를 사용하면 AimTarget 기반 방향보다 플레이 의도 반영은 약하다.

하지만 기존 Aim 시스템과의 결합을 피할 수 있고, VisionComponent의 코어 판정 로직을 먼저 안정화할 수 있다.

AimTarget 기준 전환은 후속 단계에서 진행한다.

### NearVision + LineTrace

`NearVisionRadius`는 방향과 무관한 기본 시야를 제공한다.

다만 LineTrace를 완전히 무시하면 벽 뒤의 적/아이템까지 보이는 문제가 생길 수 있다.

따라서 초기 규칙은 NearVision이 콘 체크는 생략하되, `bUseLineOfSightCheck`가 켜져 있다면 가림 검사는 유지한다.

### 지형 표시와 엔티티 표시 분리

지형을 완전히 숨기지 않고 어둡게 표현하면 플레이어가 맵 구조를 어느 정도 인지할 수 있어 탐색 스트레스가 줄어든다.

대신 적/아이템/탄피 같은 엔티티는 시야 판정 결과에 따라 숨기므로, 어둠 속 위협과 보상 탐색의 긴장감을 유지할 수 있다.

---

## Future

다음 기능들은 `UTDVisionComponent`의 코어 판정이 안정화된 뒤 별도 단계에서 논의한다.

- AimTarget 기반 시야 방향 전환
- 손전등 On/Off, 배터리, 사운드, 이펙트
- 손전등이 켜졌을 때 `ConeVisionDistance` 또는 `ConeHalfAngleDeg` 변경 여부
- RenderTarget 또는 PostProcess 기반 Fog of War 표현
- “현재 보임”과 “한 번 탐색함” 상태를 둘 다 기록할지 여부
- 적 Mesh / 아이템 / 탄피 표시·숨김을 담당하는 ActorVisibility 시스템
- 시야 밖 적의 AudioPresence 시스템 연동
- 적 AI Alert 상승과 플레이어 시야 판정의 연결 여부
- 필요 시 `Vision` 전용 Collision Channel 추가

---

## Current Decisions

- 플레이어 시야 판정은 `UTDVisionComponent`에서 구현한다.
- `UTDVisionComponent`는 “보이는지 판정”까지만 책임진다.
- 맵 지형지물은 기본적으로 보이되, Fog / Darkness 계열 시스템에서 어둡게 표현한다.
- 적, 아이템, 탄피 같은 엔티티는 시야 판정 결과에 따라 표시/숨김을 결정한다.
- Unreal `AI Perception Component`는 적 AI 감지용으로 남겨둔다.
- 초기 전방 시야 방향은 `Owner->GetActorForwardVector()` 기준으로 한다.
- AimTarget 기준 시야 방향은 후속 단계에서 전환한다.
- NearVision은 방향과 무관하게 visible 후보로 처리한다.
- LineTrace는 지형 렌더링 제어가 아니라 엔티티 가림 판정에 사용한다.
- 초기 구현에서는 Tick을 사용하지 않는다.
- 구현 프롬프트는 사고 방지를 위해 작은 단계로 나누어 진행한다.
