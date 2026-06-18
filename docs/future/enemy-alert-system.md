# Enemy Alert System

## Overview
적이 시각과 Noise 입력을 바탕으로 플레이어에 대한 경계 단계를 관리하는 시스템이다.

## Key Decisions
- 단계 기반 Alert 구조 사용
- AI Perception과 Noise Event를 감지 입력으로 사용
- 플레이어의 Vision System과 적의 감지 판정은 분리

## Architecture
```text
Idle → Suspicious → Alert → Combat
```

- 각 단계는 조사, 추적, 공격 같은 AI 행동 선택에 영향을 준다.
- Enemy Alert System은 플레이어 화면의 가시성을 계산하지 않는다.
- Flashlight는 직접 Alert를 올리기보다 AI 감지 입력 또는 감지 보정값으로 전달한다.

## Trade-offs
- 복잡한 감지 모델 대신 단계 기반 시스템 사용
- 단계가 적으면 구현은 단순하지만 경계 변화가 갑작스럽게 느껴질 수 있음

## Future
- 그룹 Alert 공유
- 전술 행동 추가
- 관련 문서: [[noise-system]], [[flashlight-system]], [[enemy-system]]
