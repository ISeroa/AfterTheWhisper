
# Aim-System

## Overview

Aim System은 플레이어의 사격 정확도와 전투 상황 인식을 개선하기 위한 시스템이다.

플레이어는 기본적으로 항상 발사가 가능하며, RMB(Hold)를 통해 **Aim Modifier**
상태에 진입하여 정확도와 시야를 개선하는 대신 이동 속도에 페널티를 받는다.

Aim 상태는 Weapon System과 직접 연결되어 **Spread 계산과 이동 속도 보정에
영향을 준다.**

---

## Key Decisions

- **좌클릭(LMB)은 항상 발사**
  → 입력 구조 단순화

- **우클릭(RMB)은 Aim Modifier**
  → 사격 정확도 및 카메라 오프셋 조정

- Aim 상태에서도 **상호작용(E)은 가능**

- Aim 상태는 다음 세 요소에 영향을 준다

```
Camera
Weapon Spread
Movement Speed
```

### (추가) 시스템 설계 원칙

- Aim 상태는 **발사 가능 여부를 바꾸지 않는다**
- Aim은 Weapon System의 Spread 계산에 **Modifier 형태로만 개입한다**
- Aim 상태는 PlayerCharacter에서 관리하며 다른 시스템은 **읽기 전용으로 참조**한다

---

## Architecture

Aim 시스템은 다음 시스템들과 연결된다.

```
Input
   |
   v
PlayerCharacter
   |
   |-- Weapon System
   |
   |-- Camera System
   |
   `-- Movement System
```

Aim 상태는 PlayerCharacter에서 관리한다.

```
bIsAiming
```

Weapon System은 이 값을 참조하여 **Spread 계산을 수정**한다.

---

## Camera System

기본적으로 카메라는 조준 방향을 향해 일정 거리 오프셋을 가진다.

Aim 상태에서는 이 오프셋이 증가하여 플레이어가 전방 상황을 더 넓게 확인할 수 있다.

무기마다 서로 다른 카메라 오프셋을 가질 수 있다.

예시 파라미터

```
CameraOffset_NormalScale
CameraOffset_AimScale
```

예시 값

```
Pistol   : 1.0 -> 1.2
SMG      : 1.0 -> 1.25
AR       : 1.0 -> 1.35
LMG      : 1.0 -> 1.40
```

---

## Weapon Accuracy

무기의 기본 정확도는 WeaponData에서 정의된

```
SpreadDeg
```

값을 사용한다.

Aim 상태에서는 Spread가 감소한다.

```
FinalSpread = SpreadDeg * AimSpreadMultiplier
```

### (추가) Spread 계산 흐름

```
Weapon Fire
    |
    v
Base Spread (SpreadDeg)
    |
    v
Aim Modifier 적용
    |
    v
Final Spread
```

예시 값

```
SMG  : 0.55
AR   : 0.40
LMG  : 0.65
```

연사 무기를 기본 전투 무기로 가정한다.

---

## Movement Penalty

Aim 상태에서는 이동 속도가 감소한다.

각 무기는 Aim 상태에서의 이동 속도 배율을 가진다.

```
MoveSpeed_AimMultiplier
```

예시

```
Pistol : 0.85
AR     : 0.75
LMG    : 0.65
```

---

## Final Movement Speed

이동 속도는 여러 시스템의 영향을 동시에 받을 수 있다.

- Aim Modifier
- Inventory Weight

최종 속도는 다음 방식으로 계산한다.

```
FinalMoveSpeed =
BaseMoveSpeed
× WeightMultiplier
× AimMultiplier
```

모든 속도 변경은 **하나의 계산 함수**를 통해 적용하여
시스템 간 속도 충돌을 방지한다.

---

## Trade-offs

- Aim Modifier는 이동 속도를 감소시키므로 플레이어가
  정지 상태에서 전투하는 경향이 생길 수 있다.

- 무기별 카메라 오프셋 설정은 밸런스 조정이 필요하다.

- Spread 기반 정확도 시스템은 단순하지만
  복잡한 반동 모델에는 한계가 있다.

---

## Future

- 반동(Recoil) 시스템 추가
- Aim Toggle 옵션 추가
- 무기별 Aim 애니메이션 추가
- 카메라 줌 시스템 추가
- 무기별 Aim 파라미터 확장
