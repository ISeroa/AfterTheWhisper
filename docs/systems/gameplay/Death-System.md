# Death System

## Overview
적 사망 시 AI와 공격을 즉시 중단하고, 선택된 Death Mode에 따라 Ragdoll 또는 제거 처리를 수행하는 시스템이다.
현재 Ragdoll과 Immediate Destroy가 동작하며 Animation Mode는 임시 처리 상태다.

## Key Decisions
- 공통 사망 정리는 Death Mode 분기 전에 한 번만 수행한다.
- Controller UnPossess로 Enemy AI의 Repath Timer를 해제한다.
- `UTDEnemyMeleeAttackComponent::StopAttack()`으로 Windup과 Cooldown Timer를 정리한다.
- 이동을 중단하고 Capsule Collision을 비활성화한다.
- Ragdoll은 마지막 Point Damage 방향을 사용하여 방향성 Impulse를 적용한다.
- Ragdoll 제거는 `SetLifeSpan()`으로 지연한다.
- Death Mode는 `EditDefaultsOnly`로 노출하여 적 Blueprint별로 선택한다.

## Architecture
- `UTDHealthComponent`는 체력이 0 이하가 되면 `OnDeath`를 Broadcast한다.
- `ATDBaseCharacter`가 연결한 `HandleDeath()`는 파생 `ATDEnemyCharacter` 구현으로 전달된다.
- `ATDEnemyCharacter::TakeDamage()`는 `FPointDamageEvent::ShotDirection`을 `LastHitDirection`에 저장한다.
- Point Damage가 아니면 Damage Causer 위치를 기준으로 방향을 fallback 계산한다.
- `ATDEnemyCharacter::HandleDeath()`는 AI, 공격, 이동, Capsule을 공통 정리한다.
- `Ragdoll`은 Mesh Collision과 Physics를 활성화하고 Impulse를 적용한 뒤 `RagdollLifeTime` 후 제거한다.
- `Animation`은 아직 사망 Animation을 재생하지 않고 Actor를 숨긴 뒤 2초 후 제거하는 임시 처리다.
- `ImmediateDestroy`는 즉시 `Destroy()`를 호출한다.
- Ragdoll 동작에는 Physics Asset과 적절한 Mesh Collision 설정이 필요하며 Blueprint Override를 확인해야 한다.

## Trade-offs
- Ragdoll은 즉각적인 피격 피드백을 주지만 다수 적이 동시에 사망하면 Physics 비용이 증가한다.
- Point Damage가 아닌 공격의 방향 fallback은 실제 충격 방향과 다를 수 있다.
- Animation Mode의 현재 숨김 처리는 구조 검증용일 뿐 완성된 사망 연출이 아니다.
- Capsule을 비활성화한 뒤에는 Mesh Physics와 Collision Preset 설정에 동작이 크게 의존한다.

## Future
- Animation Mode의 사망 Montage 구현
- Dissolve와 사망 Sound
- Loot Drop 연결
- 동시 Ragdoll 수와 Life Time 성능 튜닝
- 관련 문서: [[enemy-system]], [[inventory-loot-system]]
