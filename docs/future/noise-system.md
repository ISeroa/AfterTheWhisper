# Noise System

## Overview
플레이어 행동으로 발생하는 소음을 기반으로 적의 반응을 유도하는 시스템.
소음은 탐사 난이도와 전투 확장을 결정한다.

## Key Decisions
- Gunshot, Door, Movement 등 이벤트 기반 Noise 생성
- 좀비는 Noise 기반 반응
- Survivor는 Noise + Vision 기반 반응

## Architecture
- Noise Event 생성
- Noise Radius 계산
- Zombie Spawn / Alert Event 발생

## Trade-offs
- 단순 Radius 기반 Noise 시스템 사용
- 복잡한 물리 음향 시스템은 제외

## Future
- 다양한 Noise 타입
- AI별 Noise 민감도 차이
