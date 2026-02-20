# Reload Indicator System
## Overview
재장전 진행도를 시각적으로 표시하기 위한 UI 시스템.
Weapon의 재장전 상태와 UI 애니메이션을 동기화하면서, Tick 의존 없이 이벤트 기반으로 동작하도록 설계하였다.

## Key Decisions
- UI는 Weapon을 직접 참조하지 않는다.
    - Delegate 이벤트를 통해 상태만 전달받는다.
- 재장전 게이지는 Duration 기반 애니메이션 속도 계산으로 동기화한다.
    - PlayRate = 1.0 / Duration
- 위젯 생성과 바인딩은 C++에서 확정한다.
    - Blueprint 초기화 순서 문제를 방지.
- Character는 생성/바인딩 담당, PlayerController는 위치 제어 담당으로 분리한다.

## Architecture
- Weapon은 UI를 모른다.
- UI는 Weapon Delegate에 구독한다.
- UI 갱신은 Tick 기반이 아닌 이벤트 기반 구조.
- 재장전 시간은 Duration 값만 전달하고, 애니메이션 속도로 변환하여 동기화.
- ReloadBarWidget은 C++ 위젯 클래스를 부모로 사용하여 바인딩을 명확히 유지.

## Trade-offs
- 애니메이션 길이(1초)를 기준으로 설계되어, 타임라인 변경 시 재조정 필요.
- 인터럽트(피격/구르기 등) 처리 로직은 아직 완전하지 않음.

## Future
- 타이머 기반 → 몽타주 NotifyState 기반으로 전환.
- 재장전 인터럽트 상태 처리 확장.
- 체력 UI 및 기타 상태 UI에도 동일한 이벤트 기반 구조 적용.
- UI 페이드/연출 추가 여부 검토.