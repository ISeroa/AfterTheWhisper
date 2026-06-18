# Ally Squad AI System

## Overview
플레이어가 Move, Guard, Attack 명령을 내려 여러 동료를 운용하고 방 단위 전투를 수행하는 아군 AI 시스템이다.
현재는 구현 전 설계이며, 복잡한 Behavior Tree보다 작은 FSM과 중앙 분배 구조를 우선 검토한다.

## Key Decisions
- 초기 행동 구조는 경량 FSM으로 제한한다.
- Door와 Room Volume이 공유하는 `RoomId`를 기준으로 방 단위 명령을 전달한다.
- Entry Slot과 Target 배정은 Squad Coordinator가 중앙에서 관리한다.
- 동료는 적 처치만 담당하고 아이템 회수는 플레이어가 수행한다.
- 자가 치료는 Inventory 연결 없이 Charge와 Cooldown으로 단순화한다.
- 탐색과 명령 갱신은 이벤트 또는 낮은 빈도의 Timer를 사용하고 매 프레임 검색하지 않는다.

## Architecture
- `ATDAllyCharacter`는 공통 Character와 Health 흐름을 재사용한다.
- `ATDAllyAIController`는 현재 명령에 맞는 FSM을 실행한다.
- `UTDAllyCommandComponent`는 명령, 목표, Target, `RoomId` 데이터를 보관한다.
- `UTDSquadCoordinatorComponent`는 명령 접수, 집결, Entry Slot, Target 분배와 Room Clear 판정을 담당한다.
- `ATDRoomVolume`과 `ATDDoor`는 동일한 `RoomId`로 명령 대상 공간을 식별한다.
- Character는 명령 실행 방식을 직접 결정하지 않고, Controller와 Coordinator가 행동과 분배를 담당한다.

## Trade-offs
- 중앙 Coordinator는 중복 Target과 진입 충돌을 줄이지만 단일 컴포넌트의 책임이 커질 수 있다.
- FSM은 초기 구현과 디버깅이 쉽지만 명령과 전투 상태가 많아지면 전이 관리가 복잡해진다.
- 엄폐, EQS, Formation을 제외하면 전술 표현에는 한계가 있지만 초기 범위를 통제할 수 있다.

## Future
- 명령 Queue와 Shift 입력
- 근거리·원거리 역할 구분
- 위협도 기반 Target 우선순위
- 동료 성장과 관계 시스템은 별도 문서로 분리
