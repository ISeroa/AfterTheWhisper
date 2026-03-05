# Enemy Alert System

## Overview
적이 플레이어를 감지하는 단계를 관리하는 시스템.

## Key Decisions
- 단계 기반 Alert 구조 사용
- Vision / Noise / Flashlight 영향을 받음

## Architecture
- Idle
- Suspicious
- Alert
- Combat

각 단계는 AI 행동 상태를 변경한다.

## Trade-offs
- 복잡한 감지 모델 대신 단계 기반 시스템 사용

## Future
- 그룹 Alert 공유
- 전술 행동 추가
