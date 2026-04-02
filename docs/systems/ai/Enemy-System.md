# Enemy-System

## Overview
Enemy System은 게임 내 적 캐릭터의 행동, 전투, 이동 전략을 구성하는 상위 시스템이다.

현재 Enemy 관련 기능은 여러 개의 하위 시스템으로 분리되어 있으며,
각 문서는 독립적으로 설계되어 있지만 전체적으로 하나의 Enemy 전투 파이프라인을 형성한다.

이 문서는 Enemy 관련 시스템의 **상위 개요와 문서 인덱스 역할**을 한다.

---

## Sub Systems

현재 Enemy 시스템은 다음 문서들로 구성되어 있다.

```
Enemy-System
 ├ EnemyAISystem.md
 ├ EnemyEncircleMovement.md
 ├ EnemyAttackSystem.md (future / partially implemented)
 └ → Death-System.md (docs/systems/gameplay/)
```

각 문서의 역할:

### EnemyAISystem.md
적의 **상태 관리와 행동 선택 로직**을 담당한다.

예:
- Idle
- Alert
- Chase
- Combat

AIController 및 Behavior 로직이 이 영역에 해당한다.

---

### EnemyEncircleMovement.md
적이 플레이어를 **둘러싸는 이동 전략(Encircle)**을 정의한다.

핵심 목표:

- 적이 한 방향에서만 몰리지 않도록 분산
- 플레이어 주변 슬롯 기반 포지셔닝
- 근접 공격을 위한 위치 확보

---

### EnemyAttackSystem.md
적의 **공격 로직과 전투 처리**를 담당한다.

현재 구현:

- 근접 공격 컴포넌트
- 공격 쿨다운
- Windup
- 거리 체크

향후 확장:

- 원거리 공격
- 공격 애니메이션
- 히트 리액션

---

### Death-System.md (`docs/systems/gameplay/`)
적의 **사망 처리**를 담당한다.

현재 구현:

- Ragdoll 전환 (SimulatePhysics + 방향성 Impulse)
- AI / 이동 / 공격 타이머 즉시 중단
- `ETDEnemyDeathMode` 분기 구조 (Ragdoll / Animation / ImmediateDestroy)
- BP `EditDefaultsOnly` 로 적 타입별 사망 방식 설정 가능

향후 확장:

- Animation case 구현
- Dissolve 효과
- Loot Drop 연결

---

## Architecture

Enemy 시스템 전체 구조

```
EnemyCharacter
      |
      v
EnemyAIController
      |
      v
AI Decision
      |
      |-- Encircle Movement
      |
      |-- Attack Component
      |
      `-- State Control
```

각 적 타입은 공통 기반 클래스를 사용한다.

```
EnemyCharacter (Base C++)
   |
   ├ Zombie
   ├ Melee Survivor
   └ Ranged Survivor
```

Blueprint 파생을 통해 적 타입을 구분한다.

---

## Design Goals

Enemy 시스템 설계 목표:

- **C++ 기반 공통 로직**
- **BP 파생으로 적 타입 분리**
- **Encircle 기반 전투 포지셔닝**
- **컴포넌트 기반 공격 시스템**
- **AI 상태와 이동 전략 분리**

---

## Future

향후 Enemy 시스템 확장 예정:

- Ranged Combat System
- Damage Reaction System
- Enemy Group Coordination
- Threat / Aggro System
- Advanced Navigation Behaviors
