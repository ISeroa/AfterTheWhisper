# Flashlight System

## Overview
어두운 환경에서 플레이어가 시야를 확보할 수 있도록 하는 조명 시스템.
하지만 적에게 발각될 위험이 증가하는 Risk / Reward 구조를 가진다.

## Key Decisions
- Flashlight는 Direction Vision을 확장
- Flashlight 사용 시 적의 탐지 속도 증가
- 배터리 소모 자원 시스템 적용

## Architecture
- Flashlight ON → Cone Vision Range 증가
- Flashlight OFF → 기본 시야만 유지
- AI는 Flashlight 사용 여부에 따라 Alert 증가

## Trade-offs
- 현실적인 광원 계산 대신 단순 Cone Light 구조 사용
- 복잡한 조명 시뮬레이션은 제외

## Future
- Flashlight 깜빡임
- AI가 Flashlight 직접 인식
