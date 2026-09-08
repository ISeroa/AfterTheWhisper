# Navigation Indicator

## Overview
플레이어가 `O` 키를 누르면 미리 배치된 탈출 지점의 화면상 방향과 거리를 잠시 표시한다.
항상 노출되는 퀘스트 마커 대신, FPS의 방향성 피격 표시처럼 플레이어 주변에 회전하는 `>` 형태의 인디케이터를 사용해 탐색 흐름을 과도하게 방해하지 않는다.

## Key Decisions
- 안내 UI는 상시 표시하지 않고 `O` 입력 시 5초 동안만 표시한다.
- 마지막 1초에는 `FadeOut` 애니메이션을 재생하고 이후 숨긴다.
- 표시 중 `O`를 다시 누르면 기존 타이머를 초기화하고 표시 시간을 다시 시작한다.
- 현재 데모에서는 맵에 미리 배치된 첫 번째 `ATDExtractionZone`을 안내 대상으로 사용한다.
- 게임 시작 시 기본 탈출 지점을 등록하고, 플레이 도중 다른 탈출 지점이 활성화되면 해당 지점을 새로운 `NavigationTarget`으로 갱신하는 방향으로 확장한다.
- 인디케이터가 월드의 모든 탈출 지점을 반복 검색하지 않도록, 추가 지점의 활성화 주체가 변경된 목표를 명시적으로 전달하는 구조를 사용한다.
- 방향은 플레이어와 탈출 지점의 월드 위치를 화면 좌표로 투영한 뒤 정규화하여 계산한다.
- 거리는 2D 월드 거리(cm)를 m로 변환하고 정수로 반올림해 표시한다.
- 퀘스트 목표와 탈출구 안내는 같은 개념으로 묶지 않는다. 이 UI는 탈출 위치 확인만 담당한다.
- 입력 중에만 짧은 주기로 갱신하는 Timer 구조를 사용하며 상시 Widget Tick은 사용하지 않는다.

## Architecture
- `ATDPlayerCharacter`
  - `BeginPlay()`에서 `NavigationIndicatorWidgetClass`로 위젯을 생성해 Viewport에 추가한다.
  - 맵에 배치된 `ATDExtractionZone`을 찾아 `NavigationTarget`으로 보관한다.
  - `NavigationGuide` 입력을 받아 `ShowNavigationGuide()`를 호출한다.
  - 표시, 갱신, 페이드아웃, 숨김 시점을 각각 `TimerManager`로 제어한다.
- `UTDNavigationIndicatorWidget`
  - `ShowIndicator()`, `UpdateIndicator()`, `BeginFadeOut()`, `HideIndicator()`로 표시 책임을 나눈다.
  - `HorizontalBox_Indicator`, `Text_Arc`, `Text_Distance`, `FadeOut`을 `BindWidget`/`BindWidgetAnim`으로 연결한다.
  - 방향 벡터에 `IndicatorRadius`를 곱해 인디케이터 위치를 옮기고 `atan2` 각도로 `Text_Arc`를 회전한다.
- `WBP_NavigationIdicator`
  - 현재 `UTDNavigationIndicatorWidget`을 부모로 사용하는 실제 UMG 위젯이다.
  - 회색 방향 기호와 흰색 거리 텍스트를 표시한다.
- `Config/DefaultInput.ini`
  - `NavigationGuide` 액션을 `O` 키에 매핑한다.

```text
O 입력
  -> ATDPlayerCharacter::ShowNavigationGuide()
  -> UTDNavigationIndicatorWidget::ShowIndicator()
  -> 화면 방향 및 거리 주기적 갱신
  -> 4초 후 FadeOut
  -> 5초 후 숨김 및 갱신 타이머 종료
```

### Troubleshooting
- 증상: 입력, 목표 탐색, 화면 좌표 투영과 거리 계산 로그는 정상이지만 UI가 보이지 않았다.
- 진단: `ShowIndicator()`에서 `HorizontalBox_Indicator`가 `NULL`인 것을 확인했다.
- 실제 원인: `BP_TDPlayerCharacter`의 `NavigationIndicatorWidgetClass`에 UMG Blueprint가 아니라 C++ 부모 클래스 `UTDNavigationIndicatorWidget`을 지정했다. 빈 C++ 위젯 인스턴스에는 Widget Tree가 없으므로 `BindWidget` 대상이 존재하지 않았다.
- 해결: `NavigationIndicatorWidgetClass`에 실제 UMG 위젯 Blueprint를 지정했다.
- 교훈: `CreateWidget()` 성공은 UMG 내부 Widget Tree와 `BindWidget` 연결 성공을 보장하지 않는다. 위젯 클래스, 실제 생성 클래스 경로, 바인딩 포인터를 단계별로 확인해야 한다.
- 추가 이슈: `ATDPlayerCharacter`가 상속한 엔진 메타데이터와 `Navigation` 카테고리명이 충돌해 클래스 디폴트에서 설정이 보이지 않았다. 카테고리를 `UI|NavigationGuide`로 변경해 해결했다.

## Trade-offs
- 첫 번째 `ATDExtractionZone`을 자동 탐색하는 구조는 현재 단일 탈출구 데모에는 단순하고 충분하지만, 복수 탈출구나 동적 목표에는 적합하지 않다.
- 동적 탈출 지점 등록 시에는 동시에 여러 지점이 활성화될 경우 어떤 지점을 우선 안내할지 별도 규칙이 필요하다.
- 월드 위치를 화면 좌표로 투영하는 현재 방식은 대상이 화면 뒤에 있는 경우의 방향 보정과 화면 가장자리 제한을 별도로 처리하지 않는다.
- TextBlock의 `>` 문자를 회전하는 방식은 제작 속도가 빠르지만, 전용 이미지나 머티리얼에 비해 형태와 연출 확장성이 낮다.
- 문제 확인을 위해 추가한 `[Navigation]` 로그는 검증 후 제거하거나 non-shipping 조건으로 제한할 필요가 있다.

## Future
- `WBP_NavigationIdicator`를 `WBP_NavigationIndicator`로 이름 정리
- 화면 뒤쪽에 있는 목표의 방향 보정
- 해상도와 화면 비율을 고려한 위치 반경 및 가장자리 Clamp
- 게임 시작 시 기본 탈출 지점을 등록하고, 추가 탈출 지점 활성화 이벤트에서 대상을 명시적으로 갱신하는 구조
- 복수 탈출 지점이 동시에 활성화될 경우의 선택 우선순위 정의
- 등장과 퇴장 애니메이션 및 거리 텍스트 스타일 개선
- 기능 검증 후 상세 디버그 로그 정리
