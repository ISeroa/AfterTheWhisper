# Death System

## Overview
적이 사망할 때 즉시 제거하지 않고, 물리 기반 Ragdoll 전환과 방향성 반응을 통해 자연스러운 피드백을 제공한다.
`ETDEnemyDeathMode` 분기 구조를 통해 향후 다른 사망 연출 방식도 추가할 수 있도록 확장 지점을 열어두었다.

## Key Decisions
- Destroy 대신 DeathMode 기반 처리 (기본: Ragdoll)
- AI / Movement / 공격 타이머는 DeathMode에 관계없이 공통 처리
- Hit 방향 기반 Impulse 적용으로 타격감 강화
- 제거는 즉시가 아닌 `SetLifeSpan` 지연 처리
- Tick 사용 없음

## Architecture

```
UTDHealthComponent
  └─ CurrentHealth <= 0 → Die() → OnDeath.Broadcast()

ATDBaseCharacter::BeginPlay()
  └─ OnDeath.AddDynamic(this, HandleDeath)
       ↓ (UFunction 이름 기반 조회 → 파생 클래스 버전 자동 호출)

ATDEnemyCharacter::TakeDamage()
  └─ FPointDamageEvent::ShotDirection → LastHitDirection 저장

ATDEnemyCharacter::HandleDeath()
  ├─ [공통] GetController()->UnPossess()       — AI + RepathTimer 중단
  ├─ [공통] MeleeAttackComp->StopAttack()      — 윈드업/쿨다운 타이머 정리
  ├─ [공통] MovementComp->DisableMovement()    — 이동 중단
  ├─ [공통] Capsule->SetCollisionEnabled(None) — 캡슐 비활성화
  └─ switch(DeathMode)
       ├─ Ragdoll         → SimulatePhysics + AddImpulse + SetLifeSpan(RagdollLifeTime)
       ├─ Animation       → TODO (placeholder: Hide + LifeSpan 2s)
       └─ ImmediateDestroy→ Destroy()
```

## ETDEnemyDeathMode

| 값 | 상태 | 설명 |
|---|---|---|
| `Ragdoll` | **구현 완료** | 물리 기반 래그돌 전환 + 방향성 Impulse |
| `Animation` | placeholder | 사망 애니메이션 재생 (미구현) |
| `ImmediateDestroy` | placeholder | 즉시 Destroy (미구현) |

BP `EditDefaultsOnly` 로 노출되어 적 타입별 설정 가능. 기본값: `Ragdoll`.

## Key Properties (ATDEnemyCharacter)

| 프로퍼티 | 기본값 | 설명 |
|---|---|---|
| `DeathMode` | `Ragdoll` | 사망 처리 방식 선택 |
| `RagdollImpulseStrength` | `600.f` | Impulse 세기 (BP에서 튜닝) |
| `RagdollLifeTime` | `5.f` | 래그돌 후 제거까지 대기 시간(초) |
| `LastHitDirection` | `ZeroVector` | 마지막 피격 방향 (TakeDamage에서 갱신) |

## Direction 추출 흐름

무기가 `ApplyPointDamage(... Dir, Hit ...)` 를 사용하므로
`ATDEnemyCharacter::TakeDamage()` 에서 `FPointDamageEvent::ShotDirection` 을 직접 읽는다.
`FPointDamageEvent` 가 아닌 경우 `DamageCauser` 위치 기반으로 fallback.

## StopAttack()

`UTDEnemyMeleeAttackComponent::StopAttack()` 추가.
윈드업 진행 중 사망 시 데미지 판정이 취소됨을 보장한다.

## Blueprint 확인 사항

- **Physics Asset** 지정 필수 (없으면 래그돌 무반응)
- **Mesh Collision Preset** → `Ragdoll` 또는 `PhysicsActor` (BP 오버라이드 주의)

## Trade-offs
- Ragdoll 사용으로 Physics 비용 증가 (다수 동시 사망 시 주의)
- Capsule 비활성화 후 Mesh 충돌만 남으므로 Physics Asset 설정 필수

## Future
- `Animation` case 구현 — 사망 몽타주 재생 후 Destroy
- Dissolve 효과 연결 (Phase 2)
- Loot Drop 시스템 연결 (Phase 2)
