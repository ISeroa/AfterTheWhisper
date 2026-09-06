# AfterTheWisper

> Unreal Engine 4.27과 C++로 개발한 아포칼립스 탐사형 탑다운 슈터 포트폴리오 프로젝트

## 프로젝트 설명

AfterTheWisper는 아포칼립스 이후의 폐쇄된 장소를 탐사하고, 적과 교전하며 중요 물품을 확보한 뒤 탈출하는 탑다운 슈터다.

현재 데모에서는 사무실 형태의 Office Floor를 탐색한다. 플레이어는 적을 피해하거나 처치하면서 OfficeKey를 획득하고, 비상계단의 장치를 활성화한 뒤 탈출 영역을 일정 시간 지켜야 한다. 탈출에 성공하거나 플레이어가 사망하면 결과 화면이 표시되며 같은 레벨을 다시 시작할 수 있다.

> Office Floor 진입 → 탐색 및 전투 → OfficeKey 획득 → 탈출 장치 활성화 → 탈출 영역 체류 → Extraction Success

## 개발 환경

- Unreal Engine 4.27
- C++
- Unreal Blueprint
- UMG
- Niagara
- Git / GitHub

## 현재 구현 기능

### 플레이어와 전투

- WASD 이동과 마우스 방향 조준
- 걷기와 달리기
- LineTrace 기반 단발 및 연사
- 탄창과 예비 탄약 관리
- 수동 및 탄창 소진 시 재장전
- 재장전 진행 UI
- 총구 화염, 탄피 배출과 충돌음
- 실내·실외 발사음 분리
- 체력 및 탄약 HUD
- 인벤토리 무게에 따른 이동속도 변화

### 무기 시스템

- UTDWeaponPresetDA 기반 무기 설정
- Frame과 Part로 구성된 모듈형 Static Mesh 조립
- 슬롯별 Mesh, Socket과 Offset 설정
- Damage, FireRate, Spread, ReloadTime 등 스탯의 DataAsset 관리
- Muzzle Flash, 사운드, 탄피 클래스 설정

### 적 AI

- Timer 기반 이동 목표 갱신
- 플레이어 직접 추격과 Encircle 이동 전술
- 거리 기반 플레이어 감지 및 추격 해제
- Windup과 Cooldown을 포함한 근접 공격
- 피격 강도에 따른 감속과 짧은 이동 중단
- Ragdoll 기반 사망 처리

### 시야 시스템

- 근거리 원형 시야와 전방 원뿔 시야 결합
- 전용 VisionObstacle Trace Channel
- BP_VisionOccluder를 이용한 시각 표현과 시야 차단 판정의 분리
- Ray Fan으로 Visibility Polygon 계산
- Render Target과 Post Process Material을 이용한 시야 밖 암전
- 적과 낮은 가구가 불필요하게 시야를 자르지 않도록 분리된 충돌 정책
- 동적 Actor의 시야 여부에 따른 표시·숨김

### 아이템과 상호작용

- ITDInteractableInterface 기반 공통 상호작용
- E 키를 사용하는 월드 아이템 획득
- 슬롯과 Stack 기반 인벤토리
- 아이템 보유 여부 조회
- 아이템 무게 합산과 플레이어 이동속도 반영
- OfficeKey 중요 물품

### 탈출과 게임 흐름

- 탈출 조건 검사와 탈출 영역 판정의 책임 분리
- Activator에서 맵에 배치된 Extraction Zone 인스턴스 참조
- OfficeKey 보유 시 탈출 지점 활성화
- Box Overlap 기반 탈출 영역 진입·이탈 감지
- Timer 기반 체류 판정과 이탈 시 진행 취소
- 상호 배타적인 승리·패배 상태
- Extraction Success 및 Game Over UI
- 결과 화면 중 게임 일시정지
- Restart 후 게임 입력과 마우스 커서 상태 복구

### Office Floor 데모 맵

- 로비, 중앙 사무실, 업무 공간, 탕비실, 자료실, 통로, 화장실과 비상계단으로 구성된 실내 탐사 공간
- 선택적으로 둘러볼 수 있는 보조 공간과 복귀 동선
- 전용 Vision Occluder와 Nav Mesh 배치
- 탐색, 전투, 중요 물품 획득과 탈출로 이어지는 플레이 가능한 루프

## 핵심 기술

### 데이터 기반 무기 구성

무기의 스탯, 파츠, 사운드, 이펙트와 탄피 클래스를 UTDWeaponPresetDA에서 관리한다. 무기 Actor는 Preset을 단일 설정 소스로 사용하며 파츠를 슬롯 단위로 조립한다.

### 공통 상호작용 인터페이스

플레이어는 구체적인 Pickup이나 Activator 타입을 직접 알지 않는다. 대상이 ITDInteractableInterface를 구현했는지 확인한 뒤 Interact를 호출해 상호작용 대상과 플레이어의 의존성을 낮췄다.

### 탈출 책임 분리

ATDExtractionActivator는 OfficeKey 같은 활성화 조건을 검사하고, ATDExtractionZone은 활성 상태와 체류 완료만 관리한다. Activator가 맵에 배치된 Zone 인스턴스의 ActivateExtraction을 호출하는 단방향 구조다.

### Timer 기반 반복 처리

발사, 재장전, AI 이동 갱신, 근접 공격, 탈출 체류 판정은 필요한 주기에 맞춘 Timer를 사용한다. 매 프레임 실행할 필요가 없는 기능에서 Tick 사용을 줄였다.

### 렌더링과 게임플레이 시야 분리

실제 Mesh 및 물리 Collision과 시야 차단 Collision을 분리했다. Visibility Polygon은 전용 Occluder만 추적하며, Render Target과 Post Process Material이 시야 밖의 화면을 어둡게 표현한다.

### 이벤트 기반 UI

탄약, 재장전, 체력과 게임 결과는 Delegate 또는 Blueprint Event를 통해 UI에 전달한다. 결과 화면은 GameMode의 승리·패배 상태를 기준으로 표시된다.

## 조작법

| 입력 | 동작 |
|---|---|
| W, A, S, D | 이동 |
| 마우스 | 조준 |
| 마우스 왼쪽 버튼 | 발사 |
| R | 재장전 |
| Left Shift | 달리기 |
| E | 아이템 획득 및 장치 상호작용 |
| 결과 화면의 Restart | 현재 레벨 다시 시작 |

## 실행 및 빌드

프로젝트는 Unreal Engine 4.27을 사용한다.

1. TopdownShooter.uproject를 Unreal Engine 4.27로 연다.
2. 필요하면 Visual Studio 프로젝트 파일을 생성한다.
3. L_OfficeFloor를 열고 Play한다.

Windows 개발 빌드:

    build.bat

## 주요 문서

- [Roadmap](docs/Roadmap.md)
- [Development Log](docs/DevLog.md)
- [Extraction Loop](docs/future/extraction-loop.md)
- [Interaction System](docs/future/interaction-system.md)
- [Vision System](docs/systems/gameplay/vision-system.md)
- [Enemy System](docs/systems/ai/Enemy-System.md)
- [Weapon System](docs/systems/combat/Weapon-System.md)

## 참고

본 프로젝트는 개인 포트폴리오 목적으로 개발하고 있다. 외부 에셋은 라이선스를 확인한 뒤 사용하며, 저장소에 포함하지 않는 에셋은 프로젝트의 .gitignore 정책에 따라 관리한다.
