# Extraction Loop

## Overview

탐사와 전투를 거쳐 목표를 달성한 뒤, 활성화된 탈출 영역에 일정 시간 머물러 Location을 완료하는 핵심 게임 루프다.

초기 Office Floor 데모에서는 `OfficeKey`를 획득해 비상계단의 잠긴 장치를 해제한다. 다만 모든 Location이 열쇠를 요구하지는 않으므로, 아이템 조건과 탈출 판정은 서로 독립된 기능으로 구성한다.

## Current State

- `ATDExtractionActivator`가 `ITDInteractableInterface`를 구현하고 `RequiredItem` 보유 여부를 검사한다.
- Office Floor의 Activator는 `OfficeKey` 조건을 통과하면 맵에 배치된 `ATDExtractionZone` 인스턴스의 `ActivateExtraction()`을 호출한다.
- `ATDExtractionZone`은 아이템과 인벤토리를 모르며 활성 상태, Box Overlap, 체류 Timer와 완료 상태만 관리한다.
- 활성화된 영역에 `ExtractionDuration` 동안 머무르면 완료되고, 중간에 나가면 Timer가 취소되어 다시 처음부터 진행한다.
- `OnExtractionCompleted`를 통해 `ATDGameMode::CompleteGameAsVictory()`를 호출한다.
- 승리 시 `WBP_ExtractionSuccess`를 표시하고 게임을 일시정지한다. Restart 버튼은 Pause를 해제하고 현재 레벨을 다시 시작한다.
- 플레이어 사망은 `ATDGameMode::CompleteGameAsDefeat()`로 전달되며, 승리와 패배 상태는 상호 배타적으로 처리한다.

## Key Decisions

### 탈출 조건과 탈출 판정 분리

`ATDExtractionZone`은 특정 아이템이나 인벤토리 규칙을 알지 않는다. Zone은 활성 상태, 플레이어 진입·이탈, 체류 시간, 탈출 완료만 책임진다.

탈출 조건은 별도의 Activator가 판단한다. Office Floor에서는 열쇠를 검사하는 상호작용 장치를 사용하지만, 다른 Location에서는 처음부터 활성화하거나 스위치·발전기 같은 탐사 조건을 사용할 수 있다.

```text
Extraction Condition
  ├─ OfficeKey를 사용하는 잠긴 장치
  ├─ 발전기 또는 스위치 작동
  └─ 조건 없음
          ↓
ActivateExtraction()
          ↓
Extraction Zone
  → 플레이어 진입
  → 일정 시간 체류
  → 탈출 완료
```

### Office Floor 데모 흐름

```text
OfficeKey 획득
  → 비상계단 장치와 상호작용
  → Activator가 OfficeKey 보유 여부 확인
  → 연결된 Extraction Zone 활성화
  → 탈출 영역에 일정 시간 머무름
  → 탈출 성공
```

- OfficeKey는 활성화 조건이며, `ATDExtractionZone` 자체의 고정 요구사항이 아니다.
- 초기 구현에서는 활성화할 때 OfficeKey를 소비하지 않는다.
- 체류 도중 영역을 벗어나면 진행 시간을 취소하거나 초기화한다.
- 지속 시간 판정은 Tick 대신 `TimerManager`를 사용한다.
- 승리 UI와 레벨 종료 처리는 Zone 내부에 직접 넣지 않고, 탈출 완료 이벤트를 구독하는 상위 게임 흐름에 맡긴다.

## Responsibility

### Extraction Activator

- 플레이어의 상호작용 처리
- OfficeKey 등 활성화 조건 검사
- 연결된 `ATDExtractionZone` 참조
- 조건 충족 시 `ActivateExtraction()` 호출
- 조건 실패 안내

### Extraction Zone

- 활성/비활성 상태 관리
- 플레이어의 탈출 영역 진입과 이탈 감지
- 체류 타이머 시작과 취소
- 중복 완료 방지
- 탈출 완료 이벤트 전달

### Game Flow / UI

- 탈출 완료 이벤트 수신
- 승리 화면 표시
- 입력 제한 또는 레벨 전환
- 결과 및 획득물 정산

## Implemented Flow

1. OfficeKey Pickup과 Activator 상호작용
2. `UTDInventoryComponent::HasItem()` 조건 검사
3. 배치된 Extraction Zone 인스턴스 활성화
4. Extraction Area 진입 시 one-shot Timer 시작
5. 영역 이탈 시 Timer 취소 및 진행 초기화
6. 체류 완료 시 GameMode 승리 상태 전환
7. 승리 UI 표시, UI Only 입력, 게임 Pause
8. Restart 버튼으로 Pause 해제 및 레벨 재시작

`ATDPlayerController::BeginPlay()`는 재시작 시 커서를 숨기고 Game Only 입력으로 복구한다.

## Trade-offs

- Actor가 하나 더 필요하고 BP에서 Activator와 Zone 참조를 연결해야 한다.
- 대신 탈출 조건이 달라져도 Zone을 수정하지 않아 재사용성과 테스트 범위가 좋아진다.
- 여러 조건을 조합하는 기능은 초기 데모 범위에서 제외하고, Activator 교체만 가능한 구조를 우선한다.

## Future

- 복수 스위치 또는 발전기 기반 Activator
- 처음부터 활성화된 Extraction Zone
- 여러 탈출 지점과 선택 조건
- 탈출 진행 UI와 활성화 연출
- 관련 문서: [[interaction-system]], [[inventory-loot-system]], [[office-floor-layout]]
