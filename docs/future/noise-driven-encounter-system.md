# Noise-Driven Encounter System

## Overview
Noise Event를 입력으로 받아 기존 적의 행동 변화와 제한적인 좀비 증원을 조정하는 Encounter 시스템이다.
고정된 작은 맵에서도 플레이어 행동에 따라 교전 밀도와 전투 흐름이 달라지는 구조를 목표로 한다.

## Key Decisions
- 인간형 생존자는 맵 시작 시 배치된 개체만 사용하고 Noise로 추가 Spawn하지 않는다.
- 대부분의 Noise 반응은 기존 AI의 Investigate, Alert, Positioning 변화로 처리한다.
- 좀비 증원은 모든 Noise에 발생하지 않고 Cooldown, 최대 수, 지역 Cap을 통과한 경우에만 허용한다.
- Noise Event 생성과 전달은 Noise System의 책임으로 두고 Encounter는 소비자 역할만 한다.
- Encounter Director는 무작위 Spawn보다 전투 밀도를 제한하는 연출된 Random을 제공한다.
- 갱신은 이벤트와 Timer를 사용하고 Tick 기반 전체 탐색을 피한다.

## Architecture
### Core Flow

```text
Noise System
  → Noise Event
  → Encounter Director
      ├─ 주변 Zombie 반응 요청
      ├─ Survivor 전술 상태 변경 요청
      └─ 조건 충족 시 Zombie Reinforcement
```

### Zombie Reaction
- Noise Radius 안의 Zombie는 거리와 Loudness를 기준으로 반응 확률을 계산한다.
- 반응한 Zombie는 Noise Location을 Investigate하고 플레이어를 발견하면 Chase로 전환한다.
- 실제 상태 전이는 Enemy AI 또는 Alert System이 담당하고 Director는 반응 요청만 전달한다.

### Survivor Reaction
- Survivor는 추가 Spawn 없이 Alert, Cover 이동, Encircle 같은 전술 상태만 변경한다.
- Noise 방향은 Positioning 입력으로 사용할 수 있다.

### Reinforcement Limits
- `ReinforcementCooldown`
- `MaxZombieCount`
- `AreaEnemyCap`
- Spawn 가능 위치와 NavMesh 유효성

### Encounter Director
- Noise Event 수신
- 반응 대상과 확률 결정
- 증원 조건과 전투 밀도 확인
- Cooldown과 지역 Cap 관리

## Trade-offs
- 플레이어 행동으로 전투가 변해 반복 플레이가 다양해지지만 튜닝이 부족하면 작은 실수가 과도한 교전으로 번질 수 있다.
- Director는 전투 밀도를 통제할 수 있지만 Noise System과 Enemy 상태 시스템 사이의 조정 책임이 커질 수 있다.
- 확률 기반 반응은 예측 불가능성을 만들지만 플레이어가 소음 결과를 이해하기 어려울 수 있다.
- 제한적 증원도 Spawn 위치가 부자연스러우면 연출 의도가 쉽게 드러난다.

## Future
- Horde Event
- Noise 종류별 반응 Profile
- AI Communication
- Adaptive Difficulty는 별도 단계에서 검토
- 관련 문서: [[noise-system]], [[enemy-alert-system]], [[enemy-system]]
