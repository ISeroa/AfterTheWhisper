# Enemy System

## Overview
적 캐릭터의 이동, 플레이어 추적, 근접 공격, 사망 중단 흐름을 구성하는 상위 시스템이다.
현재 구현은 `ATDEnemyAIController`의 Timer 기반 이동과 `UTDEnemyMeleeAttackComponent`의 근접 공격을 중심으로 한다.

## Key Decisions
- AI 이동 갱신은 Tick이 아니라 `RepathInterval` 기반 Timer를 사용한다.
- 이동 방식은 `ETDMovementTactic`의 `DirectChase`와 `Encircle`로 구분한다.
- 이동 목표 계산과 `MoveToLocation()` 호출은 `ATDEnemyAIController`가 담당한다.
- 근접 공격의 Windup, 거리 재검사, 데미지, Cooldown은 `UTDEnemyMeleeAttackComponent`가 담당한다.
- 적 사망 시 Controller를 UnPossess하여 이동 Timer를 해제하고 공격 컴포넌트의 Timer도 중단한다.
- Behavior Tree, AI Perception, Alert 상태는 현재 구현 범위에 포함하지 않는다.

## Architecture
- `ATDEnemyCharacter`는 `ATDEnemyAIController`를 기본 Controller로 사용하고 `UTDEnemyMeleeAttackComponent`를 소유한다.
- `ATDEnemyAIController::OnPossess()`는 적마다 초기 실행 시점을 분산하고 반복 Timer를 시작한다.
- Timer가 호출하는 `UpdateMoveTarget()`은 플레이어를 조회하고 이동 전술에 맞는 목표를 계산한다.
- `DirectChase`는 플레이어 위치를 목표로 사용한다.
- `Encircle`은 플레이어 주변 슬롯 중 비용이 가장 낮은 위치를 선택한다.
- 이동 요청 후 `MeleeAttackComp->TryAttack(Player)`를 호출하며, 컴포넌트가 사거리와 Cooldown을 판정한다.
- `OnUnPossess()`는 이동 Timer와 슬롯 예약 상태를 정리한다.
- 사망 처리는 `ATDEnemyCharacter::HandleDeath()`에서 AI, 공격, 이동을 공통 중단한 뒤 Death Mode를 적용한다.

```text
ATDEnemyCharacter
 ├─ ATDEnemyAIController
 │   └─ Timer → 이동 목표 계산 → MoveToLocation
 └─ UTDEnemyMeleeAttackComponent
     └─ 거리 확인 → Windup → 재확인 → ApplyDamage → Cooldown
```

## Trade-offs
- Timer 기반 구조는 단순하고 Tick 비용을 줄이지만 상태 전이가 복잡해지면 Controller 코드가 비대해질 수 있다.
- `GetPlayerPawn(0)` 기반 추적은 싱글플레이에는 충분하지만 멀티플레이나 Target 우선순위를 지원하지 않는다.
- 이동 갱신과 공격 시도를 같은 Timer에서 처리하므로 두 로직에 서로 다른 갱신 빈도가 필요해지면 분리가 필요하다.
- Behavior Tree 없이 빠르게 검증할 수 있지만 Alert, Investigate, Ranged 전투가 추가되면 명시적인 상태 구조가 필요하다.

## Future
- Enemy Alert 및 AI Perception 도입
- 원거리 공격 방식 추가
- Target 선택 정책
- 다수 적 성능 점검과 Encircle 조회 최적화
- 관련 문서: [[enemy-encircle-movement]], [[death-system]], [[enemy-alert-system]]
