# Systems Documentation

이 폴더는 프로젝트에서 구현된 **게임플레이 시스템 설계 문서**를 정리한 공간이다.

문서는 기능 영역별로 분류되어 있으며, 각 시스템의 구조와 설계 의도를 설명한다.

---

## Folder Structure

```
systems
 ├ combat
 │   ├ Weapon-System.md
 │   ├ Weapon-Audio-System.md
 │   └ Interaction-System.md
 │
 ├ ai
 │   ├ Enemy-System.md
 │   ├ EnemyAISystem.md
 │   ├ EnemyEncircleMovement.md
 │   └ Ally-Squad-AI-System.md
 │
 ├ gameplay
 │   ├ Aim-System.md
 │   ├ Death-System.md
 │   └ Inventory-Loot-System.md
 │
 ├ ui
 │   ├ PlayerHUD.md
 │   ├ Ammo-Indicator.md
 │   └ Reload-indicator.md
 │
 └ README.md
```

---

## Categories

### Combat
전투와 직접적으로 관련된 시스템.

예:
- Weapon System
- Weapon Audio
- Interaction System

### AI
적과 아군의 AI 관련 시스템.

예:
- Enemy AI
- Encircle Movement
- Squad AI

### Gameplay
플레이어 조작 및 핵심 게임플레이 로직.

예:
- Aim System
- Inventory / Loot

### UI
플레이어에게 정보를 전달하는 인터페이스.

예:
- Player HUD
- Ammo Indicator
- Reload Indicator

---

## Design Philosophy

문서는 다음 원칙을 기반으로 작성된다.

- 시스템 단위 문서화
- Data‑Driven 설계 설명
- C++ / Blueprint 역할 분리 명시
- 확장 가능성 고려

---

## Related Documents

상위 문서:

- `docs/DevLog.md`
- `docs/Roadmap.md`

향후 설계 문서:

- `docs/future/`
