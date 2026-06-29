# Future Systems

이 폴더는 아직 구현되지 않았거나 구현 범위가 확정되지 않은 시스템 설계를 기록한다.
문서는 구현 전에 책임 경계와 핵심 결정을 정리하기 위한 것이며 개발 과정에서 변경되거나 제거될 수 있다.

## Documents

### Gameplay

- `actor-visibility-system.md`
- `aim-modifier-system.md`
- `extraction-loop.md`
- `flashlight-system.md`
- `hit-reaction-and-hit-marker-system.md`
- `interaction-system.md`
- `inventory-loot-system.md`

### AI and Encounter

- `ally-squad-ai-system.md`
- `enemy-alert-system.md`
- `enemy-audio-presence-system.md`
- `noise-system.md`
- `noise-driven-encounter-system.md`

## Documentation Policy

- 모든 시스템 문서는 `docs/_template.md`의 `Overview`, `Key Decisions`, `Architecture`, `Trade-offs`, `Future` 구조를 사용한다.
- 추가 설명은 다섯 개 섹션 안에서 `###` 하위 제목으로 작성한다.
- 파일명은 소문자 `kebab-case`를 사용한다.
- 실제 구현된 내용처럼 단정하지 않고 설계 결정과 예상 구조를 구분한다.
- 구현 단위가 다른 시스템은 문서를 분리하고 서로의 책임을 반복하지 않는다.

## Workflow

```text
Future Design Document
  → 작은 기능 단위 구현
  → 코드 기준으로 문서 교정
  → docs/systems의 적절한 카테고리로 이동
```

- 일부만 구현된 경우 구현된 핵심 시스템은 `systems`에 두고 미구현 확장은 별도 `future` 문서로 유지한다.
- 구현 완료 시 기존 Future 문서를 단순 이동하지 않고 실제 클래스와 흐름을 다시 확인한다.

## Related Documents

- `docs/_template.md`
- `docs/systems/README.md`
- `docs/Roadmap.md`
