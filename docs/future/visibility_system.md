# Visibility System

## Overview
플레이어가 바라보는 방향과 근거리 인지 범위를 결합하여 탐사 긴장감을 만드는 시야 시스템.
근거리는 항상 인지 가능하지만 먼 거리는 바라봐야 확인할 수 있도록 설계한다.

## Key Decisions
- Circle + Direction 구조 사용
- 근거리 Circle Vision은 항상 가시
- Direction Cone은 플레이어가 바라보는 방향만 확장 시야 제공
- 시야 밖의 적은 렌더링하지 않음

## Architecture
- Circle Vision: 플레이어 주변 기본 시야
- Direction Cone: 바라보는 방향 확장 시야
- Visibility 체크 후 Enemy Mesh Visibility 제어
- Tick 최소화, 이벤트 기반 업데이트

## Trade-offs
- 완전한 물리 기반 시야 계산 대신 단순화된 Vision 구조 사용
- 스텔스보다는 탐사 긴장감 중심 설계

## Future
- 벽/장애물 기반 시야 차단 확장
- AI와 동일 시야 시스템 공유
