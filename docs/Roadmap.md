# TopDownShooter Roadmap (Portfolio Focus: Small but Polished Game)

## 목표
- 1개 스테이지를 '처음부터 끝까지' 플레이 가능하게 만들기
- 전투가 손맛 있고, 피드백이 명확하고, 버그가 적은 상태
- 첫 30초 체험에서 "게임 같다"가 나오게 만들기

---

## Phase 1 — Playable Vertical Slice (필수)
### 1) Core Gameplay
- [ ] 이동/에임 안정화 (Aim 실패 시 fallback, 회전 안정)
- [ ] 무기 기본 완성 (단발 + 연사/FireRate Timer)
- [ ] 리로드(재장전) 구현 + UI 반영
- [ ] 탄약/재장전 사운드 훅(빈 슬롯이어도 placeholder로)

### 2) Combat Feel (손맛 = 포트폴리오 핵심)
- [ ] Muzzle Flash (간단한 Niagara라도)
- [ ] Hit Impact (표면별 아니어도 OK)
- [ ] Hit Marker (UI)
- [ ] 피격 피드백(적 플래시/넉백/짧은 스턴 중 1개 이상)
- [ ] Camera Shake (약하게)

### 3) Enemy (1종이라도 '게임'처럼)
- [ ] Enemy 1종: Chase + Attack + Dead
- [ ] 피격/사망 처리 (HealthComponent 공용화)
- [ ] 스폰 5~10마리에도 프레임 안정 (Tick 최소화)

### 4) Minimal UI & Flow
- [ ] 체력 UI
- [ ] 탄약 UI
- [ ] Game Over → Restart
- [ ] Victory 조건(목표 지점 도달 / 웨이브 종료 중 하나)

### 5) Stage 1 제작
- [ ] 방 2~3개 + 장애물(커버) 배치
- [ ] 적 웨이브 또는 구간 스폰
- [ ] 시작→전투→클리어 흐름 완성

완료 기준:
- 처음 실행해서 5분 플레이해도 버그/막힘 없이 진행 가능
- 쏘는 맛(피드백)과 목표(클리어/게임오버)가 명확함

---

## Phase 2 — Polish Pass
### 1) 오디오/비주얼 정리
- [ ] BGM 1개, SFX 세트(발사/피격/사망/리로드)
- [ ] 간단한 화면 효과(피격 시 vignette/붉은 플래시)
- [ ] 적 사망 연출(간단한 dissolve/파티클/사운드)

### 2) 난이도/밸런스
- [ ] 적 체력/속도/데미지 튜닝
- [ ] 탄약/리로드 템포 조정
- [ ] 한 판(3~7분) 플레이 타임 목표

### 3) 사용자 경험
- [ ] 시작 화면(Play/Quit까지는 아니어도 Restart 안내)
- [ ] 튜토리얼 텍스트 2줄(이동/사격/리로드)

---

## Phase 3 — Portfolio Packaging (제출용 마감)
- [ ] 버그 정리 리스트 운영(치명/중간/사소)
- [ ] 성능 점검 (Stat Unit, Stat FPS, GPU/CPU 병목 확인)
- [ ] 조작감 마감 (입력 반응성/카메라 흔들림/시야)
- [ ] README(외부용) 정리 + 스크린샷/GIF 2~3개
- [ ] 1~2분 플레이 영상 캡처 (핵심 루프 보여주기)

---

## 운영 규칙 (문서/작업 관리)
- DevLog는 매일 종료 전에 10분 작성
- Roadmap은 "기능 단위 완료" 시에만 체크
- 새로운 아이디어는 즉시 넣지 말고 `Backlog`에만 기록

---

## Backlog (나중에 할것)
- [ ] 무기 2종/3종 확장
- [ ] 인벤토리/루팅
- [ ] 스토리/퀘스트
- [ ] 데이터 기반 전면 개편
