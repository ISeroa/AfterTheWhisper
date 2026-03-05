# Enemy Audio Presence System (Off-Screen Threat)

## Overview
시야 시스템에서 적이 시야 밖일 경우 메쉬는 완전히 보이지 않도록 처리하되, 적의 존재를 소리로 전달하여 탐사 긴장감을 유지하는 시스템.
어두운 건물 탐사 환경에서 “보이지 않지만 근처에 있다”는 정보 신호를 제공해 공포/스릴을 강화한다.

## Key Decisions
- 적이 시야 밖이면 완전 비가시 처리 (Mesh/Actor Hidden)
- 비가시 상태에서도 적 사운드는 재생 가능
- 적 사운드는 2 레이어로 분리
  - Ambient(Idle): “근처 존재” 신호
  - Foley(Action): “이동/접근” 신호 (발걸음 등)
- 정보 과다/소음 난장 방지를 위해 동시 재생 제한을 둔다
- 플레이어 Noise 시스템(총소리 등)과 적 Ambient 사운드는 목적이 다르므로 분리하여 설계한다

## Architecture
### Runtime Flow
- Visibility System이 적의 가시 상태를 결정
  - Visible: 메쉬 표시
  - Not Visible: 메쉬 숨김(완전 비가시)
- Enemy Audio Presence는 가시 상태와 무관하게 동작
  - 단, 플레이어로부터 너무 멀면 비용 절감을 위해 비활성(거리 게이트)

### Audio Layers
1) Ambient(Idle) Layer
- 목적: 적의 “존재”를 간헐적으로 알림 (신음/숨소리/속삭임/무전 등)
- 트리거: 타이머 기반 랜덤 간격 재생
- 제어: 쿨다운 + 랜덤 지터로 반복감 감소

2) Foley(Action) Layer
- 목적: “이동/접근” 정보를 제공 (발걸음/장비 소리/옷 스침 등)
- 트리거(선호): 애니메이션 노티파이 기반
- 트리거(대안): 이동 시작/종료 이벤트 기반의 루프/원샷 사운드

### Control Gates
- Distance Gate: 플레이어 반경 밖 적은 오디오 업데이트/재생 제한
- Concurrency Gate: 근처 영역 내 “동시에 재생 가능한 Ambient 사운드 개수” 상한
- Cooldown Gate: 개체별/그룹별 최소 재생 간격 유지

### Tuning Targets (Concept)
- Ambient
  - 긴장 유지용: 너무 잦지 않게(피로도 방지), 너무 드물지 않게(정보 제공)
- Foley
  - 접근/회피 판단을 돕는 수준으로만 제공(정확한 위치 핀포인트 금지)

## Trade-offs
- (장점) 시야 제한으로 발생할 수 있는 답답함을 청각 정보로 보완
- (장점) 공포/스릴 강화: “보이지 않는데 들린다”는 심리적 압박
- (장점) 시스템 복잡도 대비 체감 효과가 큼
- (단점) 동시 재생 제어가 없으면 다수 적 상황에서 소리 혼잡 가능
- (단점) 소리가 지나치게 정확하면 공포가 약해지고, 지나치게 랜덤이면 피로/불쾌 가능
- (단점) 벽 차폐/리버브 등 고급 음향 모델링은 범위 확장 시 비용 증가

## Future
- Zone/Room 기반 Concurrency (구역별 동시 재생 상한)
- 오디오 Occlusion/Obstruction(벽/문 차폐) 단계적 적용
- 적 타입별 “시그니처 사운드” 설계 (좀비/근접 생존자/원거리 생존자)
- 생존자 손전등/무전 사운드와 Alert Level 연동 강화
- 플레이어 상태(체력/탄약/배터리)에 따른 연출형 Ambient 빈도 조절 (Director-lite)
