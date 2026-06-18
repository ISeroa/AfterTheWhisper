# Systems Documentation

이 폴더는 프로젝트에 구현된 Gameplay System의 현재 구조와 설계 결정을 기록한다.
구현 전 아이디어와 계획은 `docs/future/`에서 관리하고, 실제 구현이 시작되면 현재 코드 기준으로 문서를 이곳으로 이동한다.

## Folder Structure

```text
systems
 ├─ ai
 │   ├─ enemy-system.md
 │   └─ enemy-encircle-movement.md
 ├─ combat
 │   ├─ weapon-system.md
 │   └─ weapon-audio-system.md
 ├─ gameplay
 │   ├─ aim-system.md
 │   ├─ death-system.md
 │   └─ vision-system.md
 └─ ui
     ├─ ammo-indicator.md
     ├─ player-hud.md
     └─ reload-indicator.md
```

## Categories

- `ai`: 적과 아군의 판단, 이동, 전투 행동
- `combat`: Weapon과 직접적인 전투 처리
- `gameplay`: 플레이어 조작과 공통 Gameplay 규칙
- `ui`: Gameplay 상태를 표시하는 Widget과 갱신 흐름

## Documentation Policy

- 시스템 문서는 `docs/_template.md`의 `Overview`, `Key Decisions`, `Architecture`, `Trade-offs`, `Future` 구조를 사용한다.
- 추가 설명이 필요하면 다섯 개 섹션 안에서 `###` 하위 제목을 사용한다.
- 파일명은 소문자 `kebab-case`를 사용한다.
- 문서는 현재 코드에서 확인되는 사실을 기준으로 작성한다.
- 아직 구현되지 않은 기능은 `Future`에 짧게 기록하거나 `docs/future/`의 별도 문서로 분리한다.
- 하나의 문서가 다른 하위 시스템의 상세 구현을 반복하지 않도록 관련 문서 링크를 사용한다.

## Related Documents

- `docs/_template.md`
- `docs/future/README.md`
- `docs/DevLog.md`
- `docs/Roadmap.md`
