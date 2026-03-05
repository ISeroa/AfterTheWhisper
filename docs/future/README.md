# Future Systems

이 폴더는 **아직 구현되지 않았지만 설계가 진행 중인 시스템**을 정리하는 공간이다.

현재 프로젝트는 기능을 단계적으로 구현하고 있으며,
일부 시스템은 실제 구현 전에 **설계 문서 형태로 먼저 정리**된다.

이 폴더에 있는 문서는 다음 목적을 가진다:

- 향후 구현 예정 기능 기록
- 시스템 설계 방향 정리
- 다른 시스템과의 연동 구조 미리 정의

---

## Document Types

Future 폴더의 문서는 다음과 같은 종류를 포함한다.

### Gameplay Systems

예시:

- Extraction Loop
- Loot System
- Flashlight System
- Visibility System

이 문서들은 **게임의 핵심 플레이 루프를 구성하는 시스템**을 설명한다.

---

### AI / Encounter Systems

예시:

- Enemy Alert System
- Noise System
- Enemy Audio Presence System
- Noise Driven Encounter System

이 문서들은 **적의 반응과 전투 상황 생성 방식**을 정의한다.

---

## Design Policy

Future 문서는 다음 원칙을 따른다.

- 구현 이전 단계의 **설계 기록**
- 실제 구현 시 변경될 수 있음
- 시스템 간 관계를 먼저 정의하는 것을 목표로 함

---

## Workflow

일반적인 개발 흐름:

```
Future Design Document
        ↓
System Implementation
        ↓
docs/systems 로 이동
```

즉,

- **Future → 실제 구현**
- 구현 완료 시 문서를 `systems` 폴더로 이동한다.

---

## Notes

Future 문서는 설계 단계 문서이기 때문에
프로젝트 진행 과정에서 **변경되거나 삭제될 수 있다.**
