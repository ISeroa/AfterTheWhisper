# Death System

## Overview
적이 사망할 때 즉시 제거하지 않고, 물리 기반 Ragdoll 전환과 방향성 반응을 통해 자연스러운 피드백을 제공한다.
이후 일정 시간 뒤 제거하여 향후 Dissolve 및 Loot 시스템 확장을 고려한다.

## Key Decisions
- Destroy 대신 Ragdoll 전환 사용 (즉시 제거 금지)
- 사망 시 AI / Movement 즉시 중단
- Hit 방향 기반 Impulse 적용으로 타격감 강화
- 제거는 즉시가 아닌 Timer 기반 지연 처리
- 사망 처리는 **HealthComponent 이벤트(OnDeath)**를 통해 트리거

## Architecture
- UTDHealthComponent
    - 체력 0 이하 시 OnDeath Delegate 발생
- ATDEnemyCharacter
    - OnDeath 구독
    - 사망 시:
        - AI 정지 (Controller / Behavior 중단)
        - Movement 비활성화
        - Capsule 충돌 비활성화
        - Mesh → SimulatePhysics(true) 전환
        - HitDirection 기반 AddImpulse
- 제거 처리
    - SetLifeSpan 또는 Timer 기반 지연 제거
    - Tick 사용 없음
### 핵심 원칙
- HealthComponent는 Character를 모른다
- Character가 이벤트를 받아 처리한다
- 이벤트 기반 / Tick 없음

## Trade-offs
- Ragdoll 사용으로 Physics 비용 증가
- 다수 적 사망 시 퍼포먼스 영향 가능
- 충돌 설정(Capsule vs Mesh) 관리 필요

## Future
- Dissolve 효과 추가
- Loot Drop 시스템 연결
- Death Animation + Ragdoll 블렌딩
- 사망 상태(Downed 등) 확장 가능
