#Ally Squad AI System (Implementation)

##Overview

플레이어가 직접 명령(Move/Guard/Attack)을 내려 아군 동료를 제어하는 Squad 기반 AI 시스템.
Door로 방(RoomId)을 지정해 "방 정리(Room Clear)"를 수행하며, 여러 명의 동료가 함께 움직이는 것처럼 보이도록 집결/진입슬롯/타겟분산을 제공한다.

##Key Decisions

-경량 FSM 채택: Behavior Tree 없이 상태 기반(FSM)으로 단순 구현/디버깅.

-RoomId 기반 방 명령: Door/RoomVolume이 동일 RoomId를 공유하여 방 단위 명령을 단순화.

-중앙 분배(SquadCoordinator): EntrySlot/타겟 할당을 중앙에서 관리해 Overkill과 겹침을 최소화.

-아이템 회수는 플레이어: 아군은 적 처치만. 대신 무장 요약 리포트만 제공(Weapon DA 기반).

-자가치료는 Charges 기반: 인벤토리 없이 HealCharges + Cooldown으로만 처리.

##Architecture

-ATDAllyCharacter: 동료 캐릭터 (기존 BaseCharacter/HealthComponent 흐름 재사용)

-ATDAllyAIController: FSM 실행, MoveTo/Target/Fire 요청

-UTDAllyCommandComponent: 현재 명령/목표/타겟/RoomId 데이터 보관

-UTDSquadCoordinatorComponent: 스쿼드 명령 접수, 집결/슬롯/타겟 분배, 방 정리 완료 판정

-ATDRoomVolume: RoomId 기반 방 영역(Overlap) 정의

-ATDDoor: RoomId 보유, 클릭/명령 트리거

##핵심 원칙

-A는 B를 모른다: 캐릭터는 명령 데이터만 보유, 컨트롤러가 실행. 분배는 Coordinator에서.

-이벤트/쿨다운 기반: 탐색/갱신은 저주파(쿨다운)로만. Tick에서 매 프레임 탐색 금지.

-MoveTo 스팸 금지: 목표 변경 시에만 갱신.

##Trade-offs

-엄폐/EQS/포메이션/고급 전술 의사결정은 제외(스코프 통제).

-협동 전투의 "그럴듯함"을 집결/슬롯/타겟분산으로만 달성.

-드랍/인벤토리 연동은 하지 않음(무장 요약 리포트만).

##Future

-(구현 이후 품질 개선) 명령 큐(Shift), 역할 세분화(근/원거리), 위협도 기반 타겟 우선순위

-(필요 시) 동료 성장/관계/추모 등 메타 시스템은 별도 Future 문서로 관리

