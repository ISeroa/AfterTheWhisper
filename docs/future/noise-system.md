# Noise System

## Overview
총격, 폭발, 이동, 환경 상호작용에서 발생한 소음을 공통 Gameplay Event로 표현하고 필요한 시스템에 전달한다.
이 시스템은 소음 데이터의 생성과 배포만 담당하며 AI 행동, Alert 전이, 증원 Spawn은 직접 결정하지 않는다.

## Key Decisions
- 소음은 호출 시점에 생성되는 이벤트 기반 구조를 사용한다.
- Noise Event는 최소한 Location, Loudness, Radius, Type 정보를 제공한다.
- 소음 발생 주체는 반응할 AI를 직접 찾거나 구체 행동을 지시하지 않는다.
- AI 감지와 Encounter 연출은 동일한 Noise Event를 각자의 규칙으로 소비한다.
- 초기 전달 범위는 단순 Radius를 사용하고 복잡한 물리 음향 전파는 제외한다.
- Weapon Fire, Explosion, Environmental Noise를 동일한 이벤트 형식으로 통합한다.

## Architecture
- Weapon, Door, 환경 Actor가 행동이 발생한 시점에 Noise Event를 생성한다.
- Noise System은 위치와 반경을 기준으로 이벤트를 전달하거나 중앙 Dispatcher에 Broadcast한다.
- Enemy Alert System은 이벤트를 받아 Suspicious 또는 Alert 전이를 판단한다.
- Noise-Driven Encounter System은 이벤트를 받아 주변 적 반응과 제한적 증원을 판단한다.
- 발생 주체와 소비 시스템은 서로의 구체 클래스를 직접 알지 않는다.
- 기본 흐름은 다음과 같다.

```text
Weapon / Environment / Movement
  → Noise Event(Location, Loudness, Radius, Type)
      ├─ Enemy Alert System
      └─ Noise-Driven Encounter System
```

## Trade-offs
- Radius 기반 전달은 단순하고 예측 가능하지만 벽, 문, 층 구조에 따른 차폐를 표현하지 못한다.
- 중앙 Event 구조는 결합도를 줄이지만 여러 소비자의 반응 순서와 중복 처리를 관리해야 한다.
- Noise Type을 너무 일찍 세분화하면 튜닝 데이터가 늘어나고, 너무 단순하면 AI별 반응 차이를 만들기 어렵다.

## Future
- AI 타입별 Noise 민감도
- 벽과 문에 의한 감쇠
- Noise Type 세분화
- 디버그 반경과 이벤트 로그
- 관련 문서: [[noise-driven-encounter-system]], [[enemy-alert-system]], [[weapon-system]]
