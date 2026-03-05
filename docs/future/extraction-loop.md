# Extraction Loop

## Overview
아이템을 획득한 후 탈출하는 핵심 게임 루프.

## Key Decisions
- 탐사 → 전투 → 아이템 획득 → 탈출 구조
- 플레이어 선택 기반 탐사 리스크 설계

## Architecture
Base
↓
Building Exploration
↓
Loot / Enemy
↓
Item 획득
↓
Extraction
↓
Base

## Trade-offs
- 작은 맵에서 높은 밀도 플레이 경험

## Future
- 다양한 Extraction 조건
- 여러 탈출 지점
